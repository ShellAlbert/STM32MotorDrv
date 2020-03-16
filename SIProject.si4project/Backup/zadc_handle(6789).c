/*
 * filename:zadc_handle.c
 * versin:0.1.0
 * author:1811543668@qq.com
 * date: 2020/3/16
 */

#include <zadc_handle.h>
#include "drv_adc.h"
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
			sum += adc1_conv_buff[i][j];
		}
		average_value[j] = sum/ADC1_CHANNEL_SAMPLES;
		sum = 0;
	}
	if((average_value[0]>3000)&&(fiber_flg==0))
	{
		  HAL_UART_Transmit(&huart4,&fiber_L[0],9,1);
		  fiber_flg=1;
	}
	else if((average_value[0]<3000)&&(fiber_flg==1))
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
	upload_attr = pack_upload_data(BATTERY_AND_RSSI_VOLTAGE,(uint8_t *)&average_value[Battery_Voltage_ADC1_CH10],4);
	if(adc_protocol_upload != NULL)
	{
		adc_protocol_upload(upload_attr.pdata,upload_attr.len);
	}
}


