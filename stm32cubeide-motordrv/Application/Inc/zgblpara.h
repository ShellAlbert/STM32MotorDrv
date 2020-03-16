

#ifndef _ZGBLPARA_H__
#define _ZGBLPARA_H__
#include <stdio.h>

//ADC1-Channel4:	NTC
//ADC1-CHannel10:	BatteryVoltage
//ADC1-Channel12: 	RSSI
#define ADC1_CHANNEL_NUM 		3
#define ADC1_CHANNEL_SAMPLES 	50
extern uint16_t g_ADC1DMABuffer[ADC1_CHANNEL_SAMPLES][ADC1_CHANNEL_NUM];
extern uint16_t g_ADC1AverageValue[ADC1_CHANNEL_NUM];

//the battery voltage measured by ADC1-CH10.
extern uint32_t g_BatteryVoltage;

//the distance value measured by LaserDistanceMeasure module.
extern uint32_t g_LaserDistance;

//crc32 check.
extern uint32_t g_CRC32Table[];
extern uint32_t g_CRC32Calculate(uint8_t* s, int len);


//GPIO define.
#include "stm32f1xx_hal.h"

#define Laser_Fiber_En_Pin GPIO_PIN_2
#define Laser_Fiber_En_GPIO_Port GPIOE
#define AOM1_En_Pin GPIO_PIN_3
#define AOM1_En_GPIO_Port GPIOE
#define AOM2_En_Pin GPIO_PIN_4
#define AOM2_En_GPIO_Port GPIOE
#define Charge_Test_Pin GPIO_PIN_5
#define Charge_Test_GPIO_Port GPIOE
#define Battery_Voltage_ADC1_CH10_Pin GPIO_PIN_0
#define Battery_Voltage_ADC1_CH10_GPIO_Port GPIOC
#define RSSI1_ADC1_CH12_Pin GPIO_PIN_2
#define RSSI1_ADC1_CH12_GPIO_Port GPIOC
#define M1_RS422_A_Pin GPIO_PIN_0
#define M1_RS422_A_GPIO_Port GPIOA
#define M1_RS422_B_Pin GPIO_PIN_1
#define M1_RS422_B_GPIO_Port GPIOA
#define M2_ADC3_CH3_AWD_Pin GPIO_PIN_3
#define M2_ADC3_CH3_AWD_GPIO_Port GPIOA
#define NTC_ADC1_CH4_Pin GPIO_PIN_4
#define NTC_ADC1_CH4_GPIO_Port GPIOA
#define Stepper_LR_Sleep_Pin GPIO_PIN_7
#define Stepper_LR_Sleep_GPIO_Port GPIOA
#define Sleep2_Pin GPIO_PIN_4
#define Sleep2_GPIO_Port GPIOC
#define M1_ADC2_CH8_AWD_Pin GPIO_PIN_0
#define M1_ADC2_CH8_AWD_GPIO_Port GPIOB
#define Sleep1_Pin GPIO_PIN_7
#define Sleep1_GPIO_Port GPIOE
#define M1_PWM_OUT__Pin GPIO_PIN_8
#define M1_PWM_OUT__GPIO_Port GPIOE
#define M1_PWM_OUT_E9_Pin GPIO_PIN_9
#define M1_PWM_OUT_E9_GPIO_Port GPIOE
#define M2_PWM_OUT__Pin GPIO_PIN_10
#define M2_PWM_OUT__GPIO_Port GPIOE
#define M2_PWM_OUT_E11_Pin GPIO_PIN_11
#define M2_PWM_OUT_E11_GPIO_Port GPIOE
#define M_Protect_OUT__Pin GPIO_PIN_12
#define M_Protect_OUT__GPIO_Port GPIOE
#define M_Protect_OUT_E13_Pin GPIO_PIN_13
#define M_Protect_OUT_E13_GPIO_Port GPIOE
#define LR_Limit_Switch_Pin GPIO_PIN_12
#define LR_Limit_Switch_GPIO_Port GPIOB
#define LR_Limit_Switch_EXTI_IRQn EXTI15_10_IRQn
#define UD_Limit_Switch_Pin GPIO_PIN_13
#define UD_Limit_Switch_GPIO_Port GPIOB
#define UD_Limit_Switch_EXTI_IRQn EXTI15_10_IRQn
#define Ceju_UART3_TX_Pin GPIO_PIN_8
#define Ceju_UART3_TX_GPIO_Port GPIOD
#define Ceju_UART3_RX_Pin GPIO_PIN_9
#define Ceju_UART3_RX_GPIO_Port GPIOD
#define Stepper_LR_Dir_Pin GPIO_PIN_11
#define Stepper_LR_Dir_GPIO_Port GPIOD
#define Stepper_Protect_Sleep_Pin GPIO_PIN_12
#define Stepper_Protect_Sleep_GPIO_Port GPIOD
#define Stepper_UD_Sleep_Pin GPIO_PIN_13
#define Stepper_UD_Sleep_GPIO_Port GPIOD
#define Stepper_Up_Dir_Pin GPIO_PIN_14
#define Stepper_Up_Dir_GPIO_Port GPIOD
#define Stepper_Protect_Dir_Pin GPIO_PIN_15
#define Stepper_Protect_Dir_GPIO_Port GPIOD
#define Stepper_UD_Step_Pin GPIO_PIN_7
#define Stepper_UD_Step_GPIO_Port GPIOC
#define Jetson_UART1_TX_Pin GPIO_PIN_9
#define Jetson_UART1_TX_GPIO_Port GPIOA
#define Jetson_UART1_RX_Pin GPIO_PIN_10
#define Jetson_UART1_RX_GPIO_Port GPIOA
#define Stepper_LR_Step_Pin GPIO_PIN_15
#define Stepper_LR_Step_GPIO_Port GPIOA
#define Fiber_UART4_TX_Pin GPIO_PIN_10
#define Fiber_UART4_TX_GPIO_Port GPIOC
#define Fiber_UART4_RX_Pin GPIO_PIN_11
#define Fiber_UART4_RX_GPIO_Port GPIOC
#define TestPin_Pin GPIO_PIN_5
#define TestPin_GPIO_Port GPIOD
#define M62429_D_Pin GPIO_PIN_6
#define M62429_D_GPIO_Port GPIOD
#define M62429_CLK_Pin GPIO_PIN_7
#define M62429_CLK_GPIO_Port GPIOD
#define M2_RS422_B_Pin GPIO_PIN_4
#define M2_RS422_B_GPIO_Port GPIOB
#define M2_RS422_A_Pin GPIO_PIN_5
#define M2_RS422_A_GPIO_Port GPIOB

#endif

