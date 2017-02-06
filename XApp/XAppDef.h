#ifndef __APP_XAPPDEF_H
#define	__APP_XAPPDEF_H

#include "driver_gpio.h"
#include "driver_rcc.h"
#include "driver_uart1.h"
#include "driver_uart2.h"
#include "driver_uart6.h"
#include "driver_iwdg.h"
#include "driver_i2c1.h"

#include "ayla_conf_access.h"
#include "ayla_clock.h"
#include "ayla_props.h"
#include "ayla_sched.h"
#include "ayla_schedeval.h"
#include "ayla_serial.h"
#include "ayla_uart.h"

#include "MUtility.h"
#include "XTaskHost.h"
#include "XTaskComVRF.h"
#include "XTaskComWifi.h"

#define OD_DAT_LINK       0x10                     // vrf outdoor link state
#define OD_DAT_READY      0x20                     // vrf outdoor ready state

#define HOST_DAT_SW1_ON   0x20                     // sw1-1
#define HOST_DAT_SW2_ON   0x10                     // sw1-2
#define HOST_DAT_SW3_ON   0x08                     // sw1-3
#define HOST_DAT_SW4_ON   0x04                     // sw1-4
#define HOST_DAT_SW5_ON   0x02                     // sw1-5
#define HOST_DAT_SW6_ON   0x01                     // sw1-6

//#define  OEM_HOST_VER     "YES_V1.04"            // oem host template version
//#define  FW_VER           "V0.05_20160517"       // host software version

#define  OEM_HOST_VER     "YES_V1.00"              // oem host template version
#define  FW_VER           "V1.00_20160822"         // host software version

#define  HOST_SIZE        10                       // host data buffer size
#define  SYS_SIZE         15                       // vrf system data buffer size
#define  ODU_SIZE         120                      // vrf outdoor data  buffer size
#define  IDU_SIZE         15                       // vrf indoor data buffer size
#define  RMT_SIZE         25                       // vrf remote control data buffer size
#define  CMD_SIZE         25                       // host command control data buffer size

#define  SZ_OEM_VER_SIZE  50                       // oem host version prop buffer size
#define  SZ_FW_VER_SIZE   30                       // host software verison prop buffer size

#define  SZ_HOST_SIZE     (HOST_SIZE*2)            // host prop buffer size
#define  SZ_SYS_SIZE      (SYS_SIZE*2)             // vrf system prop buffer size
#define  SZ_ODU_SIZE      (ODU_SIZE*2)             // vrf outdoor prop buffer size
#define  SZ_IDU_SIZE      (IDU_SIZE*2*8)           // vrf indoor prop buffer size
#define  SZ_RMT_SIZE      (RMT_SIZE*2)             // vrf remote control prop buffer size
#define  SZ_CMD_SIZE      (CMD_SIZE*2)             // host command control prop buffer size

#endif /* __APP_XAPPDEF_H */
