#ifndef __DRIVER_IWDG_H
#define __DRIVER_IWDG_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "driver_utility.h"

void IWDG_Init(void);                              // initilize the independent watchdog
void IWDG_Feed(void);                              // refresh the watchdog

void IWDG_InitEx(void);                              // initilize the independent watchdog
void IWDG_FeedEx(void);                              // refresh the watchdog
	
#ifdef __cplusplus
}
#endif

#endif /*__DRIVER_IWDG_H */
