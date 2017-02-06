/*******************************************************************************
 * @file    driver_iwdg.c
 * @author  jianchuan.zhao
 * @version V1.0.0
 * @date    2016-01-26
 * @brief   This file provides IWDG functions.
 *******************************************************************************/
 
#include "driver_iwdg.h"

static IWDG_HandleTypeDef IWDG_Handle;
	
void IWDG_Init(void)
{
	IWDG_Handle.Instance       = IWDG;               // Register base address 
	IWDG_Handle.Init.Prescaler = IWDG_PRESCALER_32;  // Select the prescaler of the IWDG  IWDG_Prescaler  32kHz(LSI)/32=1kHz
	IWDG_Handle.Init.Reload    = 256;                // Specifies the IWDG down-counter reload value between (0 ~ 0xfff) 256ms
//	IwdgHandle.Lock           =;                     // IWDG locking object
//	IwdgHandle.State          =;                     // IWDG communication state

	// TODO: maybe dump in some boards
//	HAL_IWDG_Init(&IWDG_Handle);
//	if(HAL_IWDG_Start(&IWDG_Handle) != HAL_OK)
//	{
//		//Error_Handler();
//	}
}

void IWDG_Feed(void)
{
	IWDG_Handle.Instance       = IWDG;               // Register base address 
	IWDG_Handle.Init.Prescaler = IWDG_PRESCALER_32;  // Select the prescaler of the IWDG  IWDG_Prescaler  32kHz(LSI)/32=1kHz
	IWDG_Handle.Init.Reload    = 256;                // Specifies the IWDG down-counter reload value between (0 ~ 0xfff) 256ms
//	IwdgHandle.Lock           =;                     // IWDG locking object
//	IwdgHandle.State          =;                     // IWDG communication state

	// TODO: maybe dump in some boards
	// Reload IWDG counter
//	if(HAL_IWDG_Refresh(&IWDG_Handle) != HAL_OK)
//	{
//		// Error_Handler();
//	}
}

void IWDG_InitEx(void)
{
	IWDG_Handle.Instance       = IWDG;               // Register base address 
	IWDG_Handle.Init.Prescaler = IWDG_PRESCALER_32;  // Select the prescaler of the IWDG  IWDG_Prescaler  32kHz(LSI)/32=1kHz
	IWDG_Handle.Init.Reload    = 256;                // Specifies the IWDG down-counter reload value between (0 ~ 0xfff) 256ms
//	IwdgHandle.Lock           =;                     // IWDG locking object
//	IwdgHandle.State          =;                     // IWDG communication state

	// TODO: maybe dump in some boards
	HAL_IWDG_Init(&IWDG_Handle);
	if(HAL_IWDG_Start(&IWDG_Handle) != HAL_OK)
	{
		//ERROR_HANDLER();
	}
}

void IWDG_FeedEx(void)
{
	IWDG_Handle.Instance       = IWDG;               // Register base address 
	IWDG_Handle.Init.Prescaler = IWDG_PRESCALER_32;  // Select the prescaler of the IWDG  IWDG_Prescaler  32kHz(LSI)/32=1kHz
	IWDG_Handle.Init.Reload    = 256;                // Specifies the IWDG down-counter reload value between (0 ~ 0xfff) 256ms
//	IwdgHandle.Lock           =;                     // IWDG locking object
//	IwdgHandle.State          =;                     // IWDG communication state

	// TODO: maybe dump in some boards
	// Reload IWDG counter
	if(HAL_IWDG_Refresh(&IWDG_Handle) != HAL_OK)
	{
		// Error_Handler();
	}
}
