

/*
 * filename:zdistance_handle.c
 * versin:0.1.0
 * author:1811543668@qq.com
 * date: 2020/3/16
 */
#include <stdlib.h>
#include <zdataprocess_task.h>
#include <zdistance_handle.h>
#include "cmsis_os.h"
#include "fifo.h"
#include "drv_uart.h"
#include "system_cmd.h"
#include "package.h"
#include "zgblpara.h"

#define DISTANCE_RX_BUF_SIZE	128
static uint8_t	g_DistanceRxBuffer[DISTANCE_RX_BUF_SIZE];
static fifo_s_t g_DistanceRxFIFO;

static unpack_ceju_data_t g_DisUnPackHelper;
static hold_distance_t hold_distance;

extern osThreadId DataProcessTaskHandle;


void zsy_DistanceInit(void)
{
	fifo_s_init(&g_DistanceRxFIFO, g_DistanceRxBuffer, DISTANCE_RX_BUF_SIZE);

	/* initial judge data unpack object */
	g_DisUnPackHelper.data_fifo = &g_DistanceRxFIFO;
	g_DisUnPackHelper.index = 0;
	g_DisUnPackHelper.frame_len = 0;
	g_DisUnPackHelper.unpack_step = STEP_CEJU_HRADER_FIRST;

	//initial USART3.
	MX_USART3_UART_Init();
	usart3_manage_init();

	//setup USART3 Rx CallBack.
	usart_rx_callback_register(&usart3_manage_obj, zsy_DistanceRxCallBack);
}


uint32_t zsy_DistanceRxCallBack(uint8_t * data, uint32_t len)
{
	//copy data to DistanceRxFIFO.
	fifo_s_puts(&g_DistanceRxFIFO, (char *) data, len);

	//send signal to DataProcessTask with NotificateValue.
	osSignalSet(DataProcessTaskHandle, OS_SIGNAL_DISTANCE);
	return 0;
}


void zsy_DistanceParseFrame(uint8_t * p_frame, uint8_t len)
{
	uint16_t		dis = 0;

	/* When distance measure fail: D=-----m , don't change the value of distance*/
	if ((p_frame[2] != '-') && (p_frame[6] != '-'))
		{
		dis 				= (uint16_t) (atof((char *) (&p_frame[2])) + 0.5);
		manifold_cmd.distance = dis;
		osSignalSet(CmdTaskHandle, MANIFOLD2_DISTANCE_SIGNAL);
		}

	//	else
	//	{
	//		zsy_DistanceMeasureStop();
	//	}

	// save the distance,len-4去除D=和\r\n
	memcpy(hold_distance.distance, &p_frame[2], len - 4);
	hold_distance.len	= len - 4;

	zsy_DistanceMeasureStop();

	//add by zhangshaoyan 2020/3/16 begin.
	//save distance value to global variable.
	float			fDistance = atof(hold_distance.distance);
	uint32_t		uInteger = floor(fDistance);
	uint8_t 		uDecimal = floor((fDistance - uInteger) * 100);

	g_LaserDistance 	= (uInteger << 8 | uDecimal);

	//upload distance to APP.
	zsy_ModBusTxOneRegister(nReg_Distance_R, g_LaserDistance);

	//add by zhangshaoyan 2020/3/16 end.
}


void cj_get_distance(void)
{
	upload_attr_t	upload_attr = pack_upload_data(MEASURE_DISTANCE, hold_distance.distance, hold_distance.len);

	pdh_data_upload(upload_attr.pdata, upload_attr.len);
}


uint8_t 		distence_flag = 0;
uint8_t 		write_step_flag = 0;
uint8_t 		distence_data_H = 0, distence_data_L = 0, point_flg = 0;
uint16_t		distence_turn = 0;


