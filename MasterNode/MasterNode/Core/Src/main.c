/*
 *
 *
 *  Created on: Nov 14, 2020
 *      Author: AnılParlak & BurakAkgun
 */

#include "main.h"
#include "wifi_module.h"
#include "timer4.h"
#include "timer3.h"
#include "timer2.h"

void console(void); // Declaration for console function

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);

/* You can enter the information of the desired WiFi Address to be registered  */
char * ssid = "Wifi SSID";
char * seckey = "WiFi Password";
/* --------------------------------------------------------------------- */


volatile uint8_t console_input[1], console_count=0;
char console_ssid[40]={0};
char console_psk[20]={0};

char buffertoButon1[20];
char buffertoButon2[20];
char buffertoButon3[20];

char buffer_tim2[96];
volatile uint8_t butonControl = 0;
volatile uint8_t butonControl2 = 0;
volatile uint8_t butonControl3 = 0;


volatile char lora_read[95]={0};
volatile uint8_t lora_count =0;

/* ---------------- Timers Interrupts ------------*/
void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~(TIM_SR_UIF);//timer2 clear flag
	butonControl2 = 1; // buton-2's flag is high
}
void TIM3_IRQHandler(void)
{
	TIM3->SR &= ~(TIM_SR_UIF);//timer3 clear flag
	butonControl = 1; // buton-1's flag is high
}
void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~(TIM_SR_UIF);//timer4 clear flag
	butonControl3 = 1;  // buton-3's flag is high
}
/* -------------------------------------------------*/

int main(void)
{

  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();

  console(); // Entering wifi ssid and psk for user

  while(!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3))); // wait for LoRa module's AUX pin
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); //M0 = 0 NORMAL MODE
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); //M1 = 0
  HAL_Delay(10);
  /* Setting up and enabling timers */
  /* -------------*/
  timer2_init();
  timer3_init();
  timer4_init();
  timer4_enable();
  timer3_enable();
  timer2_enable();
 /* --------------- */

  while (1)
  {

	  /* Is there any data coming to the Tx pin of the LoRa module? */
	  /* ------------------------------------------------------------ */
	  if(IsDataAvailable(lora_uart))
	  {
		  do
		  {
			  while(!(IsDataAvailable(lora_uart)));
			  lora_read[lora_count] = Uart_read(lora_uart); // Reading incoming data
			  Uart_write(lora_read[lora_count], wifi_uart); // Writing incoming data to wifi module
			  lora_count++;
		  }
		  while(!(Look_for("3000\r", lora_read))); // Check port
		  lora_count=0;
	  }
	  /* ------------------------ End ----------------------------- */


	  /* ---------Did the user press the button-1? ----------*/
	  if(butonControl)
	  {
		 timer3_disable(); // Disable timer counting
		 butonControl = 0; // flag is low for recounting

		 Uart_sendstring("AT+S.HTTPGET=192.168.1.30,/button/get,3000\r",wifi_uart); // Send a request
		 Get_after("HTTP/1.1",20, &buffertoButon1, wifi_uart); // If the incoming response starts with HTTP, put it in the buffer
		  if((Look_for("201",buffertoButon1) == 1)) // If there is 201 in the answer, let node-1 start the process
		  {
			Uart_sendstring("one-run", lora_uart); // send the task
			while(!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)));// wait AUX=1

		  }
		  else if((Look_for("202",buffertoButon1) == 1)) // If there is 202 in the answer, let node-1 stop the process
		  {

			Uart_sendstring("one-stop", lora_uart); // send the task
			while(!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)));// wait AUX=1
		  }
		  timer3_enable(); // Enable timer counting
	  }
	 /* ------------------------------ End Buton-1 ------------------------------------------*/

		  /* ----------------------- Did the user press the button-2? ---------------------------*/
	  if(butonControl2)
	  {
		  timer2_disable(); // Disable timer counting
		  butonControl2 = 0; // flag is low for recounting
		  Uart_sendstring("AT+S.HTTPGET=192.168.1.30,/button/get2,3000\r",wifi_uart); // Send a request
		  Get_after("HTTP/1.1",20, &buffertoButon2, wifi_uart); // If the incoming response starts with HTTP, put it in the buffer
		  if((Look_for("201",buffertoButon2) == 1)) // If there is 201 in the answer, let node-2 start the process
		  {
			  Uart_sendstring("two-run", lora_uart); // send the task
			  while(!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)));// wait AUX=1

		  }
		  else if((Look_for("202",buffertoButon2) == 1)) // If there is 202 in the answer, let node-2 stop the process
		  {

			  Uart_sendstring("two-stop", lora_uart); // send the task
			  while(!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)));// wait AUX=1
		  }
		  timer2_enable(); // Enable timer counting
	  }
	  /* -------------------------- End Buton-2  -------------------------------- */


	  /* ----------------------- Did the user press the button-3? ---------------------------*/
	 if(butonControl3)
	  {
		  timer4_disable(); // Disable timer counting
		  butonControl3 = 0; // flag is low for recounting
		  Uart_sendstring("AT+S.HTTPGET=192.168.1.30,/button/get3,3000\r",wifi_uart); // Send a request
		  Get_after("HTTP/1.1",20, &buffertoButon3, wifi_uart); // If the incoming response starts with HTTP, put it in the buffer
		  if((Look_for("201",buffertoButon3) == 1)) // If there is 201 in the answer, let node-1 start the process
		  {
		  	Uart_sendstring("three-on", lora_uart); // send the task
		  	while(!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)));// wait AUX=1
		  }
		  else if((Look_for("202",buffertoButon3) == 1)) // If there is 202 in the answer, let node-1 stop the process
		  {
			Uart_sendstring("three-off", lora_uart); // send the task
		  	while(!(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)));// wait AUX=1
		  }
		  timer4_enable(); // Enable timer counting
	  }
	 /* ------------------------------ End Buton-3 ------------------------------------------*/

  }
  /* USER CODE END 3 */
}

