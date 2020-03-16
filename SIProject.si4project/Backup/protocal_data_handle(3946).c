

/*
 * data_handle.c
 *
 *	Created on: Jul 25, 2019
 *			Author: wt
 */
//PDH:全称是protocol data handle,表示协议数据处理器
#include "protocal_data_handle.h"
#include "stdio.h"
#include "drv_uart.h"
#include "fifo.h"
#include "cmd.h"
#include "cmsis_os.h"
#include "communicate.h"

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

//global Distance Measure Variable
//define in 
extern hold_distance_t hold_distance;
//CRC32 algorithm.
uint32_t		g_CRC32Table[] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L, 
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L, 
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL, 
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL, 
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L, 
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L, 
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L, 
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL, 
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L, 
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL, 
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L, 
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L, 
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L, 
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL, 
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L, 
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL, 
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL, 
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L, 
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L, 
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L, 
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L, 
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L, 
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 
	0x2d02ef8dL
};


uint32_t g_CRC32Calculate(const char * s, int len)
{
	int 			i;
	uint32_t		crc32val = 0;

	crc32val			^= 0xFFFFFFFF;

	for (i = 0; i < len; i++)
		{
		crc32val			= g_CRC32Table[(crc32val ^ s[i]) & 0xFF] ^ ((crc32val >> 8) & 0x00FFFFFF);
		}

	return labs(crc32val ^ 0xFFFFFFFF);
}


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

					if (uRxInt32 == g_CRC32Calculate(crc32_buffer, crc32_buffer_len))
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
					{
					float			fDistance = atof(hold_distance.distance);
					uint32_t		uInteger = floor(fDistance);
					uint8_t 		uDecimal = floor((fDistance - uInteger) * 100);
					uint32_t		uDistanceRegData = (uInteger << 8 | uDecimal);

					zsy_ModBusTxOneRegister(nReg_Distance_R, uDistanceRegData);
					}
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
	buffer[5]			= g_CRC32Calculate((char *) buffer, sizeof(uint32_t) * 5);

	usart1_transmit((uint8_t *) buffer, sizeof(buffer));
	osMutexRelease(uploadMutexHandle);
	return;
}


