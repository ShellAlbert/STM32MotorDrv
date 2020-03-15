/*
 * data_handle.c
 *
 *  Created on: Jul 25, 2019
 *      Author: wt
 */
//PDH:全称是protocol data handle,表示协议数据处理器

#include "protocal_data_handle.h"
#include "stdio.h"
#include "drv_uart.h"
#include "fifo.h"
#include "cmd.h"
#include "cmsis_os.h"
#include "communicate.h"



static unpack_data_t dh_unpack_obj;
static fifo_s_t  dh_rxdata_fifo;

//定义一个500字节的全局的静态数据用于当作接收FIFO.
#define DH_FIFO_BUFLEN 500
static uint8_t   dh_rxdata_buf[DH_FIFO_BUFLEN];

void pdh_param_init(void)
{
  fifo_s_init(&dh_rxdata_fifo, dh_rxdata_buf, DH_FIFO_BUFLEN);

  /* initial judge data unpack object */
  dh_unpack_obj.data_fifo = &dh_rxdata_fifo;
  dh_unpack_obj.index = 0;
  dh_unpack_obj.frame_len = 0;
  dh_unpack_obj.unpack_step = STEP_HEADER_5A;
}

uint32_t pdh_uart_rx_data_handle(uint8_t *data, uint32_t len)
{
	//将数据复制到Rx FIFO中
	fifo_s_puts(&dh_rxdata_fifo, (char *)data, len);
	//向通信任务CommunicateTask发送通知，并携带通知值
	osSignalSet(CommunicateTaskHandle, RECV_PROTOCOL_SIGNAL);
	return 0;
}

void pdh_data_handler(uint8_t *p_frame, uint8_t len)
{
	static attr_pack_t pdata;
	uint8_t data_len = p_frame[frame_length]-PROTOCOL_FIXED_LENGTH;
	if(data_len>0)
	{
		memcpy(pdata.pdata,&p_frame[attribute_id],data_len);
		pdata.len = data_len;
		protocol_rcv_pack_handle((uint8_t*)&pdata,p_frame[attribute_id]);
	}
}

void pdh_unpack_fifo_data(void)
{
  uint8_t byte = 0;
  unpack_data_t *p_obj = &dh_unpack_obj;

  while ( fifo_s_used(p_obj->data_fifo) )
  {
    byte = fifo_s_get(p_obj->data_fifo);
    switch(p_obj->unpack_step)
    {
      case STEP_HEADER_5A:
      {
        if(byte == DH_PROTOCOL_HEADER_5A)
        {
          p_obj->unpack_step = STEP_HEADER_A5;
          p_obj->protocol_packet[p_obj->index++] = byte;
        }
        else
        {
          p_obj->index = 0;
        }
      }break;

      case STEP_HEADER_A5:
	  {
	    if(byte == DH_PROTOCOL_HEADER_A5)
	    {
		  p_obj->unpack_step = STEP_LENGTH;
		  p_obj->protocol_packet[p_obj->index++] = byte;
	    }
	    else
	    {
		  p_obj->index = 0;
	    }
	  }break;

      case STEP_LENGTH:
      {
        p_obj->frame_len = byte;
        p_obj->protocol_packet[p_obj->index++] = byte;

        if( (p_obj->frame_len > (PROTOCOL_FIXED_LENGTH)) && (p_obj->frame_len < (DH_PROTOCOL_FRAME_MAX_SIZE)) )
        {
          p_obj->unpack_step = STEP_FRAME_SUM;
        }
        else
        {
          p_obj->unpack_step = STEP_HEADER_5A;
          p_obj->index = 0;
        }
      }break;

      case STEP_FRAME_SUM:
      {
        if (p_obj->index < (p_obj->frame_len))
        {
           p_obj->protocol_packet[p_obj->index++] = byte;
        }
        if (p_obj->index >= (p_obj->frame_len))
        {
          p_obj->unpack_step = STEP_HEADER_5A;
          p_obj->index = 0;

          if (pdh_verify_sum(p_obj->protocol_packet, p_obj->frame_len))
          {
        	  pdh_data_handler(p_obj->protocol_packet, p_obj->frame_len);
          }
        }
      }break;

      default:
      {
        p_obj->unpack_step = STEP_HEADER_5A;
        p_obj->index = 0;
      }break;
    }
  }
}

void pdh_data_upload(uint8_t *p_frame, uint8_t len)
{
	osMutexWait(uploadMutexHandle,osWaitForever);
	
	//PROTOCOL_FIXED_LENGTH，帧的最小长度为6个字节
	//包括:
	//0x5A,0xA5：2个字节的同步头
	//Length:1个字节的帧长度
	//cmd:1个字节表示upload或download，表示方向
	//endpoint:1个字节表示端点，0x00:表示APP,0x01:表示STM32
	//data:帧数据，这个是不定长的
	//checksum:1个字节的校验和
	
	uint8_t i=0,j=0;
	uint8_t buff[256];
	buff[i++]=0x5A;
	buff[i++]=0xA5;
	buff[i++]=PROTOCOL_FIXED_LENGTH+len;
	buff[i++]=0x05; // upload
	buff[i++]=0x01; // endpoint
	for(j=0;j<len;j++)
	{
		buff[i++]=p_frame[j];
	}
	buff[i]=pdh_get_sum(buff,i);
	usart1_transmit(buff, i+1);
	osMutexRelease(uploadMutexHandle);
}

uint8_t pdh_verify_sum(uint8_t *p_msg, uint8_t len)
{
	uint16_t w_expected = 0;

	if ((p_msg == NULL) || (len <= PROTOCOL_FIXED_LENGTH))
	{
		return 1;
	}
	w_expected = pdh_get_sum(p_msg, len-1);

	return w_expected == p_msg[len-1];

}

uint8_t pdh_get_sum(uint8_t *p_msg, uint8_t len)
{
  uint8_t i;
  uint8_t sum = 0;

  for(i = 0; i < len; i ++)
  {
    sum += *(p_msg++);
  }

  return sum;
}


