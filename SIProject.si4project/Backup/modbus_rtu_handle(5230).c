

/*
 * data_handle.c
 *
 *	Created on: Jul 25, 2019
 *			Author: wt
 */
//PDH:全称是protocol data handle,表示协议数据处理器
#include <modbus_rtu_handle.h>
#include "stdio.h"
#include "drv_uart.h"
#include "fifo.h"
#include "cmd.h"
#include "cmsis_os.h"
#include "communicate.h"
#include "zgblpara.h"
#if 1
static unpack_data_t dh_unpack_obj;
static fifo_s_t dh_rxdata_fifo;

//定义一个500字节的全局的静态数据用于当作接收FIFO.
#define DH_FIFO_BUFLEN			500
static uint8_t	dh_rxdata_buf[DH_FIFO_BUFLEN];

#endif

//add by zhangshaoyan 2020/3/15 begin.
#define MODBUS_RX_BUF_SIZE		128
static uint8_t	g_ModBusRxBuffer[MODBUS_RX_BUF_SIZE];
static fifo_s_t g_ModBusRxFIFO;

//an container object to help unpack plain data.
static ModBus_UnPack_Helper g_UnPackHelper;

//add by zhangshaoyan 2020/3/15 end.
void pdh_param_init(void)
{
	//create a fifo for modbus rx.
	fifo_s_init(&g_ModBusRxFIFO, g_ModBusRxBuffer, MODBUS_RX_BUF_SIZE);
	g_UnPackHelper.fifo = &g_ModBusRxFIFO;
	g_UnPackHelper.fsm	= UnPack_Field_Sync;
	g_UnPackHelper.encryptFlag = 0;
}


uint32_t pdh_uart_rx_data_handle(uint8_t * data, uint32_t len)
{
	//copy data to ModBusRxFIFO.
	fifo_s_puts(&g_ModBusRxFIFO, (char *) data, len);

	//send signal to CommunicateTask with NotificateValue.
	osSignalSet(CommunicateTaskHandle, RECV_PROTOCOL_SIGNAL);
	return 0;
}


void pdh_data_handler(uint8_t * p_frame, uint8_t len)
{
	static attr_pack_t pdata;
	uint8_t 		data_len = p_frame[frame_length] -PROTOCOL_FIXED_LENGTH;

	if (data_len > 0)
		{
		memcpy(pdata.pdata, &p_frame[attribute_id], data_len);
		pdata.len			= data_len;
		protocol_rcv_pack_handle((uint8_t *) &pdata, p_frame[attribute_id]);
		}
}


