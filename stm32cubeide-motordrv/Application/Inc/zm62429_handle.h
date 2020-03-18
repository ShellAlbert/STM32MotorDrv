/*
 * m62429.h
 *
 *  Created on: Aug 8, 2019
 *      Author: wt
 */

#ifndef DEVICES_M62429_H_
#define DEVICES_M62429_H_
#include "gpio.h"
#include "zgblpara.h"

#define CLK_L HAL_GPIO_WritePin(M62429_CLK_GPIO_Port,M62429_CLK_Pin,GPIO_PIN_RESET)
#define CLK_H HAL_GPIO_WritePin(M62429_CLK_GPIO_Port,M62429_CLK_Pin,GPIO_PIN_SET)
#define DAT_L HAL_GPIO_WritePin(M62429_D_GPIO_Port,M62429_D_Pin,GPIO_PIN_RESET)
#define DAT_H HAL_GPIO_WritePin(M62429_D_GPIO_Port,M62429_D_Pin,GPIO_PIN_SET)

extern uint8_t g_M62429Volume;
void zsy_M62429Init(void);
void zsy_M62429Ctrl(uint8_t volume);
#endif /* DEVICES_M62429_H_ */
