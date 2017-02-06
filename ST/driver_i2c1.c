/*******************************************************************************
 * @file    driver_i2c1.c
 * @author  jianchuan.zhao
 * @version V1.0.0
 * @date    2016-02-02
 * @brief   This file provides I2C1 functions.
 *******************************************************************************/
 
#include "driver_i2c1.h"

//static I2C_HandleTypeDef I2C1_Handle;
//uint8_t I2C1_Status=0;
//	
//void I2C1_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;

//	// SCL
//	RCC->AHB1ENR |= (I2C1_SCL_CLK);
//	GPIO_InitStructure.Pin       = I2C1_SCL_PIN;
//	GPIO_InitStructure.Mode      = GPIO_MODE_AF_OD;       // external Pull-up
//	GPIO_InitStructure.Pull      = GPIO_NOPULL;
//	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
//	GPIO_InitStructure.Alternate = I2C1_AF;
//	HAL_GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStructure);

//	// SDA
//	RCC->AHB1ENR |= (I2C1_SDA_CLK);
//	GPIO_InitStructure.Pin       = I2C1_SDA_PIN;
//	GPIO_InitStructure.Mode      = GPIO_MODE_AF_OD;       // external Pull-up
//	GPIO_InitStructure.Pull      = GPIO_NOPULL;
//	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
//	GPIO_InitStructure.Alternate = I2C1_AF;
//	HAL_GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);

//	// TODO: not check
//	// I2C1 configuration
//	__I2C1_FORCE_RESET();
//	__I2C1_RELEASE_RESET();

//	__I2C1_CLK_ENABLE();
//	I2C1_Handle.Instance             = I2C1;
//	I2C1_Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
//	I2C1_Handle.Init.ClockSpeed      = 200000;
//	I2C1_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
//	I2C1_Handle.Init.DutyCycle       = I2C_DUTYCYCLE_2;
//	I2C1_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
//	I2C1_Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
//	I2C1_Handle.Init.OwnAddress1     = I2C_DEV_ADDR;
//	I2C1_Handle.Init.OwnAddress2     = 0;
//	if(HAL_I2C_Init(&I2C1_Handle) != HAL_OK)
//	{
//		Error_Handler();
//	}

//	HAL_I2CEx_AnalogFilter_Config(&I2C1_Handle, I2C_ANALOGFILTER_ENABLED);
//	
//	// TODO: not check
//	// NVIC for I2C1
////	HAL_NVIC_SetPriority(I2C1_IRQn, 0, 0);                //i2c1
////  HAL_NVIC_EnableIRQ(I2C1_IRQn);
//	
//	HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
//	HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 1);
//	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
//	
//	__HAL_I2C_ENABLE_IT(&I2C1_Handle,I2C_IT_EVT);
//}

//uint8_t I2C1_GetState(void)
//{
//	return (uint8_t)HAL_I2C_GetState(&I2C1_Handle);
//}

//uint8_t I2C1_Write(uint8_t memAddr, uint8_t *buffer, uint8_t nBytes)
//{
//	// check the current state of the I2C1 peripheral before starting a new commnunication transfer
//	if(HAL_I2C_GetState(&I2C1_Handle) != HAL_I2C_STATE_READY)
//	{
//		return 1;
//	}

//	// Start the transmission process While the I2C in reception process, user can transmit data through buffer
//	if(HAL_I2C_Mem_Write_IT(&I2C1_Handle, (uint16_t)I2C_DEV_ADDR, memAddr, nBytes, buffer, nBytes)== HAL_OK)
//	{
//		I2C1_Status=1;
//		return 0;
//	}

//	if (HAL_I2C_GetError(&I2C1_Handle) != HAL_I2C_ERROR_AF)
//	{
//		 Error_Handler();
//	}  
//	
////	// try it again
////	if(HAL_I2C_GetState(&I2C1_Handle) != HAL_I2C_STATE_READY)
////	{
////		return 2;
////	}
////	if(HAL_I2C_Mem_Write_IT(&I2C1_Handle, (uint16_t)I2C_DEV_ADDR, memAddr, nBytes, buffer, nBytes)== HAL_OK)
////	{
////		I2C1_Status=1;
////		return 0;
////	}

////	// try it third
////	if(HAL_I2C_Mem_Write_IT(&I2C1_Handle, (uint16_t)I2C_DEV_ADDR, memAddr, nBytes, buffer, nBytes)== HAL_OK)
////	{
////		I2C1_Status=1;
////		return 0;
////	}

//	return 2;
//}

//uint8_t I2C1_Read(uint8_t memAddr, uint8_t *buffer, uint8_t nBytes)
//{
//	// check the current state of the I2C1 peripheral before starting a new commnunication transfer
//	if(HAL_I2C_GetState(&I2C1_Handle) != HAL_I2C_STATE_READY)
//	{
//		return 1;
//	}

//	// Put I2C peripheral in reception process
//	while(HAL_I2C_Mem_Read_IT(&I2C1_Handle, (uint16_t)I2C_DEV_ADDR, memAddr, nBytes, buffer, nBytes) != HAL_OK)
//	{
//	}
//	I2C1_Status=1;
//	return nBytes;
//}

//void I2Cx_EV_IRQHandler(void)
//{
//  HAL_I2C_EV_IRQHandler(& I2C1_Handle);
//}

///**
//  * @brief  This function handles I2C error interrupt request.
//  * @param  None
//  * @retval None
//  * @Note   This function is redefined in "main.h" and related to I2C error
//  */
//void I2Cx_ER_IRQHandler(void)
//{
//  HAL_I2C_ER_IRQHandler(& I2C1_Handle);
//}