//try to read one complete frame from ModBusRxFIFO byte by byte.
void pdh_unpack_fifo_data(void)
{
	uint8_t 		byte = 0;
	unpack_data_t * p_obj = &dh_unpack_obj;



	//add by zhangshaoyan 2020/3/15 begin.
	uint32_t		crc32_buffer[32];
	uint32_t		crc32_buffer_len = 0;
	uint32_t		i, j;

	while (fifo_s_used(g_UnPackHelper.fifo) >= sizeof(int))
		{
		uint32_t		uRxInt32;
		uint8_t *		p	= (uint8_t *) &uRxInt32;

		//fetch 1 int (4 bytes) from fifo.
		p[0]				= fifo_s_get(g_UnPackHelper.fifo);
		p[1]				= fifo_s_get(g_UnPackHelper.fifo);
		p[2]				= fifo_s_get(g_UnPackHelper.fifo);
		p[3]				= fifo_s_get(g_UnPackHelper.fifo);

		switch (g_UnPackHelper.fsm)
			{
			case UnPack_Field_Sync:
					{
					//check 4 bytes sync header.
					if (SYNC_HEADER_CLEAR == uRxInt32)
						{
						g_UnPackHelper.encryptFlag = 0;
						g_UnPackHelper.fsm	= UnPack_Field_Length;
						memcpy((void *) (crc32_buffer + crc32_buffer_len), (void *) &uRxInt32, sizeof(uRxInt32));
						crc32_buffer_len	= sizeof(uRxInt32);
						}
					else if (SYNC_HEADER_ENCRYPT == uRxInt32)
						{
						g_UnPackHelper.encryptFlag = 1;
						g_UnPackHelper.fsm	= UnPack_Field_Length;
						memcpy((void *) (crc32_buffer + crc32_buffer_len), (void *) &uRxInt32, sizeof(uRxInt32));
						crc32_buffer_len	= sizeof(uRxInt32);
						}
					else 
						{
						//sync header is error,bypass it.
						//continue to fetch next 4 bytes and check it.
						}
					}
				break;

			case UnPack_Field_Length:
					{
					//sync header(4),length(4),key(4),regAddr(4),regData(4),crc32(4)
					//so the minimum length of a frame is 24 bytes.
					if (uRxInt32 < 24)
						{
						//if length field is error,reset FSM.
						g_UnPackHelper.fsm	= UnPack_Field_Sync;
						}
					else 
						{
						//frameLen=key(4)+regAddr(4)+regData(N)+crc32(4)
						//so,regData(N)=frameLen-12
						g_UnPackHelper.lenField = uRxInt32;
						g_UnPackHelper.regDataFieldLen = (uRxInt32 - 12);

						//to help move regData more than 1 bytes.
						i					= g_UnPackHelper.regDataFieldLen;
						j					= 0;
						g_UnPackHelper.fsm	= UnPack_Field_Key;

						memcpy((void *) (crc32_buffer + crc32_buffer_len), (void *) &uRxInt32, sizeof(uRxInt32));
						crc32_buffer_len	+= sizeof(uRxInt32);
						}
					}
				break;

			case UnPack_Field_Key:
					{
					g_UnPackHelper.keyField = uRxInt32;
					g_UnPackHelper.fsm	= UnPack_Field_RegAddr;
					memcpy((void *) (crc32_buffer + crc32_buffer_len), (void *) &uRxInt32, sizeof(uRxInt32));
					crc32_buffer_len	+= sizeof(uRxInt32);
					}
				break;

			case UnPack_Field_RegAddr:
					{
					g_UnPackHelper.regAddrField = uRxInt32;
					g_UnPackHelper.fsm	= UnPack_Field_RegData;
					memcpy((void *) (crc32_buffer + crc32_buffer_len), (void *) &uRxInt32, sizeof(uRxInt32));
					crc32_buffer_len	+= sizeof(uRxInt32);
					}
				break;

			case UnPack_Field_RegData:
					{
					if (i-- > 0)
						{
						g_UnPackHelper.regDataField[j++] = uRxInt32;
						memcpy((void *) (crc32_buffer + crc32_buffer_len), (void *) &uRxInt32, sizeof(uRxInt32));
						crc32_buffer_len	+= sizeof(uRxInt32);
						}
					else 
						{
						g_UnPackHelper.fsm	= UnPack_Field_CRC32;
						}
					}
				break;

			case UnPack_Field_CRC32:
					{
					g_UnPackHelper.crc32Field = uRxInt32;
					g_UnPackHelper.fsm	= UnPack_Field_Sync;
					crc32_buffer_len	= 0;

					if (uRxInt32 == g_CRC32Calculate((uint8_t*)crc32_buffer, crc32_buffer_len))
						{
						zsy_ParseModBusFrame(&g_UnPackHelper);
						}
					else 
						{
						//crc32 check error,do something to indicate error occures.
						}
					}
				break;

			default:
					{
					g_UnPackHelper.fsm	= UnPack_Field_Sync;
					}
				break;
			}
		}

	//add by zhangshaoyan 2020/3/15 end.
	while (fifo_s_used(p_obj->data_fifo))
		{
		byte				= fifo_s_get(p_obj->data_fifo);

		switch (p_obj->unpack_step)
			{
			case STEP_HEADER_5A:
					{
					if (byte == DH_PROTOCOL_HEADER_5A)
						{
						p_obj->unpack_step	= STEP_HEADER_A5;
						p_obj->protocol_packet[p_obj->index++] = byte;
						}
					else 
						{
						p_obj->index		= 0;
						}
					}
				break;

			case STEP_HEADER_A5:
					{
					if (byte == DH_PROTOCOL_HEADER_A5)
						{
						p_obj->unpack_step	= STEP_LENGTH;
						p_obj->protocol_packet[p_obj->index++] = byte;
						}
					else 
						{
						p_obj->index		= 0;
						}
					}
				break;

			case STEP_LENGTH:
					{
					p_obj->frame_len	= byte;
					p_obj->protocol_packet[p_obj->index++] = byte;

					if ((p_obj->frame_len > (PROTOCOL_FIXED_LENGTH)) &&
						 (p_obj->frame_len < (DH_PROTOCOL_FRAME_MAX_SIZE)))
						{
						p_obj->unpack_step	= STEP_FRAME_SUM;
						}
					else 
						{
						p_obj->unpack_step	= STEP_HEADER_5A;
						p_obj->index		= 0;
						}
					}
				break;

			case STEP_FRAME_SUM:
					{
					if (p_obj->index < (p_obj->frame_len))
						{
						p_obj->protocol_packet[p_obj->index++] = byte;
						}

					if (p_obj->index >= (p_obj->frame_len))
						{
						p_obj->unpack_step	= STEP_HEADER_5A;
						p_obj->index		= 0;

						if (pdh_verify_sum(p_obj->protocol_packet, p_obj->frame_len))
							{
							pdh_data_handler(p_obj->protocol_packet, p_obj->frame_len);
							}
						}
					}
				break;

			default:
					{
					p_obj->unpack_step	= STEP_HEADER_5A;
					p_obj->index		= 0;
					}
				break;
			}
		}
}


