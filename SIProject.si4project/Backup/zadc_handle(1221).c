/*
 * filename:zadc_handle.c
 * versin:0.1.0
 * author:1811543668@qq.com
 * date: 2020/3/16
 */

#include <zadc_handle.h>
#include "communicate.h"
#include "cmsis_os.h"
#include "drv_uart.h"
#include "package.h"

#include "usart.h"
#include "zgblpara.h"

static adc_upload_handler_t adc_protocol_upload;

uint8_t fiber_H[9]={0x53,0x4F,0x50,0x3D,0x32,0x37,0x30,0x0A,0x0D};
uint8_t fiber_L[9]={0x53,0x4F,0x50,0x3D,0x31,0x37,0x30,0x0A,0x0D};
uint8_t fiber_flg=0;

void zsy_ADCInit(void)
{
	adc1_manage_init();
	adc2_manage_init();
	adc3_manage_init();
//	adc1_conv_cplt_callback_register(adh_adc_dma_data_handle);
}

void adh_adc_dma_data_handle(uint8_t *buf, uint32_t len)
{
	memcpy(g_ADC1DMABuffer,buf,len);
	osSignalSet(CommunicateTaskHandle, RECV_ADC_DMA_SIGNAL);
}

void adc_upload_data_register(adc_upload_handler_t upload_t)
{
	adc_protocol_upload = upload_t;
}

void adh_prase_adc_dam_data(void)
{
	uint32_t sum = 0;
	uint8_t i,j = 0;
	upload_attr_t upload_attr;
	for( j=0;j<ADC1_CHANNEL_NUM;j++ )
	{
		for( i=0;i<ADC1_CHANNEL_SAMPLES;i++)
		{
			sum += g_ADC1DMABuffer[i][j];
		}
		g_ADC1AverageValue[j] = sum/ADC1_CHANNEL_SAMPLES;
		sum = 0;
	}
	if((g_ADC1AverageValue[0]>3000)&&(fiber_flg==0))
	{
		  HAL_UART_Transmit(&huart4,&fiber_L[0],9,1);
		  fiber_flg=1;
	}
	else if((g_ADC1AverageValue[0]<3000)&&(fiber_flg==1))
	{
		  HAL_UART_Transmit(&huart4,&fiber_H[0],9,1);
		  fiber_flg=0;
	}

//	upload_attr = pack_upload_data(BATTERY_AND_RSSI_VOLTAGE,(uint8_t *)&average_value[NTC_ADC1_CH4],2);
//	if(adc_protocol_upload != NULL)
//	{
//		adc_protocol_upload(upload_attr.pdata,upload_attr.len);
//	}
	// 字节序与老机器相反
	upload_attr = pack_upload_data(BATTERY_AND_RSSI_VOLTAGE,(uint8_t *)&g_ADC1AverageValue[Battery_Voltage_ADC1_CH10],4);
	if(adc_protocol_upload != NULL)
	{
		adc_protocol_upload(upload_attr.pdata,upload_attr.len);
	}
}

//static adc_conv_cplt_call_back adc1_conv_cplt_call_back_f = NULL;
static adc_awd_trigger_call_back right_motor_awd_trigger_call_back_f = NULL;
static adc_awd_trigger_call_back left_motor_awd_trigger_call_back_f = NULL;

void adc1_manage_init(void)
{
	//  HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)g_ADC1DMABuffer,ADC1_CHANNEL_NUM*ADC1_CHANNEL_SAMPLES);
}

void adc2_manage_init(void)
{
  HAL_ADC_Start(&hadc2);

}

void adc3_manage_init(void)
{
	HAL_ADC_Start(&hadc3);
}


//void adc1_conv_cplt_callback_register(adc_conv_cplt_call_back fun)
//{
//	adc1_conv_cplt_call_back_f = fun;
//}

void right_motor_awd_trigger_callback_register(adc_awd_trigger_call_back fun)
{
	right_motor_awd_trigger_call_back_f = fun;
}

void left_motor_awd_trigger_callback_register(adc_awd_trigger_call_back fun)
{
	left_motor_awd_trigger_call_back_f = fun;
}

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//	if(hadc->Instance == ADC1)
//	{
//		if(adc1_conv_cplt_call_back_f != NULL)
//		{
//			adc1_conv_cplt_call_back_f((uint8_t *)adc1_conv_buff,SAMPLE_PER_CHANNEL*SAMPLE_CHANNEL_NUM*2);
//		}
//	}
//}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{

	if(hadc->Instance == ADC2)
	{
		if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_AWD) != RESET)
		{
			if(right_motor_awd_trigger_call_back_f != NULL)
			{
				right_motor_awd_trigger_call_back_f();
			}

		}
	}
	else if(hadc->Instance == ADC3)
	{
		if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_AWD) != RESET)
		{
			if(left_motor_awd_trigger_call_back_f != NULL)
			{
				left_motor_awd_trigger_call_back_f();
			}
		}
	}
}


