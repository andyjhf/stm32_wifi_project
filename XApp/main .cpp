/**
  ******************************************************************************
  * @file    main.c 
  * @author  jianchuan.zhao
  * @version V1.0.0
  * @date    18-Jan-2016
  * @brief   Main program body
  ******************************************************************************
  *
  *
  *
  ******************************************************************************
  */

#include "main.h"

int main(int argc, char **argv)
{
	HAL_Init();                                    // STM32F4xx HAL library initialization:
	RCC_SystemClockHSE_Config();                   // configure the System clock to 60 MHz
	HAL_InitTick(TICK_INT_PRIORITY);               // configure the Systick to generate an interrupt each 1 msec

	WIFI_Init();                                   // initialize all drivers of mcu peripheral
	LED_Init(LED2);                                // config LED2 pin
	LED_Init(LED3);                                // config LED3 pin
	SWI_Init(SW1_1);                               // config SW1_1 pin
	SWI_Init(SW1_2);                               // config SW1_2 pin
	SWI_Init(SW1_3);                               // config SW1_3 pin
	SWI_Init(SW1_4);                               // config SW1_4 pin
	SWI_Init(SW1_5);                               // config SW1_5 pin
	SWI_Init(SW1_6);                               // config SW1_6 pin
	LEFT_Init();                                   // config preserved pins
	USART2_Init();                                 // config usart2 peripheral and pins
	USART6_Init();                                 // config usart6 peripheral and pins
	USART1_Init();                                 // config usart1 peripheral and pins
	I2C_Init();                                    // config I2C1 peripheral and pins
	IWDG_Init();                                   // config independent watchdog

#ifdef DEBUG
//	SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
#endif
	XApp_Init();                                   // initialize app tasks
	XApp_Run();                                    // run all app tasks
	
}
