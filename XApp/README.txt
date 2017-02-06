
FirstVersion: 2016.01.18
(1).NOTE: KeilV5.12 should intall "Keil.STM32F4xx_DFP.2.3.0.pack" package or the previous versions, 
          then it can create a project with STM32F411CE
(2).demo_img_mgmt.c     remove all functions about flash, the flash layout of M4 is different with M3
(3).spi.c    554 line,  comment "intr_disable_save",  because can't link to this
(4).spi_ping 121 line,  comment "intr_disable_save",  because can't link to this
(5)macro define
STM32F411xE
USE_HAL_DRIVER

AYLA_KEIL                   // demo.c Image header location
__NO_EMBEDDED_ASM
DEMO_IMG_MGMT               // demo.c
AYLA_HOST_OTA               // conf_access.c
DEMO_SCHED_LIB              // demo.c schedeval.c
DEMO_TIME_LIB               // clock_utils.c
AYLA_SCHED                  // conf_access.c sched.c schedeval.c
AYLA_TIME                   // conf_access.c sched.c clock_utils.c
DEMO_CONF                   // demo.c
AYLA_CONF                   // serial_msg.c
DEMO_FILE_PROP              // demo.c
AYLA_FILE_PROP              // props.c serial_msg.c
AYLA_UART                   // mcu_io.c serial_msg.c prop_dp.c host_ota.c
DEMO_UART                   // stm32.c demo.c

Revision:
1)2016.01.19: remove the ayla's ota features
2)2016.01.21: reorganize ayla's source files
3)2016.01.25: the old form of pin is "GPIO_Pin_4"(lower-case),current form is "GPIO_PIN_4"(capital-case)
4)2016.01.28: select the MCU of STM32F411CEUx, 512Kbytes of Flash, 128Kbytes of SRAM
5)2016.02.04: apply readonly attributes to the folder of STM32F4xx_DFP_2.3.0
6)2016.04.05: update startup_stm32f411xe.s Heap_Size, because of C++ new operator

STM32F411xE 
USE_HAL_DRIVER 
AYLA_KEIL 
__NO_EMBEDDED_ASM 
DEMO_SCHED_LIB 
//DEMO_TIME_LIB
//DEMO_CONF
//DEMO_UART

AYLA_SCHED 
AYLA_TIME
AYLA_CONF 
AYLA_UART
AYLA_REG_TOKEN
