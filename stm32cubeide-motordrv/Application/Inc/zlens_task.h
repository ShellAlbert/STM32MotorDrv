

#ifndef _ZLENS_HANDLE_H__
#define _ZLENS_HANDLE_H__
#include "zgblpara.h"


struct pidDevice
{
	float			maxOut;
	float			inteLimit;
	float 			inputMaxErr;

	float			kP;
	float			kI;
	float			kD;

	float			setV;
	float			getV;

	float			err;
	float			last_err;

	float			pOut;
	float			iOut;
	float			dOut;
	float			out;
};


struct motorDevice
{
	uint8_t 		goto_zp_flag;
	uint8_t 		nArrivedZPFlag;
	uint16_t		zero_point;
	uint16_t		targetEncoder;
	TIM_HandleTypeDef * encoder_tim_handle;
	TIM_HandleTypeDef * pwm_tim_handle;
	uint32_t		pwm_channel;
	int16_t 		currentEncoder;
};


struct lensDevice
{
	uint16_t		uLftMotorZeroPoint; 			//zero point encoder value.
	struct motorDevice lftMotor;
	struct pidDevice pidLft;
	uint16_t		uRhtMotorZeroPoint; 			//zero point encoder value.
	struct motorDevice rhtMotor;
	struct pidDevice pidRht;
	uint8_t 		autoFocusFlag;
};


extern struct lensDevice lensDev;
extern void zsy_LensInit();
extern void zsy_LensTaskLoop(void const * argument);
extern float zsy_PIDCalculate(struct pidDevice *pidDev, float getV, float setV);
extern void zsy_LftMotorGotPeakTorqueCurrentCallback(void);
extern void zsy_RhtMotorGotPeakTorqueCurrentCallback(void);
extern void zsy_LensThreadResume(void);
//左右电机零点时的编码器值
#define LFT_MOTOR_ZEROPOINT_ENCODER_VALUE	8000
#define RHT_MOTOR_ZEROPOINT_ENCODER_VALUE	10000
#endif //_ZLENS_HANDLE_H__
