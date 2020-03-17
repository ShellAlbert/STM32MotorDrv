

/*
 * filename:zdataprocess_thread.c
 * versin:0.1.0
 * author:1811543668@qq.com
 * date: 2020/3/16
 */
#include <zadc_handle.h>
#include <zdataprocess_task.h>
#include <zdistance_handle.h>
#include <zmodbus_rtu_handle.h>
#include "cmsis_os.h"
#include "drv_uart.h"
#include "cmd.h"

void zsy_DataProcessTaskLoop(void const * argument)
{
	while (1)
		{
		osEvent 		event;

		event				= osSignalWait(OS_SIGNAL_MODBUS_RTU | OS_SIGNAL_DISTANCE | OS_SIGNAL_ADC1, osWaitForever);

		if (event.status == osEventSignal)
			{
			if (event.value.signals & OS_SIGNAL_MODBUS_RTU)
				{
				zsy_ModBusParseFIFOData();
				}

			if (event.value.signals & OS_SIGNAL_DISTANCE)
				{
				zsy_DistanceParseFIFOData();
				}

			if (event.value.signals & OS_SIGNAL_ADC1)
				{
				zsy_ParseADC1DMAData();
				}
			}
		}
}


