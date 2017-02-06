#ifndef __DRIVER_GPIO_H
#define __DRIVER_GPIO_H

#ifdef __cplusplus
	extern "C" {
#endif
#include "driver_utility.h"


typedef enum 
{
	LED_OFF = 0,
	LED_ON  = 1
} LED_ACTION;


void LED_Init(LED_TypeDef led);
void LED_On(LED_TypeDef led);
void LED_Off(LED_TypeDef led);
void LED_Toggle(LED_TypeDef led);
void LED_Set(LED_TypeDef led, uint8_t onoff);
uint8_t LED_Get(LED_TypeDef led);

void    SWI_Init(SWI_TypeDef swi);                 // Configure switch pin
uint8_t SWI_GetState(SWI_TypeDef swi);             // Get the level(low/high) of each line

void    WIFI_Init(void);                           // Set the READY_N/RESET_N/LINK_N settings
void    WIFI_Reset(uint8_t reset);                 // Reset the wifi module
uint8_t WIFI_Ready(void);                          // Get the wifi module ready state
uint8_t WIFI_Link(void);                           // Get the wifi module link state

void    LEFT_Init(void);                           // configure preserved pins

#ifdef __cplusplus
}
#endif

#endif /*__DRIVER_GPIO_H */
