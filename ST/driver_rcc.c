/*******************************************************************************
 * @file    driver_rcc.c
 * @author  jianchuan.zhao
 * @version V1.0.0
 * @date    2016-01-20
 * @brief   This file provides all RCC functions.
 *******************************************************************************/

#include "driver_rcc.h"

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE_CRYSTAL or HSE_BYPASS) 
  *            SYSCLK(Hz)                     = 60000000
  *            HCLK(Hz)                       = 60000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 12000000
  *            PLL_M                          = 12
  *            PLL_N                          = 240
  *            PLL_P                          = 4
  *            PLL_Q                          = 5
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale2 mode
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
void RCC_SystemClockHSE_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	// Enable Power Control clock
	__PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is 
	 clocked below the maximum system frequency, to update the voltage scaling value 
	 regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	// 1.Select HSI as system clock source to allow modification of the PLL configuration
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	{
		Error_Handler();
	}

	// 2.Enable HSE Oscillator, select it as PLL source and finally activate the PLL
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;

	// 2014.04.06
//#ifdef HSE_CRYSTAL  
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//#elif defined (HSE_BYPASS)
//	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
//#endif
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 12;
	RCC_OscInitStruct.PLL.PLLN = 240;              // (336, 84M uart6 baudrate is not right);
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;    // AHB 60M
	RCC_OscInitStruct.PLL.PLLQ = 5;                // USB OTG must be keep 48M
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		// Initialization Error
		Error_Handler();
	}

	// 2014.04.07
	// 3.Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK; // HSE->PLL->SYSCLK
	RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;         // 
	RCC_ClkInitStruct.APB1CLKDivider = RCC_CFGR_PPRE1_DIV2;     // 30MHz 2014.04.07:RCC_HCLK_DIV2=RCC_CFGR_PPRE1_DIV2
	RCC_ClkInitStruct.APB2CLKDivider = RCC_CFGR_PPRE2_DIV1;     // 60MHz 2014.04.07:ST lib bug(APB2 used RCC_HCLK_DIV1=RCC_CFGR_PPRE1_DIV1)
	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}

	// 4.Optional: Disable HSI Oscillator (if the HSI is no more needed by the application)
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		// Initialization Error
		Error_Handler();
	}
}
