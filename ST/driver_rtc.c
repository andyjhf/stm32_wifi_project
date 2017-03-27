/*******************************************************************************
 * @file    driver_rtc.c
 * @author  huafeng.jiang
 * @version V1.0.0
 * @date    2017-03-28
 * @brief   This file provides all LED/SWI/WIFI-Pin/LEFT functions.
 *******************************************************************************/

#include "driver_rtc.h"

	/* RTC handler declaration */
	RTC_HandleTypeDef RtcHandle;
	
void RTC_Init(void)
{
  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follow:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Error */
    Error_Handler(); 
  }	
}

int is_RTC_RESET(void)
{
	if(HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0) != 0x32F2)
  {  
		return 1;
  }
	return 0;
}

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void set_RTC(RTC_DATE_TIME *time)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Friday March 13th 2015 */
  sdatestructure.Year = RTC_ByteToBcd2(time->years - 2000);
  sdatestructure.Month = RTC_ByteToBcd2(time->months);
  sdatestructure.Date = RTC_ByteToBcd2(time->days);
  sdatestructure.WeekDay = time->weaks;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,FORMAT_BCD) != HAL_OK)
  {
    /* Error */
    Error_Handler(); 
  } 
  
  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = RTC_ByteToBcd2(time->hours);
  stimestructure.Minutes = RTC_ByteToBcd2(time->minute);
  stimestructure.Seconds = RTC_ByteToBcd2(time->second);
  stimestructure.TimeFormat = 0x00;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,FORMAT_BCD) != HAL_OK)
  {
    /* Error */
    Error_Handler(); 
  }
  
  /*##-3- Writes a data in a RTC Backup data Register0 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle,RTC_BKP_DR0,0x32F2);  
}

/**
  * @brief  Display the current time and date.
  * @param  showtime: pointer to buffer
  * @param  showdate: pointer to buffer
  * @retval None
  */
void get_RTC(RTC_DATE_TIME *time)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, FORMAT_BIN);
	
	time->years = sdatestructureget.Year + 2000;
	time->months = sdatestructureget.Month;
	time->days = sdatestructureget.Date;
	time->weaks = sdatestructureget.WeekDay;
	time->hours = stimestructureget.Hours;
	time->minute = stimestructureget.Minutes;
	time->second = stimestructureget.Seconds;
	
}