void pdh_data_upload(uint8_t * p_frame, uint8_t len)
{
#if 1
	osMutexWait(uploadMutexHandle, osWaitForever);

	//PROTOCOL_FIXED_LENGTH，帧的最小长度为6个字节
	//包括:
	//0x5A,0xA5：2个字节的同步头
	//Length:1个字节的帧长度
	//cmd:1个字节表示upload或download，表示方向
	//endpoint:1个字节表示端点，0x00:表示APP,0x01:表示STM32
	//data:帧数据，这个是不定长的
	//checksum:1个字节的校验和
	uint8_t 		i	= 0, j = 0;
	uint8_t 		buff[256];

	buff[i++]			= 0x5A;
	buff[i++]			= 0xA5;
	buff[i++]			= PROTOCOL_FIXED_LENGTH + len;
	buff[i++]			= 0x05; 					// upload
	buff[i++]			= 0x01; 					// endpoint

	for (j = 0; j < len; j++)
		{
		buff[i++]			= p_frame[j];
		}

	buff[i] 			= pdh_get_sum(buff, i);
	usart1_transmit(buff, i + 1);
	osMutexRelease(uploadMutexHandle);
#endif
}


uint8_t pdh_verify_sum(uint8_t * p_msg, uint8_t len)
{
	uint16_t		w_expected = 0;

	if ((p_msg == NULL) || (len <= PROTOCOL_FIXED_LENGTH))
		{
		return 1;
		}

	w_expected			= pdh_get_sum(p_msg, len - 1);

	return w_expected == p_msg[len - 1];

}


uint8_t pdh_get_sum(uint8_t * p_msg, uint8_t len)
{
	uint8_t 		i;
	uint8_t 		sum = 0;

	for (i = 0; i < len; i++)
		{
		sum 				+= * (p_msg++);
		}

	return sum;
}


void zsy_ParseModBusFrame(ModBus_UnPack_Helper * frm)
{
	if (frm->encryptFlag)
		{
		//decrypt first.
		}
	else 
		{
		//clear text,parse directly.
		switch (frm->regAddrField)
			{
			case nReg_Battery_R:
				zsy_ModBusTxOneRegister(nReg_Battery_R, 0);
				break;

			case nReg_RSSI_R:
				zsy_ModBusTxOneRegister(nReg_RSSI_R, 0);
				break;

			case nReg_Distance_R:
					zsy_ModBusTxOneRegister(nReg_Distance_R, g_LaserDistance);
				break;

			case nReg_LenMotorCtl_W:
				break;

			case nReg_2DBracketCtl_W:
				break;

			case nReg_OutVolume_R:
				zsy_ModBusTxOneRegister(nReg_OutVolume_R, 0);
				break;

			case nReg_OutVolume_W:
				break;

			case nReg_VideoCtl_R:
				zsy_ModBusTxOneRegister(nReg_VideoCtl_R, 0);
				break;

			case nReg_VideoCtl_W:
				break;

			case nReg_FPGA_R:
				zsy_ModBusTxOneRegister(nReg_FPGA_R, 0);
				break;

			case nReg_FPGA_W:
				break;

			case nReg_CrossXY_R:
				zsy_ModBusTxOneRegister(nReg_CrossXY_R, 0);
				break;

			case nReg_CrossXY_W:
				break;

			case nReg_AutoFocus1_R:
				zsy_ModBusTxOneRegister(nReg_AutoFocus1_R, 0);
				break;

			case nReg_AutoFocus1_W:
				break;

			default:
				break;
			}
		}
}


void zsy_ModBusTxOneRegister(uint32_t regAddr, uint32_t regData)
{
	uint32_t		buffer[6];

	//there are more than one Task will call this function to Tx ModBus data,
	//so here we use Mutex to lock.
	osMutexWait(uploadMutexHandle, osWaitForever);

	buffer[0]			= SYNC_HEADER_CLEAR;

	//frameLen=key(4)+regAddr(4)+regData(4)+Padding(0)+CRC32(4)=16.
	buffer[1]			= 16;						//length.
	buffer[2]			= 0;						//key.
	buffer[3]			= regAddr | MODBUS_ADDR_APP; //regaddr,tx to APP.
	buffer[4]			= regData;					//regData.
	buffer[5]			= g_CRC32Calculate((uint8_t*)buffer, sizeof(uint32_t) * 5);

	usart1_transmit((uint8_t *) buffer, sizeof(buffer));
	osMutexRelease(uploadMutexHandle);
	return;
}