void console(void)
{
	  Ringbuf_init();
	  HAL_Delay(100);
	  Uart_sendstring("\r\n\n/********************************************************\n",pc_uart);
	  Uart_sendstring("\r *                                                      *\n",pc_uart);
	  Uart_sendstring("\r * 			 Solar Powered              	*\n",pc_uart);
	  Uart_sendstring("\r * 		LoRa Based Smart Agriculture            *\n",pc_uart);
	  Uart_sendstring("\r * 			  IoT Project                   *\n",pc_uart);
	  Uart_sendstring("\r *                                                      *\n",pc_uart);
	  Uart_sendstring("\r *******************************************************\n",pc_uart);

	  Uart_sendstring("\r\nDo you want to setup SSID?(y/n):", pc_uart);
	  while(!(IsDataAvailable(&huart2)));
	  console_input[0] = Uart_read(pc_uart);
	  Uart_write(console_input[0], pc_uart);
	  Uart_sendstring("\r\n", pc_uart);
	  if(console_input[0]=='y')
	  {
	  	Uart_sendstring("Enter the SSID:", pc_uart);
	  	while(console_count<36)
	  	{
	  		while(!(IsDataAvailable(pc_uart)));
	  		console_ssid[console_count] = Uart_read(pc_uart);
	  		console_count++;
	  		if(Look_for("\r\n",console_ssid) == 1)
	  		{
	  			break;
	  		}
	  	}
	  	Uart_sendstring(console_ssid, &huart2);
	  	HAL_Delay(10);
	  	console_count=0;
	  	Uart_sendstring("Enter the password:", pc_uart);
	    while(console_count<19)
	  	{
	    	while(!(IsDataAvailable(pc_uart)));
	  		console_psk[console_count] = Uart_read(pc_uart);
	  		console_count++;
	  		if(Look_for("\r\n",console_psk) == 1)
	  		 {
	  			break;
	  		  }
	  	}
	  	Uart_sendstring(console_psk, &huart2);
	  	console_count=0;

	  	}
	  	else
	  	{
	  		Uart_sendstring("\r\n\nModule will connect with default settings.", pc_uart);
	  		memcpy(console_ssid, (const char*)ssid, strlen((char*)ssid));
	  		memcpy(console_psk, (const char*)seckey, strlen((char*)seckey));
	  		Uart_sendstring("\n", pc_uart);
	  		Uart_sendstring("SSID: ", pc_uart);
	  		Uart_sendstring(console_ssid, &huart2);
	  		Uart_sendstring("\n", pc_uart);
	  		Uart_sendstring("Password: ", pc_uart);
	  		Uart_sendstring(console_psk, &huart2);
	  	 }
	  	 wifi_Init(console_ssid,console_psk);
	  	 HAL_Delay(2000);
	  	 Uart_sendstring("\nReady..",pc_uart);
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
