

/*
 * data_handle.h
 *
 *	Created on: Jul 25, 2019
 *		Author: wt
 */
#ifndef INC_PROTOCAL_DATA_HANDLE_H_
#define INC_PROTOCAL_DATA_HANDLE_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "fifo.h"

extern osThreadId CommunicateTaskHandle;
extern osMutexId uploadMutexHandle;

#define DH_PROTOCOL_HEADER_5A	(0x5A)
#define DH_PROTOCOL_HEADER_A5	(0xA5)
#define PROTOCOL_FIXED_LENGTH	(6)

#define DH_PROTOCOL_FRAME_MAX_SIZE (250)

// attribute id
#define SIGNAL_STRENGTH 		0x01
#define AUTO_FOCUS				0x02
#define MEASURE_DISTANCE		0x03
#define LEFT_MOTOR_MANUAL_FOCUS_PLUS 0x04
#define LEFT_MOTOR_MANUAL_FOCUS_MINUS 0x05
#define SYSTEM_RESET			0x06
#define BIG_VIEW				0x07
#define SMALL_VIEW				0x08
#define NOISE_ELIMINATING_LEVEL_PLUS 0x09
#define NOISE_ELIMINATING_LEVEL_MINUS 0x0A
#define VIEW					0x0B
#define NOISE_ELIMINATING_LEVEL 0x0C
#define LEFT_MOTOR_MANUAL_FOCUS_PLUS_LONG_CLICK 0x0D
#define LEFT_MOTOR_MANUAL_FOCUS_MINUS_LONG_CLICK 0x0E
#define BATTERY_AND_RSSI_VOLTAGE 0x0F
#define READ_DISTANCE_VALUE 	0x10
#define STOP_AUTO_FOCUS 		0x11
#define SET_JZ1 				0x12
#define SET_JZ2 				0x13
#define SET_SOUND_SOURCE_1		0x14
#define SET_SOUND_SOURCE_2		0x15
#define M3_FORWARD				0x16
#define M3_REVERSAL 			0x17
#define M3_STOP 				0x18
#define M4_FORWARD				0x19
#define M4_REVERSAL 			0x1A
#define M4_STOP 				0x1B
#define JZ1_LEVEL				0x1C
#define JZ2_LEVEL				0x1D
#define SET_MANUAL_FOCUS_RIGHT_MOTOR 0x1E
#define SET_MANUAL_FOCUS_LEFT_MOTOR 0x1F
#define CURRENT_MOTOR			0x20
#define CURRENT_JZ				0x21
#define SET_VOLUME				0x22
#define TWO_CH_AUTO_FOCUS		0x23
#define M3_FORWARD_LONG_CLICK	0x24
#define M3_REVERSAL_LONG_CLICK	0x25
#define M4_FORWARD_LONG_CLICK	0x26
#define M4_REVERSAL_LONG_CLICK	0x27
#define TWO_CH_AUTO_FOCUS_ENABLE_DISABLE 0x28
#define STOP_TWO_CH_AUTO_FOCUS	0x29
#define IMAGE_PROCESS_STATUS	0x2A
#define AMPLIFIER_CONTROL		0x2B
#define ADJUST_AUTO_FOCUS_START 0x2C
#define ADJUST_AUTO_FOCUS_STOP	0x2D
#define RIGHT_MOTOR_MANUAL_FOCUS_PLUS 0x2E
#define RIGHT_MOTOR_MANUAL_FOCUS_MINUS 0x2F
#define RIGHT_MOTOR_MANUAL_FOCUS_PLUS_LONG_CLICK 0x30
#define RIGHT_MOTOR_MANUAL_FOCUS_MINUS_LONG_CLICK 0x31
#define OPEN_PID				0x32
#define PROTECT_STEPPER_MOVE	0x33

//add by zhangshaoyan 2020/3/15 begin.
#define SYNC_HEADER_CLEAR		0x44454354 //clear frame.
#define SYNC_HEADER_ENCRYPT 	0x454E4354 //encrypt frame.


enum 
{
	nReg_Battery_R = 0x000001,						//read battery register.
	nReg_RSSI_R = 0x000002, 						//read RSSI register.
	nReg_Distance_R = 0x000003, 					//read Distance register.	
	nReg_LenMotorCtl_W = 0x800004,					//write LenMotorCtl register.
	nReg_2DBracketCtl_W = 0x800005, 				//write 2D bracket ctrl register.
	nReg_OutVolume_R = 0x000006,					//read output volume register.
	nReg_OutVolume_W = 0x800006,					//write output volume register.
	nReg_VideoCtl_R = 0x000007, 					//read videoCtl register.
	nReg_VideoCtl_W = 0x800007, 					//write videoCtl register.
	nReg_FPGA_R = 0x000008, 						//read FPGA register.
	nReg_FPGA_W = 0x800008, 						//write FPGA register.
	nReg_CrossXY_R = 0x000009,						//read CrossXY register.
	nReg_CrossXY_W = 0x800009,						//write CrossXY register.
	nReg_AutoFocus1_R = 0x3F0000,					//read AutoFocus1 register.
	nReg_AutoFocus1_W = 0xBF0000,					//write AutoFocus1 register.
};


//finite state machine FSM for UnPack.
typedef enum 
{
	UnPack_Field_Sync = 0, 
	UnPack_Field_Length, 
	UnPack_Field_Key, 
	UnPack_Field_RegAddr, 
	UnPack_Field_RegData, 
	UnPack_Field_CRC32, 
} FSM_UnPack;


//a container helper to hold one complete frame.
//include:SyncHeader,Length,Key,RegAddr,RegData,CRC32.
typedef struct 
{
	fifo_s_t *		fifo;
	FSM_UnPack		fsm;

	uint32_t		lenField;
	uint32_t		keyField;
	uint32_t		regAddrField;
	uint32_t		regDataField[32];
	uint32_t		crc32Field;

	//flag to indicate text was encrypted or not.
	//0:clear text,1:encrypted text.
	uint8_t 		encryptFlag;
} ModBus_UnPack_Helper;


//add by zhangshaoyan 2020/3/15 end.
// frame field
typedef enum 
{
	header_first = 0, 
	header_second = 1, 
	frame_length = 2, 
	cmd = 3, 
	endpoint = 4, 
	attribute_id = 5, 
	frame_data_length = 6, 
	frame_data_start = 7, 
} frame_data_e;


//协议解包状态机
//解包主要是4个状态，前面2个是0x5A,0xA5帧头
//然后是帧长度，最后是帧校验
typedef enum 
{
	STEP_HEADER_5A = 0, 
	STEP_HEADER_A5 = 1, 
	STEP_LENGTH = 2, 
	STEP_FRAME_SUM = 3, 
} unpack_step_e;


typedef struct 
{
	fifo_s_t *		data_fifo;
	uint8_t 		frame_len;
	uint8_t 		protocol_packet[DH_PROTOCOL_FRAME_MAX_SIZE];
	unpack_step_e	unpack_step;
	uint8_t 		index;
} unpack_data_t;


uint32_t pdh_uart_rx_data_handle(uint8_t * data, uint32_t len);
uint8_t pdh_verify_sum(uint8_t * p_msg, uint8_t len);
uint8_t pdh_get_sum(uint8_t * p_msg, uint8_t len);
void pdh_param_init(void);
void pdh_unpack_fifo_data(void);
void pdh_data_handler(uint8_t * p_frame, uint8_t len);
void pdh_data_upload(uint8_t * p_frame, uint8_t len);

#endif /* INC_PROTOCAL_DATA_HANDLE_H_ */

