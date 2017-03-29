#ifndef __APP_XAPP_H
#define	__APP_XAPP_H

#include "XAppDef.h"

#ifdef __cplusplus
	extern "C" {
#endif
extern const char version[16];	
// global variables
extern U8 g_wifiReset;                             // 1: request to reset wifi module
extern U8 g_hostReset;                             // 1: request to reset self-host
extern U8 g_led2;                                  // set led2 on/off manual or automatic mode
extern U8 g_led3;                                  // set led3 on/off manual or automatic mode
extern U8 g_rs485;

// global protocol data
extern U8  g_platform[PLATFORM_SIZE]; 
extern U8  g_module[MODULE_SIZE];
extern U8  g_ervinfo[ERVINFO_SIZE]; 
extern U8  g_ervcommand[ERVCMD_SIZE];
extern U8  g_modulecommand[MODULECMD_SIZE];
		
// global protocol prop
extern U8  g_szOemVer[SZ_OEM_VER_SIZE+1];          // oem host template version prop
extern U8  g_szPlatform[SZ_PLATFORM_SIZE+1]; 
extern U8  g_szModule[SZ_MODULE_SIZE+1];
extern U8  g_szErvinfo[SZ_ERVINFO_SIZE+1];
extern U8  g_szErvcommand[SZ_ERVCMD_SIZE+1];
extern U8  g_szModulecommand[SZ_MODULECMD_SIZE+1];

extern struct sched_prop g_timer[64];              // schedule array props

// application
void InitProp(void);                               // initilize props data
void TransProp(void);                              // hex to string for props
void ParseProp(U8 *command, U8 size);              // parse the WiFi-host command

void set_sched(struct prop *prop, void *arg, void *valp, size_t len);
int send_version(struct prop *prop, void *arg);
void set_ervcommand(struct  prop *prop, void *arg, void *valp, size_t len);
void set_modulecommand(struct prop *prop, void *arg, void *valp, size_t len);

void XApp_Init(void);                              // initialize mcu application
void XApp_Run(void);                               // run the mcu application 

#ifdef __cplusplus
}
#endif

#endif /* __APP_XAPP_H */
