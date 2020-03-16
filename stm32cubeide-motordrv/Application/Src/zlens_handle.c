
#include <zlens_handle.h>
#include "tim.h"
#include "cmsis_os.h"
extern osThreadId LensTaskHandle;

struct lensDevice lensDev;


void zsy_LensInit()
{
	//TIM1-CH1/CH1N: DC Motor1 PWM Out+/Out- (Left Lens).
	//TIM3-CH1:Encoder1 A
	//TIM3-CH2:Encoder1 B
	lensDev.lftMotor.pwm_tim_handle = &htim1;
	lensDev.lftMotor.pwm_channel = TIM_CHANNEL_2;
	lensDev.lftMotor.encoder_tim_handle = &htim3;
	lensDev.lftMotor.goto_zp_flag = 1;
	lensDev.pidLft.maxOut = 500;
	lensDev.pidLft.inteLimit = 0;
	lensDev.pidLft.inputMaxErr = 0;
	lensDev.pidLft.kP	= 1.5;
	lensDev.pidLft.kI	= 0;
	lensDev.pidLft.kD	= 0;


	//TIM1-CH2/CH2N: DC Motor2 PWM Out+/Out- (Right Lens).	
	//TIM5-CH1:Encoder2 A
	//TIM5-CH2:Encoder2 B
	lensDev.rhtMotor.pwm_tim_handle = &htim1;
	lensDev.rhtMotor.pwm_channel = TIM_CHANNEL_1;
	lensDev.rhtMotor.encoder_tim_handle = &htim5;
	lensDev.rhtMotor.goto_zp_flag = 1;
	lensDev.pidRht.maxOut = 500;
	lensDev.pidRht.inteLimit = 0;
	lensDev.pidRht.inputMaxErr = 0;
	lensDev.pidRht.kP	= 1.5;
	lensDev.pidRht.kI	= 0;
	lensDev.pidRht.kD	= 0;

	//
	lensDev.autoFocusFlag = 0;


	//TIM1-CH1/CH1N: DC Motor1 PWM Out+/Out- (Left Lens).
	//TIM1-CH2/CH2N: DC Motor2 PWM Out+/Out- (Right Lens).
	//TIM1-CH3/CH3N: DC Motor3 PWM Out+/Out- (Tamper).
	//上电即启动PWM,通过ADC堵转电流停止。
	//用于上电时电机复位，寻找零点。
	MX_TIM1_Init();
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
	
	//TIM3-CH1:Encoder1 A
	//TIM3-CH2:Encoder1 B
	MX_TIM3_Init();
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2);
	
	//TIM5-CH1:Encoder2 A
	//TIM5-CH2:Encoder2 B
	MX_TIM5_Init();
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_2);
}


void zsy_LensThreadLoop(void const * argument)
{
	uint32_t		period = osKernelSysTick();

	while (1)
		{
		float			fMotorOut;
		uint32_t		currentValue;

		//left motor.
		currentValue		= __HAL_TIM_GET_COUNTER(&htim3);
		fMotorOut			= zsy_PIDCalculate(&lensDev.pidLft, currentValue, lensDev.lftMotor.target);
		lensDev.lftMotor.current = fMotorOut;
		__HAL_TIM_SET_COMPARE(lensDev.lftMotor.pwm_tim_handle, lensDev.lftMotor.pwm_channel, 500 - fMotorOut);

		//right motor.
		currentValue		= __HAL_TIM_GET_COUNTER(&htim5);
		fMotorOut			= zsy_PIDCalculate(&lensDev.pidRht, currentValue, lensDev.rhtMotor.target);
		lensDev.rhtMotor.current = fMotorOut;
		__HAL_TIM_SET_COMPARE(lensDev.rhtMotor.pwm_tim_handle, lensDev.rhtMotor.pwm_channel, 500 - fMotorOut);

		//schedule after 5ms.
		osDelayUntil(&period, 5);
		}
}


void zsy_AbsLimit(float * a, float ABS_MAX)
{
	if (*a > ABS_MAX)
		{
		*a					= ABS_MAX;
		}

	if (*a < -ABS_MAX)
		{
		*a					= -ABS_MAX;
		}
}


