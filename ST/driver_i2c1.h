#ifndef __DRIVER_I2C1_H
#define __DRIVER_I2C1_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "driver_utility.h"

#define I2C_DEV_ADDR           0xA0                // AT240C2 1-0-1-0-A2-A1-A0-R/W
#define I2C_DEV_MEM_BITS       2048                // 2K bits
#define I2C_DEV_MEM_BYTES      256                 // 256 bytes
#define I2C_DEV_PAGE_SIZE      8                   // 8 bytes per page
#define I2C_DEV_PAGE_NUM       32                  // 32 pages
#define I2C_DEV_FLG_R          0x01                // AT240C2 1-0-1-0-A2-A1-A0-R
#define I2C_DEV_FLG_W          0x00                // AT240C2 1-0-1-0-A2-A1-A0-W

#define SCL_L    I2C1_SCL_PORT->BSRRH = I2C1_SCL_PIN
#define SCL_H    I2C1_SCL_PORT->BSRRL = I2C1_SCL_PIN
#define SDA_L    I2C1_SDA_PORT->BSRRH = I2C1_SDA_PIN
#define SDA_H    I2C1_SDA_PORT->BSRRL = I2C1_SDA_PIN
#define SCL_Read ((I2C1_SCL_PORT->IDR&I2C1_SCL_PIN)? 1: 0)
#define SDA_Read ((I2C1_SDA_PORT->IDR&I2C1_SDA_PIN)? 1: 0)

//void    I2C1_Init(void);
//uint8_t I2C1_GetState(void);
//uint8_t I2C1_Write(uint8_t memAddr, uint8_t *buffer, uint8_t nBytes);
//uint8_t I2C1_Read( uint8_t memAddr, uint8_t *buffer, uint8_t nBytes);

void    I2C_Init(void);
uint8_t I2C_WriteByte(uint16_t MemAddr,uint8_t SendByte);
uint8_t I2C_ReadByte(uint16_t MemAddr, uint8_t *ReadByte);
void    I2C_Reset(void);

#ifdef __cplusplus
}
#endif

#endif /*__DRIVER_I2C1_H */