///* override I2C callback functions using the key __weak -----------------------------*/

//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	// TODO:Transfer in transmission process is correct
//	I2C1_Status=0;
//}

//void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	// TODO:Transfer in reception process is correct
//	I2C1_Status=0;
//}

//void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
//{
//	// TODO:Transfer error in reception/transmission process
//	I2C1_Status=0;
//}


void I2C_Wait2us(void)
{
	// 64M 2.00us
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

	__NOP();__NOP();
}

void I2C_Delay(void)
{
	// 64M 5.00us
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
}

void I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// SCL
	RCC->AHB1ENR |= (I2C1_SCL_CLK);
	GPIO_InitStructure.Pin       = I2C1_SCL_PIN;
	GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_PP;      // external Pull-up
	GPIO_InitStructure.Pull      = GPIO_NOPULL;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = I2C1_AF;
	HAL_GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStructure);

	// SDA
	RCC->AHB1ENR |= (I2C1_SDA_CLK);
	GPIO_InitStructure.Pin       = I2C1_SDA_PIN;
	GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_OD;      // external Pull-up
	GPIO_InitStructure.Pull      = GPIO_NOPULL;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate = I2C1_AF;
	HAL_GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);

	SDA_H;
	SCL_H;
}


static uint8_t I2C_Start(void)                               // start condition high-to-low transition of SDA with SCL high
{
	SDA_H;  I2C_Delay();
	SCL_H;  I2C_Delay();
	SDA_L;  I2C_Delay();
	SCL_L;  I2C_Delay();
	return 0;
}

static void I2C_Stop(void)                                   // stop condition low-to-high transition of SDA with SCL high
{
	SCL_L;  I2C_Delay();
	SDA_L;  I2C_Delay();
	SCL_H;  I2C_Delay();
	SDA_H;  I2C_Delay();
}

//void I2C_Ack(void)
//{	
//	SCL_L;
//	I2C_delay();
//	SDA_L;
//	I2C_delay();
//	SCL_H;
//	I2C_delay();
//	SCL_L;
//	I2C_delay();
//}

// the EEPROM sends a zero to acknowledge during the ninth clock
static uint8_t I2C_WaitAck(void)
{
	uint8_t level=0;
	SDA_H;                                                   // force high of SDA
	SCL_L;  I2C_Delay();
	SCL_H;  I2C_Delay();                                     // SCL low to high of SCL
	level = SDA_Read;                                        // read SDA level waiting for low
	SCL_L;  I2C_Delay();

	return level;
}

static void I2C_TransmitByte(uint8_t SendByte) 
{
	uint8_t i=8;
	while(i--)
	{
		SCL_L; I2C_Delay();
		if(SendByte&0x80)
		{
			SDA_H; I2C_Delay();
		}
		else
		{
			SDA_L; I2C_Delay();
		}
		SCL_H; I2C_Delay();
		SendByte<<=1;
	}
	SCL_L;
}

static uint8_t I2C_ReceiveByte(void)
{ 
	uint8_t i=8;
	uint8_t ReceiveByte=0;

	SDA_H;
	while(i--)
	{
		ReceiveByte<<=1;
		SCL_H;  I2C_Delay();
		SCL_L;  I2C_Delay();
		if(SDA_Read)
		{
			ReceiveByte|=0x01;
		}
		I2C_Delay();
	}
	SCL_L;
	return ReceiveByte;
}

void I2C_Reset(void)
{
	uint8_t	i;
	I2C_Start();
	for(i=9; i!=0; i--)
	{
		SCL_L;
		SDA_H;  I2C_Delay();
		SCL_H;  I2C_Delay();
	}
	I2C_Start();
	I2C_Stop();
}

uint8_t I2C_WriteByte(uint16_t MemAddr,uint8_t SendByte)
{
	// Start
	I2C_Start();

	// device address(7bit) + write flag
	I2C_TransmitByte( (I2C_DEV_ADDR&0xFE)|I2C_DEV_FLG_W);
	if(I2C_WaitAck())
	{
		I2C_Stop();
		return 2;
	}

	// send memory address
	I2C_TransmitByte(MemAddr);
	if(I2C_WaitAck())
	{
		I2C_Stop();
		return 3;
	}

	// send an 8-bit data
	I2C_TransmitByte(SendByte);
	if(I2C_WaitAck())
	{
		I2C_Stop();
		return 4;
	}

	// Stop
	I2C_Stop();

	return 0;
}

uint8_t I2C_ReadByte(uint16_t MemAddr,uint8_t *ReadByte)
{
	// Start
	I2C_Start();

	// device address(7bit) + write flag
	I2C_TransmitByte( (I2C_DEV_ADDR&0xFE)|I2C_DEV_FLG_R);
	if(I2C_WaitAck())
	{
		I2C_Stop();
		return 1;
	}

	// send memory address
	I2C_TransmitByte(MemAddr);
	if(I2C_WaitAck())
	{
		I2C_Stop();
		return 2;
	}

	// Start
	I2C_Start();

	// device address(7bit) + read flag
	I2C_TransmitByte( (I2C_DEV_ADDR&0xFE)|I2C_DEV_FLG_R);
	if(I2C_WaitAck())
	{
		I2C_Stop();
		return 3;
	}

	// send an 8-bit data
	*ReadByte=I2C_ReceiveByte();

	// Stop 
	I2C_Stop();

	return 0;
}
