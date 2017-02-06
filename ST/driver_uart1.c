/*******************************************************************************
 * @file    driver_usart1.c
 * @author  jianchuan.zhao
 * @version V1.0.0
 * @date    2016-02-02
 * @brief   This file provides all hlink uart functions.
 *******************************************************************************/
 
#include "driver_uart1.h"

HLINK_DATA	g_usart1;

TIM_HandleTypeDef  TimHandle;

void USART1_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	UART_HandleTypeDef UartHandle;

	// RX
	RCC->AHB1ENR |= (USART1_RX_CLK);
	GPIO_InitStructure.Pin  = USART1_RX_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed  = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = USART1_RX_AF;
	HAL_GPIO_Init(USART1_RX_PORT, &GPIO_InitStructure);

	// TX
	RCC->AHB1ENR |= (USART1_TX_CLK);
	GPIO_InitStructure.Pin  = USART1_TX_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed  = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = USART1_RX_AF;
	HAL_GPIO_Init(USART1_TX_PORT, &GPIO_InitStructure);

	// Enable the USART6 periph clock and set the communication configures
	__USART1_CLK_ENABLE();
	UartHandle.Instance          = USART1;
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
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	// Enable USART6
	__HAL_UART_ENABLE(&UartHandle);

	// 
	USART1->CR1 &= ~USART_CR1_RE;                  // Receiver disable
	USART1->SR &=  ~USART_SR_TC;                   // clear TC interrupt flag
	USART1->SR &=  ~USART_SR_RXNE;                 // clear RXNE interrupt flag
	USART1->CR1 &= ~USART_CR1_RXNEIE;              // disable RXNE interrupt

	// CK
	RCC->AHB1ENR |= (USART1_CK_CLK);
	GPIO_InitStructure.Pin  = USART1_CK_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed  = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Alternate = USART1_CK_AF;
	HAL_GPIO_Init(USART1_CK_PORT, &GPIO_InitStructure);

	// Timer Output Compare Configuration Structure declaration
	RCC->APB2ENR |= (RCC_APB2ENR_TIM1EN);
	GPIOA->ODR   |= 1<<8;                          // PA8 
//	USART1_CK_Init();

	// RX-INT
	RCC->AHB1ENR |= (USART1_INT_CLK);
	GPIO_InitStructure.Pin  = USART1_INT_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed  = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(USART1_INT_PORT, &GPIO_InitStructure);

	// EXTI interrupt init
//	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	// MM1192-RESET
	RCC->AHB1ENR |= (USART1_RESET_CLK);
	GPIO_InitStructure.Pin  = USART1_RESET_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed  = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(USART1_RESET_PORT, &GPIO_InitStructure);

	// enable MM1192 transmission
	HAL_GPIO_WritePin(USART1_RESET_PORT, USART1_RESET_PIN, GPIO_PIN_RESET);
}

void USART1_IRQHandler(void)
{
	// Received completed interrupt(RXNE) if RXNEIE=1 in the USARTx_CR1 register
	if((USART1->SR & USART_SR_RXNE) && (USART1->CR1&USART_CR1_RXNEIE ))
	{
		USART1_Recv(USART1->DR);                   // Auto cleared by a read to the DR
	}
	// Transmit completed interrupt(TC) if TCIE=1 in the USARTx_CR1 register
	else if((USART1->SR & USART_SR_TC) && (USART1->CR1 & USART_CR1_TCIE))
	{
		USART1->SR &=  ~USART_SR_TC;               // clear TC interrupt flag
		USART1->SR &=  ~USART_SR_RXNE;             // clear RXNE interrupt flag
		TIM1->CR1  &=  ~TIM_CR1_CEN;               // disable counter
		TIM1->CCMR1 &=~TIM_OCMODE_PWM1;
		TIM1->CCMR1 |= TIM_OCMODE_FORCED_INACTIVE; // TIM_OCMODE_FORCED_ACTIVE;
		USART1_SendDR();
	}
	// TXE interrupt(Transmit Data Register Empty)
	else if((USART1->SR & USART_SR_TXE)  && (USART1->CR1 & USART_CR1_TXEIE))
	{
		USART1->CR1 &= ~USART_CR1_TXEIE;           // disable TXE interrupt
		USART1_CK_Start();
	}
	else
	{
		uint8_t data = USART1->DR;                 // Note: STM32F411 can be cleared by a read to the DR
	}
}

__weak void USART1_Recv(uint8_t dr)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the uart_recv could be implemented in the user file
   */
}

__weak uint8_t USART1_Send(uint8_t *data)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the uart_tx could be implemented in the user file
   */
	return 0;
}

void USART1_SendDR(void)
{
	static uint8_t txData;
	if(USART1_Send(&txData))
	{
		USART1->CR1 |= USART_CR1_TXEIE;            // enable TXE interrupt
		USART1->DR = txData;
	}
}

void USART1_CK_Init(void)
{
	TIM1->ARR = (60000000/9600)-1;                 // set auto-reload value(Auto-reload register)
	TIM1->PSC = 0;                                 // set prescaler
	TIM1->CCMR1 |= TIM_OCMODE_PWM1;                // capture/compare mode register
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE;                // Output Compare 1 preload enable
	TIM1->CR1   |= TIM_CR1_ARPE;                   // 1<<7 ARPE	使能自动重装载预装载允许位
	TIM1->CR1   |= TIM_COUNTERMODE_UP;             // counter up mode
	TIM1->CCER  |= TIM_OUTPUTSTATE_ENABLE;
	TIM1->CCER  |= TIM_OCPOLARITY_LOW;             // (TIM_CCER_CC1E|TIM_CCER_CC1P); output enable and output active high
	TIM1->CCR1  = TIM1->ARR/2;                     // 
	TIM1->CNT   = 0;                               // 
	TIM1->BDTR |= (TIM_BDTR_MOE);                  // 
//	TIM1->CR1|=TIM_CR1_CEN;                        // counter enable
}

void USART1_CK_Start(void)
{
	TIM1->ARR=6249;                                // set auto-reload value(Auto-reload register)
//	TIM1->PSC=0;                                   // set prescaler
//	TIM1->CCMR1 = 0;
	TIM1->CCMR1 |= (TIM_OCMODE_PWM1|TIM_CCMR1_OC1PE);//CH1 PWM1方式,TIM_CCMR1_OC1PE:CH1预装载使能
//	TIM1->CCMR1|= TIM_CCMR1_OC1PE;                 // 1<<3;  // CH1	CH1预装载使能
//	TIM1->CR1|=TIM_CR1_ARPE;                       // 1<<7;  // ARPE 使能自动重装载预装载允许位
//	TIM1->CR1|=TIM_COUNTERMODE_UP;                 // counter up mode
	TIM1->CCER |= (TIM_OCPOLARITY_LOW|TIM_OUTPUTSTATE_ENABLE);//output active high and output enable
	TIM1->CCR1  = 3124;                            // TIM1->ARR/2
	TIM1->CNT   = 0;                               // 
	TIM1->BDTR |= (TIM_BDTR_MOE);
	TIM1->CR1|=(TIM_CR1_ARPE|TIM_CR1_CEN);         // TIM_CR1_ARPE:使能自动重装载预装载允许位  TIM_CR1_CEN:CONTER enable 
}
