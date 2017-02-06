#ifndef __DRIVER_UART2_H
#define __DRIVER_UART2_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "driver_utility.h"


/******************* Platform-specific function headers *******************/

void USART2_Init(void);                            // Set the USART2 settings
void USART2_Poll(void);                            // Poll for data. If RXNE, receive the packet.


void uart_recv(uint8_t dr);                        // receive callback function
uint8_t *uart_tx(void);                            // transmit callbak function


#ifdef __cplusplus
}
#endif

#endif /*__DRIVER_UART2_H */
