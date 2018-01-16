/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "stdio.h";
#include <string.h>
/* USER CODE BEGIN Includes */

char aTxBuffer[1024]; // maximum of 1024 chars out - no error checking
char aRxBuffer[10]; // maximum of 10 chars in - no error checking


/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
typedef enum {
	pin_set,
	pin_reset
} state;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void print_menu(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

	uint8_t spi_tx_data[2] = {0x00, 0x00}; //data,command = set to NOP command initially
	uint8_t spi_data=0;
	uint8_t spi_command=0;
	int8_t a=1;
	state npress = pin_reset;
	int16_t time=400;
	//char text[]="i am fast slow fun and \r\n";
	int f;

	// buffer to receive commands - no error checking
	uint8_t command_buffer[1024]; // set up command buffer & write pointer to it
	uint8_t command_buffer_rx=0;
	uint8_t command_received = 0; // set to 1 when we get a CR to indicate a command
	uint8_t value;

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  spi_command=0;
  spi_data=0;
  spi_tx_data[1]=spi_command;
  spi_tx_data[0]=spi_data;

  /* print the menu */

  print_menu();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */
	  /* the line below is polling for the uart */
	  if (HAL_UART_Receive(&huart2,&aRxBuffer, 1, 10) == HAL_OK) // wait 10ms for a character
	  {
		  for (f=0;f<strlen(aRxBuffer);f++)
		  {
			  command_buffer[command_buffer_rx++] = aRxBuffer[f];
			  HAL_UART_Transmit(&huart2, &command_buffer[command_buffer_rx], 1, 100); //echo type

			  if (aRxBuffer[f] == 13) //CR
			  {
				  command_buffer[command_buffer_rx-1] = 0; // terminate string and remoce CR
				  command_received = 1; // indicate command ready to be processed
			  }
		  }

	  // if we have received a complete command ( terminated by CR ), echo it out
	 if (command_received != 0)
		  {
		 	// echo out the command
			sprintf(aTxBuffer, "Received: %s \n\r",command_buffer);
			HAL_UART_Transmit(&huart2, &aTxBuffer, strlen(aTxBuffer)+3, 5000);

			// no checking for command being formatted correctly
			// commands are:
			// pxx where xx = 00 to 99 - sets pwm width

			switch (command_buffer[0])
			{
			case 't':
				sprintf(aTxBuffer, "time is %d \n\r",time);
				HAL_UART_Transmit(&huart2,&aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			case 'v':
				value = ((command_buffer[1]-'0') * 10) + (command_buffer[2]-'0');
				sprintf(aTxBuffer, "Averaging set to: %d\n\r",value);
				HAL_UART_Transmit(&huart2,&aTxBuffer, strlen(aTxBuffer), 5000);
				break;
			case '?':
				print_menu();
				break;

			default: // not recognised
				break;
			}
			// reset command buffer ready for the next command
			command_received = 0;
			command_buffer_rx = 0;
		  }
}
  /* USER CODE BEGIN 3 */
	  if(( HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin)==GPIO_PIN_SET) && (npress==pin_reset) && (a<=3)){
	 		npress=pin_set;
	 			a++;
	 			sprintf(aTxBuffer, "time %d\n\r",time>>a);
	 			HAL_UART_Transmit(&huart2,&aTxBuffer,strlen(aTxBuffer)+2,0xFFF);
	 		 } else if( ( HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin)==GPIO_PIN_SET) && (npress==pin_set)&& (a<=3)) {
	 			 npress=pin_set;
	 		 } else if ( ( HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin)==GPIO_PIN_SET) && (npress==pin_set)&& (a>3)){
	 			 npress=pin_set;
	 			 a=1;
	 		 }

	 		 else {
	 			 npress =pin_reset;
	 		 }
	 		  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin); // Toggle the state of LED2
	 		  HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_9);
	 		  HAL_Delay(time>>a); //delay 100ms

  /* USER CODE END 3 */

  }
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */

void print_menu (void){
	sprintf(aTxBuffer, "pxx where xx is a number from 0 to 99. It changes the time \n\r");
	HAL_UART_Transmit(&huart2,&aTxBuffer, strlen(aTxBuffer)+3, 5000); //include character array
	sprintf(aTxBuffer, "print this menu \n\r");
	HAL_UART_Transmit(&huart2,&aTxBuffer, strlen(aTxBuffer)+3, 5000); //include character array
}

/* Format the transmit buffer */

int format_transmmit_buffer (char * tx_buffer, char * message, char * data_string){
}

void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