/**
  * @brief	   calculate delta PID and position PID
  * @param[in] pid: control pid struct
  * @param[in] get: measure feedback value
  * @param[in] set: target value
  * @retval    pid calculate output 
  */
float zsy_PIDCalculate(struct pidDevice * pidDev, float getV, float setV)
{
	pidDev->getV		= getV;
	pidDev->setV		= setV;
	pidDev->err 		= setV - getV;

	if ((pidDev->inputMaxErr != 0) && (fabs(pidDev->err) > pidDev->inputMaxErr))
		{
		return 0;
		}

	pidDev->pOut		= pidDev->kP * pidDev->err;
	pidDev->iOut		+= pidDev->kI * pidDev->err;
	pidDev->dOut		= pidDev->kD * (pidDev->err - pidDev->last_err);

	zsy_AbsLimit(& (pidDev->iOut), pidDev->inteLimit);
	pidDev->out 		= pidDev->pOut + pidDev->iOut + pidDev->dOut;
	zsy_AbsLimit(& (pidDev->out), pidDev->maxOut);

	return pidDev->out;
}


void zsy_LftMotorGotPeakTorqueCurrentCallback(void)
{
	//左电机堵转电流引发模拟看门狗中断，当发生10000次时可以认为100%到达零点了
	static uint16_t nGotPeakTimes = 0;

	if (lensDev.lftMotor.goto_zp_flag)
		{
		if (nGotPeakTimes++ >= 10000)
			{
			nGotPeakTimes		= 0;
			lensDev.lftMotor.nArrivedZPFlag = 1;
			lensDev.lftMotor.goto_zp_flag = 0;

			//到达机械零点后，再向后转500个单位，作为逻辑零点
			__HAL_TIM_SET_COMPARE(lensDev.lftMotor.pwm_tim_handle, lensDev.lftMotor.pwm_channel, 500);

			//只有左右电机都到达零点了，说明逻辑零点都找到了，
			//然后才唤醒LensThread线程
			if (lensDev.lftMotor.nArrivedZPFlag && lensDev.rhtMotor.nArrivedZPFlag)
				{
				lensDev.lftMotor.nArrivedZPFlag = 0;
				zsy_LensThreadResume();
				}
			}
		}

}


void zsy_RhtMotorGotPeakTorqueCurrentCallback(void)
{
	//右电机堵转电流引发模拟看门狗中断，当发生10000次时可以认为100%到达零点了
	static uint16_t nGotPeakTimes = 0;

	if (lensDev.rhtMotor.goto_zp_flag)
		{
		if (nGotPeakTimes++ >= 10000)
			{
			nGotPeakTimes		= 0;
			lensDev.rhtMotor.nArrivedZPFlag = 1;
			lensDev.rhtMotor.goto_zp_flag = 0;

			//到达机械零点后，再向后转500个单位，作为逻辑零点
			__HAL_TIM_SET_COMPARE(lensDev.rhtMotor.pwm_tim_handle, lensDev.rhtMotor.pwm_channel, 500);

			//只有左右电机都到达零点了，说明逻辑零点都找到了，
			//然后才唤醒LensThread线程
			if (lensDev.lftMotor.nArrivedZPFlag && lensDev.rhtMotor.nArrivedZPFlag)
				{
				lensDev.rhtMotor.nArrivedZPFlag = 0;
				zsy_LensThreadResume();
				}
			}
		}
}


void zsy_LensThreadResume(void)
{
	if (!lensDev.autoFocusFlag)
		{
		lensDev.lftMotor.target = LFT_MOTOR_ZEROPOINT_ENCODER_VALUE;
		lensDev.rhtMotor.target = RHT_MOTOR_ZEROPOINT_ENCODER_VALUE;
		}

	//reset encoder value.
	__HAL_TIM_SET_COUNTER(lensDev.lftMotor.encoder_tim_handle, 100);
	__HAL_TIM_SET_COUNTER(lensDev.rhtMotor.encoder_tim_handle, 100);
	osThreadResume(LensTaskHandle);
}


