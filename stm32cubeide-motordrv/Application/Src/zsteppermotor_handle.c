
#include <zsteppermotor_handle.h>
#include "drv_flash.h"
#include "tim.h"
struct Bracket2D g_Bracket2D;


void zsy_Bracket2DInit()
{

	flash_pack_u	flash_pack;
	flash_pack.data 	= find_used_entry();

	//left-right direction driving stepper motor.
	g_Bracket2D.lftRhtStepper.pwmTIM = &htim2;
	g_Bracket2D.lftRhtStepper.pwmCH = TIM_CHANNEL_1;
	g_Bracket2D.lftRhtStepper.cascadeTIM = &htim4;

	g_Bracket2D.lftRhtStepper.dirPort = Stepper_LR_Dir_GPIO_Port;
	g_Bracket2D.lftRhtStepper.dirPin = Stepper_LR_Dir_Pin;

	g_Bracket2D.lftRhtStepper.limitPort = LR_Limit_Switch_GPIO_Port;
	g_Bracket2D.lftRhtStepper.limitPin = LR_Limit_Switch_Pin;

	g_Bracket2D.lftRhtStepper.sleepPort = Stepper_LR_Sleep_GPIO_Port;
	g_Bracket2D.lftRhtStepper.sleepPin = Stepper_LR_Sleep_Pin;

	g_Bracket2D.lftRhtStepper.last_position = Middle;
	g_Bracket2D.lftRhtStepper.position = flash_pack.buff[2];


	//up-down direction driving stepper motor.
	g_Bracket2D.upDownStepper.pwmTIM = &htim8;
	g_Bracket2D.upDownStepper.pwmCH = TIM_CHANNEL_2;
	g_Bracket2D.upDownStepper.cascadeTIM = NULL;

	g_Bracket2D.upDownStepper.dirPort = Stepper_Up_Dir_GPIO_Port;
	g_Bracket2D.upDownStepper.dirPin = Stepper_Up_Dir_Pin;

	g_Bracket2D.upDownStepper.limitPort = UD_Limit_Switch_GPIO_Port;
	g_Bracket2D.upDownStepper.limitPin = UD_Limit_Switch_Pin;

	g_Bracket2D.upDownStepper.sleepPort = Stepper_UD_Sleep_GPIO_Port;
	g_Bracket2D.upDownStepper.sleepPin = Stepper_UD_Sleep_Pin;

	g_Bracket2D.upDownStepper.last_position = Middle;
	g_Bracket2D.upDownStepper.position = flash_pack.buff[1];

}


//whichMotor:  1:Left/Right Motor, 2:Up/Down Motor.
void zsy_Bracket2DMove(uint8_t whichMotor, uint32_t moveIncrease)
{
	GPIO_PinState	pin_state;

	struct stepperDevice * pStepperMotor = NULL;

	if (whichMotor == 1)
		{
		pStepperMotor		= &g_Bracket2D.lftRhtStepper;
		}
	else if (whichMotor == 2)
		{
		pStepperMotor		= &g_Bracket2D.upDownStepper;
		}


	if (moveIncrease >= 0) //positive value means move to Left.
		{
		if (pStepperMotor->position == Leftmost) //current position is Leftmost,no need to move.
			{
			return;
			}

		pStepperMotor->step += moveIncrease;
		pStepperMotor->direction = Leftward;
		pin_state			= GPIO_PIN_RESET;
		}
	else if (moveIncrease < 0) //negative value means move to right.
		{
		if (pStepperMotor->position == Rightmost) //current position is Rightmost,no need to move.
			{
			return;
			}

		pStepperMotor->step -= abs(moveIncrease);
		pStepperMotor->direction = Rightward;
		pin_state			= GPIO_PIN_SET;
		}

	//set Stepper Motor Drive IC Sleep Pin to HIGH to disable SLEEP mode.
	HAL_GPIO_WritePin(pStepperMotor->sleepPort, pStepperMotor->sleepPin, GPIO_PIN_SET);

	//set Stepper Motor Drive IC Dir Pin.
	HAL_GPIO_WritePin(pStepperMotor->dirPort, pStepperMotor->dirPin, pin_state);

	//两种方法产生一定数量的PWM脉冲
	//方法1：使用高级定时器的RCR(重复计数器),当RCR减到0时,停止输出脉冲.
	//方法2：使用另一个定时器来辅助，当定时器溢出中断时,停止输出脉冲.
	if (pStepperMotor->cascadeTIM == NULL)
		{
		pStepperMotor->pwmTIM->Instance->RCR = 250;
		}
	else 
		{
		__HAL_TIM_SET_AUTORELOAD(pStepperMotor->cascadeTIM, moveIncrease);
		}

	HAL_TIM_PWM_Start(pStepperMotor->pwmTIM, pStepperMotor->pwmCH);
	return;
}

void zsy_Bracket2DStopLftRhtStepperMotor()
{
	//1.stop pwm output.
	HAL_TIM_PWM_Stop(g_Bracket2D.lftRhtStepper.pwmTIM, g_Bracket2D.lftRhtStepper.pwmCH);
	//2.set Sleep Pin to 0 to save power.
	HAL_GPIO_WritePin(g_Bracket2D.lftRhtStepper.sleepPort, g_Bracket2D.lftRhtStepper.sleepPin, GPIO_PIN_RESET);
}

void zsy_Bracket2DStopUpDownStepperMotor()
{
	static uint8_t nRCRCnt=0;
	if(nRCRCnt++ >= 2)
	{
		nRCRCnt = 0;
		//1.stop pwm output.
		HAL_TIM_PWM_Stop(g_Bracket2D.upDownStepper.pwmTIM, g_Bracket2D.upDownStepper.pwmCH);
		//2.set Sleep Pin to 0 to save power.
		HAL_GPIO_WritePin(g_Bracket2D.upDownStepper.sleepPort,g_Bracket2D.upDownStepper.sleepPin,GPIO_PIN_RESET);
	}
}
