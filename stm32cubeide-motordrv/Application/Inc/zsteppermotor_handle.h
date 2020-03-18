#ifndef _ZSTEPPERMOTOR_H__
#define _ZSTEPPERMOTOR_H__
#include <zgblpara.h>
//步进电机移动方向
typedef enum
{
	Leftward = 0,//向左移动
	Rightward = 1,//向右移动
}StepperMoveDirection;
//步进电机位置
typedef enum
{
	Leftmost = 0,//最左边
	Middle = 1,//中间位置
	Rightmost = 2,//最右边
}StepperPosition;
	
struct stepperDevice
{
	TIM_HandleTypeDef *pwmTIM;
	uint32_t pwmCH;
	TIM_HandleTypeDef *cascadeTIM;
	
	GPIO_TypeDef *dirPort;
	uint16_t dirPin;
	
	GPIO_TypeDef *limitPort;
	uint16_t limitPin;
	
	GPIO_TypeDef *sleepPort;
	uint16_t sleepPin;
	
	uint16_t step;
	StepperMoveDirection direction;
	StepperPosition position;
	StepperPosition last_position;
};
struct Bracket2D
{
	struct stepperDevice lftRhtStepper;
	struct stepperDevice upDownStepper;
};
extern struct Bracket2D g_Bracket2D;

extern void zsy_Bracket2DInit();
//whichMotor:  1:Left/Right Motor, 2:Up/Down Motor.
extern void zsy_Bracket2DMove(uint8_t whichMotor,uint32_t moveIncrease);
extern void zsy_Bracket2DStopLftRhtStepperMotor();
extern void zsy_Bracket2DStopUpDownStepperMotor();
extern void zsy_Bracket2DLimitSwitchCallback(uint16_t gpioPin);
#endif //_ZSTEPPERMOTOR_H__