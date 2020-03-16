

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


//the distance value measured by LaserDistanceMeasure module.
extern uint32_t g_LaserDistance;

//crc32 check.
extern uint32_t g_CRC32Table[];
extern uint32_t g_CRC32Calculate(uint8_t* s, int len);

#endif

