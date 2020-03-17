

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file			: main.c
  * @brief			: Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  * 							www.st.com/SLA0044
  *
  ******************************************************************************
  *最初意法半导体针对STM32的SoC资源配置使用的软件是STM32CubeMX，只能生成代码不能编译，
  *所以生成的代码中好多函数带有前缀MX_
  *后来STM32CubeMX升级后，更变为STM32CubeIDE，基于Eclipse C/C++的解决方案,
  *STM32CubeIDE集成了STM32CubeMX用于配置代码生成，同时集成GNU arm-none-eabi-gcc交叉编译工具链�
	�
  *是一个完整的IDE。
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <zadc_handle.h>
#include <zmodbus_rtu_handle.h>
#include <zdistance_handle.h>
#include <zlens_task.h>
#include "zgblpara.h"
#include "cmsis_os.h"

#include "lens_task.h"
#include "drv_uart.h"
#include "2d_bracket.h"
#include "protect_structure.h"
#include "m62429.h"

#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "2d_bracket.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint8_t	fiber_H[9];
extern uint8_t	fiber_L[9];

/* USER CODE END PTD */
struct lens lensObj;
struct bracket bracketObj;
struct protect_structure protectObj;

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief	The application entry point.
  * @retval int
  */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	taskENTER_CRITICAL();

	/* USER CODE END SysInit */

	//GPIO initial.
	MX_GPIO_Init();

	//DMA1-Channel1:ADC1.
	//DMA1-Channel2:USART3_TX.
	//DMA1-Channel3:USART3_RX.
	//DMA1-Channel4:USART1_TX.
	//DMA1-Channel5:USART1_RX.
	MX_DMA_Init();

	//TIM2-CH1:Stepper Motor drive pluse for Left/Right direction move.
	MX_TIM2_Init();
	//TIM8-CH2:Stepper Motor drive pluse for Up/Down direction move.
	MX_TIM8_Init();

	//TIM4:used to help GeneralTimer without RCR(Repeat Counter Register) to stop PWM.
	MX_TIM4_Init();
	
	//communicate with Laser Module.
	MX_UART4_Init();
	HAL_UART_Transmit(&huart4, &fiber_H[0], 9, 1);

	//DMA is only move data,when finished,IRQ will happen.
	//ADC1(DMA Mode,IRQ Enabled)
	//ADC1-CH4:NTC,	ADC1-CH10:Battery Voltage,	ADC1-CH12:RSSI.
	//ADC2(IRQ Mode,IRQ Enabled)
	//ADC2-CH8:DC Motor1 Current(AnalogWatchDog).
	//ADC3(IRQ Mode,IRQ Enabled)
	//ADC3-CH3:DC Motor2 Current(AnalogWatchDog).

	//DMA1_Channel1_IRQHandler():HAL_DMA_IRQHandler(&hdma_adc1)
	//ADC1_2_IRQHandler():HAL_ADC_IRQHandler(&hadc1),HAL_ADC_IRQHandler(&hadc2)
	//ADC3_IRQHandler():HAL_ADC_IRQHandler(&hadc3)
	//zadc_handle.c:HAL_ADC_LevelOutOfWindowCallback()
	//ADC2,ADC3 used to check motor peak torque current.
	zsy_ADCInit();

	//ModBusRTU:USART1.
	//Tx&Rx with DMA(IRQ Enabled)
	//DMA1_Channel4_IRQHandler():HAL_DMA_IRQHandler(&hdma_usart1_tx)
	//DMA1_Channel5_IRQHandler():HAL_DMA_IRQHandler(&hdma_usart1_rx)
	zsy_ModBusInit();

	//LaserDistanceMeasure:USART3.
	//Tx&Rx with DMA(IRQ Enabled)
	//DMA1_Channel2_IRQHandler():HAL_DMA_IRQHandler(&hdma_usart3_tx)
	//DMA1_Channel3_IRQHandler():HAL_DMA_IRQHandler(&hdma_usart3_rx)
	zsy_DistanceInit();


	
	//Output Volume Ctrl.
	zsy_M62429Init();


	//lens_pid_register(&lensObj,"lens");
	//Left/Right DC Motor.
	zsy_LensInit();
	
	//bracket_register(&bracketObj,"bracket");
	//Left/Right & Up/Down Stepper Motor.
	zsy_Bracket2DInit();
	
	protect_structure_register(&protectObj,"protect_structure");

	/* USER CODE END 2 */

	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
		{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		}

	/* USER CODE END 3 */
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
		{
		0
		};
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
		{
		0
		};
	RCC_PeriphCLKInitTypeDef PeriphClkInit =
		{
		0
		};

	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		{
		Error_Handler();
		}

	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType =
		 RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
		{
		Error_Handler();
		}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
		Error_Handler();
		}
}


/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	update_stepper_positon(GPIO_Pin);
}


/* USER CODE END 4 */

/**
  * @brief	Period elapsed callback in non blocking mode
  * @note	This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param	htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim)
{
	/* USER CODE BEGIN Callback 0 */
	if (htim->Instance == TIM4)
		{
			zsy_Bracket2DStopLftRhtStepperMotor();
		}

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6)
		{
		HAL_IncTick();
		}

	/* USER CODE BEGIN Callback 1 */
	if (htim->Instance == TIM8)
		{
		zsy_Bracket2DStopUpDownStepperMotor();
		}

	/* USER CODE END Callback 1 */
}


/**
  * @brief	This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}


#ifdef USE_FULL_ASSERT

/**
  * @brief	Reports the name of the source file and the source line number
  * 		where the assert_param error has occurred.
  * @param	file: pointer to the source file name
  * @param	line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}


#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
