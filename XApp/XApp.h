#ifndef __APP_XAPP_H
#define	__APP_XAPP_H

#include "XAppDef.h"

#ifdef __cplusplus
	extern "C" {
#endif
		
// global variables
extern U8 g_wifiReset;                             // 1: request to reset wifi module
extern U8 g_hostReset;                             // 1: request to reset self-host
extern U8 g_led2;                                  // set led2 on/off manual or automatic mode
extern U8 g_led3;                                  // set led3 on/off manual or automatic mode

// global protocol data
extern U8  g_host[HOST_SIZE];                      // Host data
extern U8  g_sys[SYS_SIZE];                        // VRF system data
extern U8  g_od[4][ODU_SIZE];                      // VRF outdoor data
extern U8  g_id[64][IDU_SIZE];                     // VRF indoor data
extern U8  g_remote[RMT_SIZE];                     // VRF remote control data
extern U8  g_command[CMD_SIZE];                    // Host command data

// global protocol prop
extern U8  g_szOemVer[SZ_OEM_VER_SIZE+1];          // oem host template version prop
extern U8  g_szFwVer[SZ_FW_VER_SIZE+1];            // Host firmware version prop 
extern U8  g_szHost[SZ_HOST_SIZE+1];               // Host multi-props combination
extern U8  g_szSystem[SZ_SYS_SIZE+1];              // VRF system multi-props combination
extern U8  g_szOutdoor[4][SZ_ODU_SIZE+1];          // VRF outdoor multi-props combination
extern U8  g_szIndoor[8][SZ_IDU_SIZE+1];           // VRF indoor multi-props combination
extern U8  g_szRemote1[SZ_RMT_SIZE+1];             // VRF remote control command prop
extern U32 g_remote2;                              // 
extern U8  g_szCommand1[SZ_CMD_SIZE+1];            // Host command prop
extern U32 g_command2;                             // 
extern struct sched_prop g_timer[64];              // schedule array props

// application
void InitProp(void);                               // initilize props data
void TransProp(void);                              // hex to string for props
void ParseProp(U8 *command, U8 size);              // parse the WiFi-host command

void set_remote1(struct  prop *prop, void *arg, void *valp, size_t len);
void set_command1(struct prop *prop, void *arg, void *valp, size_t len);
void set_remote2(struct  prop *prop, void *arg, void *valp, size_t len);
void set_command2(struct prop *prop, void *arg, void *valp, size_t len);
void set_sched(struct prop *prop, void *arg, void *valp, size_t len);

void XApp_Init(void);                              // initialize mcu application
void XApp_Run(void);                               // run the mcu application 

#ifdef __cplusplus
}
#endif

#endif /* __APP_XAPP_H */
