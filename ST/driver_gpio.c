
/*******************************************************************************
 * @file    driver_rcc.c
 * @author  jianchuan.zhao
 * @version V1.0.0
 * @date    2016-01-20
 * @brief   This file provides all LED/SWI/WIFI-Pin/LEFT functions.
 *******************************************************************************/

#include "driver_gpio.h"

static GPIO_TypeDef*  const GPIO_LED_PORT[LEDn] = {LED1_PORT,LED2_PORT};
static uint16_t       const GPIO_LED_PIN[LEDn]  = {LED1_PIN, LED2_PIN};
static uint32_t       const GPIO_LED_CLK[LEDn]  = {LED1_CLK, LED2_CLK};
static uint8_t        GPIO_LED_STATUS[LEDn]     = {0,0};
	
static GPIO_TypeDef*  const GPIO_SWI_PORT[SWIn] = {SWI1_PORT,SWI2_PORT,SWI3_PORT,SWI4_PORT,SWI5_PORT,SWI6_PORT};
static uint16_t       const GPIO_SWI_PIN[SWIn]  = {SWI1_PIN, SWI2_PIN, SWI3_PIN, SWI4_PIN, SWI5_PIN, SWI6_PIN};
static uint32_t       const GPIO_SWI_CLK[SWIn]  = {SWI1_CLK, SWI2_CLK, SWI3_CLK, SWI4_CLK, SWI5_CLK, SWI6_CLK};

 
void LED_Init(LED_TypeDef led)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	// Check the parameters
  assert_param((led<LEDn));
	
	// GPIO_LED Peripheral Clock enable
	RCC->AHB1ENR |= (GPIO_LED_CLK[led]);

	// Configure the GPIO_LED pin
	GPIO_InitStructure.Pin   = GPIO_LED_PIN[led];
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Speed  = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIO_LED_PORT[led], &GPIO_InitStructure);
	
	LED_Off(led);
}

void LED_On(LED_TypeDef led)
{
	// Check the parameters
  assert_param((led<LEDn));
	
	// reset the corresponding ODx bit for led light on	
	HAL_GPIO_WritePin(GPIO_LED_PORT[led], GPIO_LED_PIN[led], GPIO_PIN_RESET);
	
	GPIO_LED_STATUS[led] = 1;
}

void LED_Off(LED_TypeDef led)
{
	// Check the parameters
  assert_param((led<LEDn));

	// set the corresponding ODRx bit for led light off
	HAL_GPIO_WritePin(GPIO_LED_PORT[led], GPIO_LED_PIN[led], GPIO_PIN_SET);
	
	GPIO_LED_STATUS[led] = 0;
}

void LED_Toggle(LED_TypeDef led)
{
	// Check the parameters
  assert_param((led<LEDn));
	
	// toggle the specified led
	HAL_GPIO_TogglePin(GPIO_LED_PORT[led],GPIO_LED_PIN[led]);
	
	GPIO_LED_STATUS[led] = 0;
}

void LED_Set(LED_TypeDef led, uint8_t onoff)
{
	// Check the parameters
  assert_param((led<LEDn));

	if(onoff)
	{
		HAL_GPIO_WritePin(GPIO_LED_PORT[led], GPIO_LED_PIN[led], GPIO_PIN_RESET);
		GPIO_LED_STATUS[led] = 1;
	}
	else
	{
		HAL_GPIO_WritePin(GPIO_LED_PORT[led], GPIO_LED_PIN[led], GPIO_PIN_SET);
		GPIO_LED_STATUS[led] = 0;
	}
}

uint8_t LED_Get(LED_TypeDef led)
{
	// Check the parameters
  assert_param((led<LEDn));
	return GPIO_LED_STATUS[led];
}

void SWI_Init(SWI_TypeDef swi)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Check the parameters
  assert_param((swi<SWIn));
	
	// Enable the SWI Clock
	RCC->AHB1ENR |= (GPIO_SWI_CLK[swi]);

	// Configure SWI pin as input
	GPIO_InitStructure.Pin  = GPIO_SWI_PIN[swi];
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed  = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIO_SWI_PORT[swi], &GPIO_InitStructure);
}

uint8_t SWI_GetState(SWI_TypeDef swi)
{
	// Check the parameters
  assert_param((swi<SWIn));

	return (HAL_GPIO_ReadPin(GPIO_SWI_PORT[swi], GPIO_SWI_PIN[swi]) == GPIO_PIN_SET);
}

void WIFI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Configure GPIO pins: WiFi-LINK_N(WiFi PP signal)
	RCC->AHB1ENR |= (WIFI_LINK_CLK);
	GPIO_InitStructure.Pin   = WIFI_LINK_PIN;
	GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(WIFI_LINK_PORT, &GPIO_InitStructure);

	// Configure GPIO pins: WiFi-READY_N (WiFi OD signal)
	RCC->AHB1ENR |= (WIFI_READY_CLK);
	GPIO_InitStructure.Pin   = WIFI_READY_PIN;
	GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull  = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(WIFI_READY_PORT, &GPIO_InitStructure);

	// Configure GPIO pins: WiFi-RESET_N
	RCC->AHB1ENR |= (WIFI_RESET_CLK);
	GPIO_InitStructure.Pin   = WIFI_RESET_PIN;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStructure.Pull  = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(WIFI_RESET_PORT, &GPIO_InitStructure);

	// Keep wifi module pull up
	HAL_GPIO_WritePin(WIFI_RESET_PORT, WIFI_RESET_PIN, GPIO_PIN_SET);
}

uint8_t WIFI_Ready(void)
{
	// open-drain signal of wifi module, and be pulled up by external resistance
	// wifi module will go low after initialized
	return (HAL_GPIO_ReadPin(WIFI_READY_PORT, WIFI_READY_PIN) == GPIO_PIN_RESET);
}

uint8_t WIFI_Link(void)
{
	// wifi module will go low whenever it connected to the ADS service
	return (HAL_GPIO_ReadPin(WIFI_LINK_PORT, WIFI_LINK_PIN) == GPIO_PIN_RESET);
}

void WIFI_Reset(uint8_t reset)
{
	// wifi module pulls it up, it should be driven low  by the MCU to reset the wifi module
	if(reset)
	{
		HAL_GPIO_WritePin(WIFI_RESET_PORT, WIFI_RESET_PIN, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(WIFI_RESET_PORT, WIFI_RESET_PIN, GPIO_PIN_SET);
	}
}

void LEFT_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	// Configure the LEFT_1 pin
	RCC->AHB1ENR |= (LEFT_1_CLK);
	GPIO_InitStructure.Pin   = LEFT_1_PIN;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull  = GPIO_PULLUP;
	GPIO_InitStructure.Speed  = GPIO_SPEED_LOW;
	HAL_GPIO_Init(LEFT_1_PORT, &GPIO_InitStructure);

	// Configure the LEFT_2 pin
	RCC->AHB1ENR |= (LEFT_2_CLK);
	GPIO_InitStructure.Pin   = LEFT_2_PIN;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull  = GPIO_PULLUP;
	GPIO_InitStructure.Speed  = GPIO_SPEED_LOW;
	HAL_GPIO_Init(LEFT_2_PORT, &GPIO_InitStructure);

	// Configure the LEFT_3 pin
	RCC->AHB1ENR |= (LEFT_3_CLK);
	GPIO_InitStructure.Pin   = LEFT_3_PIN;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull  = GPIO_PULLUP;
	GPIO_InitStructure.Speed  = GPIO_SPEED_LOW;
	HAL_GPIO_Init(LEFT_3_PORT, &GPIO_InitStructure);
}
