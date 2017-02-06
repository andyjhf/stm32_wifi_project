#ifndef __DRIVER_UART6_H
#define __DRIVER_UART6_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "driver_utility.h"

#define BUF_SIZE 255

// RS-485 half-duplex
typedef enum 
{
	USART_RX = 0,
	USART_TX = 1
} USART_STATUES;

typedef struct
{
	uint8_t       txBuff[BUF_SIZE+1];              // receive and transmit buffer
	uint8_t       txStart;                         // receive cursor 0-255
	uint8_t       txEnd;                           // transmit cursor 0-255
	uint8_t       rxBuff[BUF_SIZE+1];              // receive and transmit buffer
	uint8_t       rxStart;                         // receive cursor 0-255
	uint8_t       rxEnd;                           // transmit cursor 0-255
	USART_STATUES status;                          // uart status of receive or transmit
}USART_DATA;

void USART6_OffReceive(void);                      // turn off receiver
void USART6_PreReceive(void);                      // prepare for receiving
void USART6_PreTransmit(void);                     // prepare for transmitting
void USART6_StartSend(void);                       // start to transmit
void USART6_Init(void);                            // initialize USART6
uint8_t USART6_GetByte(uint8_t* pData);            // get a byte from USAR6T-buffer
uint8_t USART6_Write(uint8_t* pData, uint8_t len); // write some bytes to USART6-buffer
uint8_t USART6_GetStatus(void);                    // get usart6 state

#ifdef __cplusplus
}
#endif

#endif /*__DRIVER_UART6_H */

