#ifndef __DRIVER_UART1_H
#define __DRIVER_UART1_H

#ifdef __cplusplus
	extern "C" {
#endif
	
#include "driver_utility.h"

#define BUF_SIZE_MAX 255
		
// h-link
typedef enum 
{
	HLINK_RX = 0,
	HLINK_TX = 1
} HLINK_STATUES;

typedef struct
{
	uint8_t       rxBuff[BUF_SIZE_MAX+1];          // receive buffer
	uint8_t       rxStart;                         // receive start cursor 0-255
	uint8_t       rxEnd;                           // receive end cursor 0-255
	uint8_t       txBuff[BUF_SIZE_MAX+1];          // transmit buffer
	uint8_t       txStart;                         // transmit start cursor 0-255
	uint8_t       txEnd;                           // transmit end cursor 0-255
	HLINK_STATUES status;
}HLINK_DATA;

void USART1_Init(void);                            // initialize uasrt1
void USART1_SendDR(void);                          // send a data
void USART1_CK_Init(void);                         // initialize TX-CLK signal
void USART1_CK_Start(void);                        // start TX-CLK signal

// Callbacks
void    USART1_Recv(uint8_t data);                 // receive callback function
uint8_t USART1_Send(uint8_t *data);                // transmit callback function

#ifdef __cplusplus
}
#endif

#endif /*__DRIVER_UART1_H */