void zsy_DistanceParseFIFOData(void)
{
	uint8_t 		distence_bai = 0, distence_shi = 0, distence_ge = 0;
	uint8_t 		byte = 0;
	unpack_ceju_data_t * p_obj = &g_DisUnPackHelper;

	while (fifo_s_used(p_obj->data_fifo))
		{
		byte				= fifo_s_get(p_obj->data_fifo);

		switch (p_obj->unpack_step)
			{
			case STEP_CEJU_HRADER_FIRST:
					{
					if (byte == 0x10) // p_obj->index==0
						{
						p_obj->unpack_step	= STEP_CEJU_HEADER_SECOND;
						p_obj->protocol_packet[p_obj->index++] = CEJU_HRADER_FIRST_C;
						write_step_flag++;
						distence_flag		= 0;
						}
					else 
						{
						p_obj->index		= 0;
						write_step_flag 	= 0;
						distence_flag		= 0;
						}
					}
				break;

			case STEP_CEJU_HEADER_SECOND:
					{
					if (byte == 0x01) // p_obj->index==1
						{
						p_obj->unpack_step	= STEP_CEJU_END;
						p_obj->protocol_packet[p_obj->index++] = CEJU_HRADER_SECOND_C;
						write_step_flag++;
						distence_flag		= 0;
						}
					else 
						{
						p_obj->index		= 0;
						p_obj->unpack_step	= STEP_CEJU_HRADER_FIRST;
						write_step_flag 	= 0;
						distence_flag		= 0;
						}
					}
				break;

			case STEP_CEJU_END:
					{

					if (distence_flag == 0) // p_obj->index==2
						{
						if (byte == 0x00) // 数据描述符 0x00：有效数据
							{
							distence_flag		= 1;
							write_step_flag++;
							}
						else // 数据描述符 0x10：无效数据
							{
							p_obj->index		= 0;
							p_obj->unpack_step	= STEP_CEJU_HRADER_FIRST;
							write_step_flag 	= 0;
							distence_flag		= 0;
							}

						}
					else 
						{
						if (write_step_flag == 3)
							{
							distence_data_H 	= byte;
							write_step_flag++;
							}
						else if (write_step_flag == 4)
							{
							distence_data_L 	= byte;
							write_step_flag++;
							}
						else if (write_step_flag == 5)
							{
							if (byte == (0x100 - ((uint8_t) (distence_data_H + distence_data_L + 0x01)))) //验证校验和
								{
								distence_turn		= (distence_data_H << 8) +distence_data_L;
								point_flg			= distence_turn % 2;
								distence_turn		= distence_turn / 2;

								distence_bai		= (distence_turn / 100);

								if (distence_bai > 0)
									{
									distence_bai		+= 0x30;
									p_obj->protocol_packet[p_obj->index] = distence_bai;
									p_obj->index++;
									}

								distence_shi		= ((distence_turn % 100) / 10);

								if (distence_shi > 0)
									{
									distence_shi		+= 0x30;
									p_obj->protocol_packet[p_obj->index] = distence_shi;
									p_obj->index++;
									}
								else 
									{
									if (distence_bai > 0x30)
										{
										distence_shi		+= 0x30;
										p_obj->protocol_packet[p_obj->index] = distence_shi;
										p_obj->index++;
										}
									}

								distence_ge 		= (distence_turn % 10);

								if (distence_ge > 0)
									{
									distence_ge 		+= 0x30;
									p_obj->protocol_packet[p_obj->index] = distence_ge;
									p_obj->index++;
									}
								else 
									{
									if ((distence_shi > 0x30) || (distence_bai > 0x30))
										{
										distence_ge 		+= 0x30;
										p_obj->protocol_packet[p_obj->index] = distence_ge;
										p_obj->index++;
										}
									}

								p_obj->protocol_packet[p_obj->index] = 0x2E; //.
								p_obj->index++;

								if (point_flg)
									{
									p_obj->protocol_packet[p_obj->index] = 0x35; //5
									p_obj->index++;
									}
								else 
									{
									p_obj->protocol_packet[p_obj->index] = 0x30; //0
									p_obj->index++;
									}

								p_obj->protocol_packet[p_obj->index] = 'm'; //
								p_obj->index++;
								p_obj->protocol_packet[p_obj->index] = '\r'; //
								p_obj->index++;
								p_obj->protocol_packet[p_obj->index] = '\n'; //.
								p_obj->index++;

								p_obj->frame_len	= p_obj->index;
								p_obj->unpack_step	= STEP_CEJU_HRADER_FIRST;
								p_obj->index		= 0;
								distence_flag		= 0;
								write_step_flag 	= 0;
								zsy_DistanceParseFrame(p_obj->protocol_packet, p_obj->frame_len);
								}
							else 
								{
								p_obj->index		= 0;
								p_obj->unpack_step	= STEP_CEJU_HRADER_FIRST;
								distence_flag		= 0;
								write_step_flag 	= 0;
								}
							}
						else 
							{
							;
							}

						}

					}
				break;

			default:
					{
					p_obj->unpack_step	= STEP_CEJU_HRADER_FIRST;
					p_obj->index		= 0;
					distence_flag		= 0;
					write_step_flag 	= 0;
					}
				break;
			}
		}
}

void zsy_DistanceMeasureStart(void)
{
	//	uint8_t start[7] = {0x0D, 0x0A, 0x4F, 0x3D, 0x31, 0x0D, 0x0A};
	uint8_t start[4] ={0x10, 0x83, 0x00, 0x7D};

	//single distance measure.
	usart3_transmit(start, 4);
}

void zsy_DistanceMeasureStop(void)
{
	//	uint8_t stop[7] = {0x0D, 0x0A, 0x4F, 0x46, 0x46, 0x0D, 0x0A};
	uint8_t stop[3] ={0x10, 0x84, 0x7C};

	//stop distance measure.
	usart3_transmit(stop, 3);
}


