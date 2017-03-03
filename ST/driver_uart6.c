/*******************************************************************************
 * @file    driver_usart6.c
 * @author  jianchuan.zhao
 * @version V1.0.0
 * @date    2016-01-26
 * @brief   This file provides all rs485 functions.
 *******************************************************************************/
 
#include "driver_uart6.h"

USART_DATA	g_usart6;

void USART6_IRQHandler(void)
{
	// Received completed interrupt(RXNE) if RXNEIE=1 in the USARTx_CR1 register
	if((USART6->SR & USART_SR_RXNE) && (USART6->CR1&USART_CR1_RXNEIE ))
	{
		// Auto cleared by a read to the DR
		g_usart6.rxBuff[g_usart6.rxEnd++] = USART6->DR;
	}
	// Transmit completed interrupt(TC) if TCIE=1 in the USARTx_CR1 register
	else if((USART6->SR & USART_SR_TC) && (USART6->CR1 & USART_CR1_TCIE))
	{
		if(g_usart6.txStart!=g_usart6.txEnd)
		{
			// Auto cleared by a write to the DR
			USART6->DR= g_usart6.txBuff[g_usart6.txStart++];
		}
		else
		{
			USART6_PreReceive();
		}
	}
	else
	{
		// Note: STM32F411 can be cleared by a read to the DR
		g_usart6.rxBuff[g_usart6.rxEnd++] = USART6->DR;
	}
}

void USART6_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	UART_HandleTypeDef UartHandle;

	// Configure ERV_RS485-DE pin
	RCC->AHB1ENR |= (RS485_DE_CLK);
	GPIO_InitStructure.Pin   = RS485_DE_PIN;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(RS485_DE_PORT, &GPIO_InitStructure);

	// Driver ERV_RS485-DE pin low first(RS485 chip receive state)
	HAL_GPIO_WritePin(RS485_DE_PORT, RS485_DE_PIN, GPIO_PIN_RESET);

	// Connect RX pin to USART6_RX, enable PINx alternate functions
	RCC->AHB1ENR |= USART6_RX_CLK;
	GPIO_InitStructure.Pin       = USART6_RX_PIN;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull      = GPIO_NOPULL;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = USART6_RX_AF;
	HAL_GPIO_Init(USART6_RX_PORT, &GPIO_InitStructure);

	// Connect TX pin to USART6_TX, enable PINx alternate functions
	RCC->AHB1ENR |= USART6_TX_CLK;
	GPIO_InitStructure.Pin       = USART6_TX_PIN;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull      = GPIO_NOPULL;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = USART6_TX_AF;
	HAL_GPIO_Init(USART6_TX_PORT, &GPIO_InitStructure);

	// Enable the USART6 periph clock and set the communication configures
	__USART6_CLK_ENABLE();
	UartHandle.Instance          = USART6;
	UartHandle.Init.BaudRate     = 9600;
	UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits     = UART_STOPBITS_1;
	UartHandle.Init.Parity       = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode         = UART_MODE_TX_RX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		Error_Handler();
	}

	// Enable USART6_IRQn
	HAL_NVIC_SetPriority(USART6_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART6_IRQn);

	// Set USART6 receive state
	USART6_PreReceive();

	// Enable USART6
	__HAL_UART_ENABLE(&UartHandle);
}

void USART6_OffReceive(void)
{
	USART6->CR1 &= ~USART_CR1_RE;                  // Receiver disable
}

void USART6_PreReceive(void)
{
	// note: STM32F411 can be cleared by writing a '0'
	USART6->SR &=  ~USART_SR_TC;                   // clear TC interrupt flag
	USART6->SR &=  ~USART_SR_RXNE;                 // clear RXNE interrupt flag
	USART6->CR1 &= ~USART_CR1_TCIE;                // disable TC interrupt
	USART6->CR1 |= USART_CR1_RXNEIE;               // enable RXNE interrupt
	USART6->CR1 |= USART_CR1_RE;                   // Receiver Enable

	RS485_DE_PORT->BSRRH = RS485_DE_PIN;           // reset receive status(0) of RS485 chip

	g_usart6.rxStart= 0;                           // clear buffer and set receive state
	g_usart6.rxEnd  = 0;
	g_usart6.status = USART_RX;                    // set usart state for receving
}

void USART6_PreTransmit(void)
{
	USART6->SR &=  ~USART_SR_TC;                   // clear TC interrupt flag
	USART6->SR &=  ~USART_SR_RXNE;                 // clear RXNE interrupt flag
	USART6->CR1 &= ~USART_CR1_RXNEIE;              // disable RXNE interrupt
	USART6->CR1 |= USART_CR1_TCIE;                 // enable TC interrupt
	USART6->CR1 &= ~USART_CR1_RE;                  // Receiver disable

	RS485_DE_PORT->BSRRL = RS485_DE_PIN;           // set transmit status(1) of RS485 chip
	g_usart6.txStart  = 0;                         // clear buffer and set transmit state
	g_usart6.txEnd    = 0;                         // note: do not clear end 
	g_usart6.status = USART_TX;                    // set usart state for transmitting
}

void USART6_StartSend(void)
{
	// Transmit first data to DR
	USART6->DR = g_usart6.txBuff[g_usart6.txStart++];
}

uint8_t USART6_GetByte(uint8_t* pData)
{
	uint8_t size=0;
	if(g_usart6.rxStart != g_usart6.rxEnd)
	{
		pData[size++] = g_usart6.rxBuff[g_usart6.rxStart++];
	}
	return size;
}

uint8_t USART6_Write(uint8_t* pData, uint8_t len)
{
	while(len--)
	{
		g_usart6.txBuff[g_usart6.txEnd++] = *pData++;
	}
	return g_usart6.txEnd;
}

uint8_t USART6_GetStatus(void)
{
	return g_usart6.status;
}
