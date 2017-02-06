#ifndef __DRIVER_STM32F411CE_H
#define __DRIVER_STM32F411CE_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "stm32f4xx_hal.h"

typedef enum 
{
	SW1_1 = 0,
	SW1_2 = 1,
	SW1_3 = 2,
	SW1_4 = 3,
	SW1_5 = 4,
	SW1_6 = 5
} SWI_TypeDef;

typedef enum 
{
	LED2 = 0,
	LED3 = 1
} LED_TypeDef;

/* I2C define ------------------------------------------------------------*/
#define I2C1_SCL_PIN               GPIO_PIN_6             // I2C1-SCL
#define I2C1_SCL_PORT              GPIOB
#define I2C1_SCL_CLK               RCC_AHB1ENR_GPIOBEN
#define I2C1_SDA_PIN               GPIO_PIN_7             // I2C1-SDA
#define I2C1_SDA_PORT              GPIOB
#define I2C1_SDA_CLK               RCC_AHB1ENR_GPIOBEN
#define I2C1_AF                    GPIO_AF4_I2C1
#define I2C1_CLK_ENABLE()          __I2C1_CLK_ENABLE()

/* SWI define ------------------------------------------------------------*/
#define SWIn                       6
#define SWI1_PIN                   GPIO_PIN_13             // SW1-1
#define SWI1_PORT                  GPIOB
#define SWI1_CLK                   RCC_AHB1ENR_GPIOBEN
#define SWI2_PIN                   GPIO_PIN_12             // SW1-2
#define SWI2_PORT                  GPIOB
#define SWI2_CLK                   RCC_AHB1ENR_GPIOBEN
#define SWI3_PIN                   GPIO_PIN_10             // SW1-3
#define SWI3_PORT                  GPIOB
#define SWI3_CLK                   RCC_AHB1ENR_GPIOBEN
#define SWI4_PIN                   GPIO_PIN_1              // SW1-4
#define SWI4_PORT                  GPIOB
#define SWI4_CLK                   RCC_AHB1ENR_GPIOBEN
#define SWI5_PIN                   GPIO_PIN_0              // SW1-5
#define SWI5_PORT                  GPIOB
#define SWI5_CLK                   RCC_AHB1ENR_GPIOBEN
#define SWI6_PIN                   GPIO_PIN_7              // SW1-6
#define SWI6_PORT                  GPIOA
#define SWI6_CLK                   RCC_AHB1ENR_GPIOAEN

/* LED define ------------------------------------------------------------*/
#define LEDn                       2
#define LED1_PIN                   GPIO_PIN_15             // LED-2
#define LED1_PORT                  GPIOB
#define LED1_CLK                   RCC_AHB1ENR_GPIOBEN
#define LED2_PIN                   GPIO_PIN_14             // LED-3
#define LED2_PORT                  GPIOB
#define LED2_CLK                   RCC_AHB1ENR_GPIOBEN

/* WIFI define-------------------------------------------------------------*/
#define WIFI_RESET_PIN             GPIO_PIN_6              // wifi-RESET
#define WIFI_RESET_PORT            GPIOA
#define WIFI_RESET_CLK             RCC_AHB1ENR_GPIOAEN
#define WIFI_READY_PIN             GPIO_PIN_4              // wifi-READY
#define WIFI_READY_PORT            GPIOA
#define WIFI_READY_CLK             RCC_AHB1ENR_GPIOAEN
#define WIFI_LINK_PIN              GPIO_PIN_5              // wifi-LINK
#define WIFI_LINK_PORT             GPIOA
#define WIFI_LINK_CLK              RCC_AHB1ENR_GPIOAEN

/* USART2 define ----------------------------------------------------------*/
#define USART2_RX_PIN              GPIO_PIN_3              // wifi-TX
#define USART2_RX_PORT             GPIOA
#define USART2_RX_CLK              RCC_AHB1ENR_GPIOAEN
#define USART2_RX_AF               GPIO_AF7_USART2
#define USART2_TX_PIN              GPIO_PIN_2              // wifi-RX
#define USART2_TX_PORT             GPIOA
#define USART2_TX_CLK              RCC_AHB1ENR_GPIOAEN
#define USART2_TX_AF               GPIO_AF7_USART2
#define USART2_RTS_PIN             GPIO_PIN_1              // wifi-RTS
#define USART2_RTS_PORT            GPIOA
#define USART2_RTS_CLK             RCC_AHB1ENR_GPIOAEN
#define USART2_RTS_AF              GPIO_AF7_USART2
#define USART2_CTS_PIN             GPIO_PIN_0              // wifi-CTS
#define USART2_CTS_PORT            GPIOA
#define USART2_CTS_CLK             RCC_AHB1ENR_GPIOAEN
#define USART2_CTS_AF              GPIO_AF7_USART2

/* USART6 define ----------------------------------------------------------*/
#define USART6_RX_PIN              GPIO_PIN_12             // USART6-RX
#define USART6_RX_PORT             GPIOA
#define USART6_RX_CLK              RCC_AHB1ENR_GPIOAEN
#define USART6_RX_AF               GPIO_AF8_USART6
#define USART6_TX_PIN              GPIO_PIN_11             // USART6-TX
#define USART6_TX_PORT             GPIOA
#define USART6_TX_CLK              RCC_AHB1ENR_GPIOAEN
#define USART6_TX_AF               GPIO_AF8_USART6
#define RS485_DE_PIN               GPIO_PIN_15             // RS485 chip DE pin
#define RS485_DE_PORT              GPIOA
#define RS485_DE_CLK               RCC_AHB1ENR_GPIOAEN

/* USART1 define ----------------------------------------------------------*/
#define USART1_RX_PIN              GPIO_PIN_10             // H-LINK-RX
#define USART1_RX_PORT             GPIOA
#define USART1_RX_CLK              RCC_AHB1ENR_GPIOAEN
#define USART1_RX_AF               GPIO_AF7_USART1
#define USART1_TX_PIN              GPIO_PIN_9              // H-LINK-TX
#define USART1_TX_PORT             GPIOA
#define USART1_TX_CLK              RCC_AHB1ENR_GPIOAEN
#define USART1_TX_AF               GPIO_AF7_USART1

#define USART1_CK_PIN              GPIO_PIN_8              // H-LINK-TX-CLK
#define USART1_CK_PORT             GPIOA
#define USART1_CK_CLK              RCC_AHB1ENR_GPIOAEN
#define USART1_CK_AF               GPIO_AF1_TIM1

#define USART1_INT_PIN             GPIO_PIN_4              // H-LINK-RX-INT
#define USART1_INT_PORT            GPIOB
#define USART1_INT_CLK             RCC_AHB1ENR_GPIOBEN

#define USART1_RESET_PIN           GPIO_PIN_3              // H-LINK-RESET
#define USART1_RESET_PORT          GPIOB
#define USART1_RESET_CLK           RCC_AHB1ENR_GPIOBEN

/* unused define ----------------------------------------------------------*/
#define LEFT_1_PIN                 GPIO_PIN_5
#define LEFT_1_PORT                GPIOB
#define LEFT_1_CLK                 RCC_AHB1ENR_GPIOBEN
#define LEFT_2_PIN                 GPIO_PIN_8
#define LEFT_2_PORT                GPIOB
#define LEFT_2_CLK                 RCC_AHB1ENR_GPIOBEN
#define LEFT_3_PIN                 GPIO_PIN_9
#define LEFT_3_PORT                GPIOB
#define LEFT_3_CLK                 RCC_AHB1ENR_GPIOBEN

#ifdef __cplusplus
}
#endif

#endif /* __DRIVER_STM32F411CE_H */

