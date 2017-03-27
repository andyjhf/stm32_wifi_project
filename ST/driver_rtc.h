#ifndef __DRIVER_RTC_H
#define __DRIVER_RTC_H

#ifdef __cplusplus
	extern "C" {
#endif
#include "driver_utility.h"
		
/* Defines related to Clock configuration */    
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */		

typedef struct
{
	uint32_t      years;              						
	uint8_t       months;                        
	uint8_t       days;                       
	uint8_t       weaks;              			
	uint8_t       hours;                 
	uint8_t       minute;                 
	uint8_t				second;             
}RTC_DATE_TIME;		
		
void RTC_Init(void);
int is_RTC_RESET(void);
void set_RTC(RTC_DATE_TIME *time);
void get_RTC(RTC_DATE_TIME *time);
			
#ifdef __cplusplus
}
#endif

#endif /*__DRIVER_RTC_H */
