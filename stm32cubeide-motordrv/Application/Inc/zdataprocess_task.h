/*
 * filename:zdataprocess_thread.h
 * versin:0.1.0
 * author:1811543668@qq.com
 * date: 2020/3/16
 */


#ifndef INC_ZDATAPROCESS_TASK_H_
#define INC_ZDATAPROCESS_TASK_H_

//FreeRTOS notificate signas.
//#define RECV_PROTOCOL_SIGNAL      ( 1 << 0 )
//#define RECV_CEJU_SIGNAL          ( 1 << 1 )
//#define RECV_ADC_DMA_SIGNAL       ( 1 << 2 )

#define OS_SIGNAL_MODBUS_RTU	(1<<0)
#define OS_SIGNAL_DISTANCE		(1<<1)
#define OS_SIGNAL_ADC1			(1<<2)			
extern void zsy_DataProcessTaskLoop(void const * argument);
#endif /* INC_ZDATAPROCESS_TASK_H_ */
