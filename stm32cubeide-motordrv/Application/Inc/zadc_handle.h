/*
 * filename:zadc_handle.h
 * versin:0.1.0
 * author:1811543668@qq.com
 * date: 2020/3/16
 */


#ifndef INC_ZADC_HANDLE_H_
#define INC_ZADC_HANDLE_H_
#include "cmsis_os.h"

extern osThreadId CommunicateTaskHandle;

#define BATTERY_AND_RSSI_VOLTAGE  0x0F

typedef void (*adc_upload_handler_t)(uint8_t *buf, uint8_t len);

typedef enum
{
	NTC_ADC1_CH4 = 0,
	Battery_Voltage_ADC1_CH10 = 1,
	RSSI1_ADC1_CH12 = 2,
	RSSI2_ADC1_CH13 = 3,
}voltage_e;

void zsy_ADCInit(void);
void adc_upload_data_register(adc_upload_handler_t upload_t);
void adh_prase_adc_dam_data(void);
void adh_adc_dma_data_handle(uint8_t *buf, uint32_t len);

#include "adc.h"

typedef void (*adc_conv_cplt_call_back)(uint8_t *buf, uint32_t len);
typedef void (*adc_awd_trigger_call_back)(void);


void adc1_conv_cplt_callback_register(adc_conv_cplt_call_back fun);
void right_motor_awd_trigger_callback_register(adc_awd_trigger_call_back fun);
void left_motor_awd_trigger_callback_register(adc_awd_trigger_call_back fun);
void get_adc_value(void);


#endif /* INC_ZADC_HANDLE_H_ */
