/*******************************************************************************
 * @file    driver_uart2.c
 * @author  jianchuan.zhao
 * @version V1.0.0
 * @date    2016-01-20
 * @brief   This file provides all wifi uart functions.
 *******************************************************************************/

#include "driver_uart2.h"

void USART2_Init(void)
{
	// Configure the UART peripheral
	// Put the USART peripheral in the Asynchronous mode (UART Mode)
	// USART2 configured as follow:
	// - Word Length = 9 Bits
	// - Stop Bit = 1 Bit
	// - Parity = ODD parity
	// - BaudRate = 115200 baud
	// - Hardware flow control enabled (RTS and CTS signals)
	UART_HandleTypeDef UartHandle;
	GPIO_InitTypeDef GPIO_InitStructure;

	// RX
	RCC->AHB1ENR |= (USART2_RX_CLK);
	GPIO_InitStructure.Pin       = USART2_RX_PIN;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull      = GPIO_NOPULL;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = USART2_RX_AF;
	HAL_GPIO_Init(USART2_RX_PORT, &GPIO_InitStructure);

	// TX
	RCC->AHB1ENR |= (USART2_TX_CLK);
	GPIO_InitStructure.Pin       = USART2_TX_PIN;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull      = GPIO_NOPULL;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = USART2_TX_AF;
	HAL_GPIO_Init(USART2_TX_PORT, &GPIO_InitStructure);

	// RTS
	RCC->AHB1ENR |= (USART2_RTS_CLK);
	GPIO_InitStructure.Pin       = USART2_RTS_PIN;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull      = GPIO_NOPULL;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = USART2_RTS_AF;
	HAL_GPIO_Init(USART2_RTS_PORT, &GPIO_InitStructure);

	// CTS
	RCC->AHB1ENR |= (USART2_CTS_CLK);
	GPIO_InitStructure.Pin       = USART2_CTS_PIN;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull      = GPIO_PULLUP;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = USART2_CTS_AF;
	HAL_GPIO_Init(USART2_CTS_PORT, &GPIO_InitStructure);

	// Enable the USART2 periph clock and set the communication configures
	__USART2_CLK_ENABLE();
	UartHandle.Instance        = USART2;
	UartHandle.Init.BaudRate   = 115200;
	UartHandle.Init.WordLength = UART_WORDLENGTH_9B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_ODD;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_RTS_CTS;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		Error_Handler();
	}

	// Enable USART2
	__HAL_UART_ENABLE(&UartHandle);
}

__weak void uart_recv(uint8_t dr)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the uart_recv could be implemented in the user file
   */
}

__weak uint8_t *uart_tx(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the uart_tx could be implemented in the user file
   */
	return 0;
}

/*
 * Poll for data. If RXNE, receive the packet. If TXE, see if there's another
 * packet to transmit.
 */
void USART2_Poll(void)
{
	uint8_t *dr;

	if(USART2->SR & USART_SR_RXNE)
	{
		uart_recv(USART2->DR & 0xff);
	}
	if(USART2->SR & USART_SR_TXE)
	{
		dr = uart_tx();
		if(dr != NULL)
		{
			USART2->DR = (*dr) & 0xff;
		}
	}
}
