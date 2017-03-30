#ifndef __APP_XAPPDEF_H
#define	__APP_XAPPDEF_H

#include "driver_gpio.h"
#include "driver_rcc.h"
#include "driver_uart1.h"
#include "driver_uart2.h"
#include "driver_uart6.h"
#include "driver_iwdg.h"
#include "driver_i2c1.h"
#include "driver_rtc.h"

#include "ayla_conf_access.h"
#include "ayla_clock.h"
#include "ayla_props.h"
#include "ayla_sched.h"
#include "ayla_schedeval.h"
#include "ayla_serial.h"
#include "ayla_uart.h"

#include "MUtility.h"
#include "XTaskHost.h"
#include "XTaskComERV.h"
#include "XTaskComWifi.h"
#include "XTaskSensorData.h"

#define  SZ_OEM_VER_SIZE  50                       // oem host version prop buffer size

#define OD_DAT_LINK       				0x01                     // ERV link state
//#define OD_DAT_READY      				0x80                     // ERV ready state

#define POWER_STATE								0x04
#define SLEEP_STATE								0x20
#define FAN_STATE									0x0e
#define FAULT_STATE								0x08
#define HEATING_STATE							0x02
#define CLEAN_STATE								0x20

#define FAN_AVAILABLE_STATE				0x01
#define SLEEP_AVAILABLE_STATE			0x10
#define HEATING_AVAILABLE_STATE		0x01
#define CLEAN_AVAILABLE_STATE			0x10
#define TEMP_AVAILABLE_STATE			0x01
#define HUMI_AVAILABLE_STATE			0x02
#define PM2_5_AVAILABLE_STATE			0x04
#define CO2_AVAILABLE_STATE				0x08
#define HCHO_AVAILABLE_STATE			0x10
#define TVOC_AVAILABLE_STATE			0x20

#define ERVINFO_INDEX_GROUP_NUM									0
#define ERVINFO_INDEX_BASE_STATE								1
#define ERVINFO_INDEX_FAULT_HI									2
#define ERVINFO_INDEX_FAULT_LO									3
#define ERVINFO_INDEX_FAN_SLEEP									4
#define ERVINFO_INDEX_HEAT_CLEAN								5
#define ERVINFO_INDEX_SENSOR_AVAILABLE					6
#define ERVINFO_INDEX_TEMP_VAL									7
#define ERVINFO_INDEX_HUMI_VAL									8
#define ERVINFO_INDEX_PM2_5_VAL_HI							9
#define ERVINFO_INDEX_PM2_5_VAL_LO							10
#define ERVINFO_INDEX_CO2_VAL_HI								11
#define ERVINFO_INDEX_CO2_VAL_LO								12
#define ERVINFO_INDEX_HCHO_VAL									13
#define ERVINFO_INDEX_TVOC_VAL									14
#define ERVINFO_SIZE														15

#define PLATFORM_INDEX_WIFI_STATE								0
#define PLATFORM_INDEX_SW_VAL										1
#define PLATFORM_INDEX_LED_STATE								2
#define PLATFORM_SIZE        										3

#define MODULE_INDEX_WIFI_MODULE_VER_HI					0
#define MODULE_INDEX_WIFI_MODULE_VER_LO					1
#define MODULE_INDEX_ERV_NUM_AND_STATE					2
#define MODULE_INDEX_SYSTEM_FAULT_HI						3
#define MODULE_INDEX_SYSTEM_FAULT_LO						4
#define MODULE_SIZE															5

#define ERVCMD_INDEX_GROUP_NUM									0
#define ERVCMD_INDEX_AVAILABLE_STATE						1
#define ERVCMD_INDEX_CMD_DATA										2
#define ERVCMD_SIZE															3

#define MODULECMD_INDEX_CTRL_TYPE								0
#define MODULECMD_INDEX_CTRL_CMD								1
#define MODULECMD_INDEX_CTRL_DATA								2
#define MODULECMD_SIZE													3

#define  SZ_PLATFORM_SIZE						(PLATFORM_SIZE*2)
#define  SZ_MODULE_SIZE							(MODULE_SIZE*2)
#define  SZ_ERVINFO_SIZE						(ERVINFO_SIZE*2)
#define  SZ_ERVCMD_SIZE							(ERVCMD_SIZE*2)
#define  SZ_MODULECMD_SIZE					(MODULECMD_SIZE*2)

#define ERV_CMD_LOCK_AVAILABLE_MASK				0x01
#define ERV_CMD_POWER_AVAILABLE_MASK			0x02
#define ERV_CMD_SLEEP_AVAILABLE_MASK			0x04
#define ERV_CMD_FAN_AVAILABLE_MASK				0x08

#define ERV_CMD_LOCK_DATA_MASK						0x01
#define ERV_CMD_POWER_DATA_MASK						0x02
#define ERV_CMD_SLEEP_DATA_MASK						0x04
#define ERV_CMD_FAN_DATA_MASK							0x70

#define HOST_DAT_SW1_ON   0x20                     // sw1-1
#define HOST_DAT_SW2_ON   0x10                     // sw1-2
#define HOST_DAT_SW3_ON   0x08                     // sw1-3
#define HOST_DAT_SW4_ON   0x04                     // sw1-4
#define HOST_DAT_SW5_ON   0x02                     // sw1-5
#define HOST_DAT_SW6_ON   0x01                     // sw1-6

//#define  OEM_HOST_VER     "YES_V1.04"            // oem host template version
//#define  FW_VER           "V0.05_20160517"       // host software version

#define  OEM_HOST_VER     "ERV 0.1"              // oem host template version
//#define  FW_VER           "V1.00_20160822"         // host software version


#endif /* __APP_XAPPDEF_H */
