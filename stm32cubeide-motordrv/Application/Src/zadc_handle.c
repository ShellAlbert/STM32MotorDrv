

/*
 * filename:zadc_handle.c
 * versin:0.1.0
 * author:1811543668@qq.com
 * date: 2020/3/16
 */
#include <zadc_handle.h>
#include <zdataprocess_task.h>
#include "cmsis_os.h"
#include "drv_uart.h"
#include "package.h"
#include "adc.h"
#include "usart.h"
#include "zgblpara.h"

uint8_t 		fiber_H[9] =
{
	0x53, 0x4F, 0x50, 0x3D, 0x32, 0x37, 0x30, 0x0A, 0x0D
};


uint8_t 		fiber_L[9] =
{
	0x53, 0x4F, 0x50, 0x3D, 0x31, 0x37, 0x30, 0x0A, 0x0D
};


uint8_t 		fiber_flg = 0;


void zsy_ADCInit(void)
{
	//initial code generated by STM32CubeMX.
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_ADC3_Init();

	//start ADC1 with DMA,ADC1 releated channel sample data will be saved to g_ADC1DMABuffer.
	//g_ADC1DMABuffer[]
	//NTC0,BatteryVoltage0,RSSI0,NTC1,BatteryVoltage1,RSSI1, ......
	//	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) g_ADC1DMABuffer, ADC1_CHANNEL_NUM * ADC1_CHANNEL_SAMPLES);

	//start ADC2 with IT(Interrupt).
	HAL_ADC_Start(&hadc2);

	//start ADC3 with IT(Interrupt).
	HAL_ADC_Start(&hadc3);
}


void zsy_ParseADC1DMAData(void)
{

	uint8_t 		i, j = 0;

	//calculate each channel's average value.
	for (j = 0; j < ADC1_CHANNEL_NUM; j++)
		{
		uint32_t		sum = 0;

		for (i = 0; i < ADC1_CHANNEL_SAMPLES; i++)
			{
			sum 				+= g_ADC1DMABuffer[i][j];
			}

		g_ADC1AverageValue[j] = sum / ADC1_CHANNEL_SAMPLES;
		}

	//auto setup up optical amplifier according the ADC value.
	if ((g_ADC1AverageValue[0] > 3000) && (fiber_flg == 0))
		{
		HAL_UART_Transmit(&huart4, &fiber_L[0], 9, 1);
		fiber_flg			= 1;
		}
	else if ((g_ADC1AverageValue[0] < 3000) && (fiber_flg == 1))
		{
		HAL_UART_Transmit(&huart4, &fiber_H[0], 9, 1);
		fiber_flg			= 0;
		}

	//save battery voltage to global variable.
	g_BatteryVoltage	= g_ADC1AverageValue[Battery_Voltage_ADC1_CH10];

	//save RSSI to global variable.
	g_RSSI				= g_ADC1AverageValue[RSSI1_ADC1_CH12];
}


//STM32 AWD(analog watchdog) monitor the voltage level between [min,max].
//if it overflow then trigger interrupt,then call back this function.
//I used it to minitor two DC Motors current.
//when Motor current reach threshold,it means it should be stop,it reach the limited position.
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef * hadc)
{
	//ADC2-CH8: Right Motor Current Analog WatchDog.
	//ADC3-CH3: Left Motor Current Analog WatchDog.
	if (hadc->Instance == ADC2)
		{
		if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_AWD) != RESET)
			{
			zsy_RhtMotorGotPeakTorqueCurrentCallback();
			}
		}
	else if (hadc->Instance == ADC3)
		{
		if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_AWD) != RESET)
			{
			zsy_LftMotorGotPeakTorqueCurrentCallback();
			}
		}
}


