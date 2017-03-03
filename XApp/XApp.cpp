#include "XApp.h"

// global variables
U8 g_wifiReset;                                    // 1: request to reset wifi module
U8 g_hostReset;                                    // 1: request to reset self-host
U8 g_led2 = 0xff;                                  // set led2 on/off manual or automatic mode(0xff)
U8 g_led3 = 0xff;                                  // set led3 on/off manual or automatic mode(0xff)
                              
// global protocol data
U8  g_platform[PLATFORM_SIZE]; 
U8  g_module[MODULE_SIZE];
U8  g_ervinfo[ERVINFO_SIZE]; 
U8  g_ervcommand[ERVCMD_SIZE];
U8  g_modulecommand[MODULECMD_SIZE];

// global protocol prop
U8  g_szOemVer[SZ_OEM_VER_SIZE+1];                 // oem Host template version prop
U8  g_szPlatform[SZ_PLATFORM_SIZE+1]; 
U8  g_szModule[SZ_MODULE_SIZE+1];
U8  g_szErvinfo[SZ_ERVINFO_SIZE+1];
U8  g_szErvcommand[SZ_ERVCMD_SIZE+1];
U8  g_szModulecommand[SZ_MODULECMD_SIZE+1];

struct sched_prop g_timer[64];                     // schedule array props

// global task object
CXTaskComVRF  *taskVRF  = NULL;                    // ERV communication task
CXTaskHost    *taskHost = NULL;                    // Host processing task
CXTaskComWifi *taskWifi = NULL;                    // Module communication task

// index of prop table
#define PI_OEM_HOST_VERSION       0                             // WIFI prop index of table
#define PI_PLATFORM       				1                             // WIFI prop index of table
#define PI_MODULE      						2                             // Host prop index of table
#define PI_ERVINFO       					3                             // ERV sytem prop index of table
#define PI_ERVCOMMAND    					4                             // ERV control prop index of table
#define PI_MODULECOMMAND  				5                             // wifi control prop index of table
#define MAX_PROP_NUMBER						6

// global prop table
struct prop prop_table[] = {
	{ AYLA_VER_NAME, ATLV_UTF8, NULL,    prop_send_generic, &g_szOemVer[0],      sizeof(g_szOemVer)-1,    AFMT_READ_ONLY},
	
	{ "W_HW01", ATLV_UTF8, NULL,        prop_send_generic, &g_szPlatform[0],       sizeof(g_szPlatform)-1,    AFMT_READ_ONLY},
	{ "W_INFO01", ATLV_UTF8, NULL,        prop_send_generic, &g_szModule[0],       sizeof(g_szModule)-1,    AFMT_READ_ONLY},
	{ "Y_ERV01", ATLV_UTF8, NULL,        prop_send_generic, &g_szErvinfo[0],       sizeof(g_szErvinfo)-1,    AFMT_READ_ONLY},
	{ "Y_RMT01", ATLV_UTF8, set_ervcommand, prop_send_generic, &g_szErvcommand[0],   sizeof(g_szErvcommand)-1, },
	{ "W_RMT01", ATLV_UTF8, set_modulecommand,  prop_send_generic, &g_szModulecommand[0],    sizeof(g_szModulecommand)-1, },
	
	{ "SCHE_01", ATLV_SCHED, set_sched, NULL, &g_timer[0]},
	{ "SCHE_02", ATLV_SCHED, set_sched, NULL, &g_timer[1]},
	{ "SCHE_03", ATLV_SCHED, set_sched, NULL, &g_timer[2]},
	{ "SCHE_04", ATLV_SCHED, set_sched, NULL, &g_timer[3]},
	{ "SCHE_05", ATLV_SCHED, set_sched, NULL, &g_timer[4]},
	{ "SCHE_06", ATLV_SCHED, set_sched, NULL, &g_timer[5]},
	{ "SCHE_07", ATLV_SCHED, set_sched, NULL, &g_timer[6]},
	{ "SCHE_08", ATLV_SCHED, set_sched, NULL, &g_timer[7]},
	{ "SCHE_09", ATLV_SCHED, set_sched, NULL, &g_timer[8]},
	{ "SCHE_10", ATLV_SCHED, set_sched, NULL, &g_timer[9]},
	{ "SCHE_11", ATLV_SCHED, set_sched, NULL, &g_timer[10]},
	{ "SCHE_12", ATLV_SCHED, set_sched, NULL, &g_timer[11]},
	{ "SCHE_13", ATLV_SCHED, set_sched, NULL, &g_timer[12]},
	{ "SCHE_14", ATLV_SCHED, set_sched, NULL, &g_timer[13]},
	{ "SCHE_15", ATLV_SCHED, set_sched, NULL, &g_timer[14]},
	{ "SCHE_16", ATLV_SCHED, set_sched, NULL, &g_timer[15]},

	{ "SCHE_17", ATLV_SCHED, set_sched, NULL, &g_timer[0+16]},
	{ "SCHE_18", ATLV_SCHED, set_sched, NULL, &g_timer[1+16]},
	{ "SCHE_19", ATLV_SCHED, set_sched, NULL, &g_timer[2+16]},
	{ "SCHE_20", ATLV_SCHED, set_sched, NULL, &g_timer[3+16]},
	{ "SCHE_21", ATLV_SCHED, set_sched, NULL, &g_timer[4+16]},
	{ "SCHE_22", ATLV_SCHED, set_sched, NULL, &g_timer[5+16]},
	{ "SCHE_23", ATLV_SCHED, set_sched, NULL, &g_timer[6+16]},
	{ "SCHE_24", ATLV_SCHED, set_sched, NULL, &g_timer[7+16]},
	{ "SCHE_25", ATLV_SCHED, set_sched, NULL, &g_timer[8+16]},
	{ "SCHE_26", ATLV_SCHED, set_sched, NULL, &g_timer[9+16]},
	{ "SCHE_27", ATLV_SCHED, set_sched, NULL, &g_timer[10+16]},
	{ "SCHE_28", ATLV_SCHED, set_sched, NULL, &g_timer[11+16]},
	{ "SCHE_29", ATLV_SCHED, set_sched, NULL, &g_timer[12+16]},
	{ "SCHE_30", ATLV_SCHED, set_sched, NULL, &g_timer[13+16]},
	{ "SCHE_31", ATLV_SCHED, set_sched, NULL, &g_timer[14+16]},
	{ "SCHE_32", ATLV_SCHED, set_sched, NULL, &g_timer[15+16]},

	{ "SCHE_33", ATLV_SCHED, set_sched, NULL, &g_timer[0+32]},
	{ "SCHE_34", ATLV_SCHED, set_sched, NULL, &g_timer[1+32]},
	{ "SCHE_35", ATLV_SCHED, set_sched, NULL, &g_timer[2+32]},
	{ "SCHE_36", ATLV_SCHED, set_sched, NULL, &g_timer[3+32]},
	{ "SCHE_37", ATLV_SCHED, set_sched, NULL, &g_timer[4+32]},
	{ "SCHE_38", ATLV_SCHED, set_sched, NULL, &g_timer[5+32]},
	{ "SCHE_39", ATLV_SCHED, set_sched, NULL, &g_timer[6+32]},
	{ "SCHE_40", ATLV_SCHED, set_sched, NULL, &g_timer[7+32]},
	{ "SCHE_41", ATLV_SCHED, set_sched, NULL, &g_timer[8+32]},
	{ "SCHE_42", ATLV_SCHED, set_sched, NULL, &g_timer[9+32]},
	{ "SCHE_43", ATLV_SCHED, set_sched, NULL, &g_timer[10+32]},
	{ "SCHE_44", ATLV_SCHED, set_sched, NULL, &g_timer[11+32]},
	{ "SCHE_45", ATLV_SCHED, set_sched, NULL, &g_timer[12+32]},
	{ "SCHE_46", ATLV_SCHED, set_sched, NULL, &g_timer[13+32]},
	{ "SCHE_47", ATLV_SCHED, set_sched, NULL, &g_timer[14+32]},
	{ "SCHE_48", ATLV_SCHED, set_sched, NULL, &g_timer[15+32]},

	{ "SCHE_49", ATLV_SCHED, set_sched, NULL, &g_timer[0+48]},
	{ "SCHE_50", ATLV_SCHED, set_sched, NULL, &g_timer[1+48]},
	{ "SCHE_51", ATLV_SCHED, set_sched, NULL, &g_timer[2+48]},
	{ "SCHE_52", ATLV_SCHED, set_sched, NULL, &g_timer[3+48]},
	{ "SCHE_53", ATLV_SCHED, set_sched, NULL, &g_timer[4+48]},
	{ "SCHE_54", ATLV_SCHED, set_sched, NULL, &g_timer[5+48]},
	{ "SCHE_55", ATLV_SCHED, set_sched, NULL, &g_timer[6+48]},
	{ "SCHE_56", ATLV_SCHED, set_sched, NULL, &g_timer[7+48]},
	{ "SCHE_57", ATLV_SCHED, set_sched, NULL, &g_timer[8+48]},
	{ "SCHE_58", ATLV_SCHED, set_sched, NULL, &g_timer[9+48]},
	{ "SCHE_59", ATLV_SCHED, set_sched, NULL, &g_timer[10+48]},
	{ "SCHE_60", ATLV_SCHED, set_sched, NULL, &g_timer[11+48]},
	{ "SCHE_61", ATLV_SCHED, set_sched, NULL, &g_timer[12+48]},
	{ "SCHE_62", ATLV_SCHED, set_sched, NULL, &g_timer[13+48]},
	{ "SCHE_63", ATLV_SCHED, set_sched, NULL, &g_timer[14+48]},
	{ "SCHE_64", ATLV_SCHED, set_sched, NULL, &g_timer[15+48]},

	// keep the last prop null
	{ NULL }
};

// global prop count
uint8_t prop_count = (sizeof(prop_table) / sizeof(prop_table[0])) - 1;

void InitProp(void)
{
	memset(g_szOemVer,         '\0', sizeof(g_szOemVer));
	memset(g_szPlatform,          '\0', sizeof(g_szPlatform));
	memset(g_szModule,           '\0', sizeof(g_szModule));
	memset(g_szErvinfo,          '\0', sizeof(g_szErvinfo));
	memset(g_szErvcommand,           '\0', sizeof(g_szErvcommand));
	memset(g_szModulecommand,          '\0', sizeof(g_szModulecommand));

	memset(g_timer, 0, sizeof(g_timer));

	memcpy(g_szOemVer,     OEM_HOST_VER, strlen(OEM_HOST_VER)); 
	memcpy(g_timer[0].name,  "SCHE_01", 7);
	memcpy(g_timer[1].name,  "SCHE_02", 7);
	memcpy(g_timer[2].name,  "SCHE_03", 7);
	memcpy(g_timer[3].name,  "SCHE_04", 7);
	memcpy(g_timer[4].name,  "SCHE_05", 7);
	memcpy(g_timer[5].name,  "SCHE_06", 7);
	memcpy(g_timer[6].name,  "SCHE_07", 7);
	memcpy(g_timer[7].name,  "SCHE_08", 7);
	memcpy(g_timer[8].name,  "SCHE_09", 7);
	memcpy(g_timer[9].name,  "SCHE_10", 7);
	memcpy(g_timer[10].name, "SCHE_11", 7);
	memcpy(g_timer[11].name, "SCHE_12", 7);
	memcpy(g_timer[12].name, "SCHE_13", 7);
	memcpy(g_timer[13].name, "SCHE_14", 7);
	memcpy(g_timer[14].name, "SCHE_15", 7);
	memcpy(g_timer[15].name, "SCHE_16", 7);

	memcpy(g_timer[0+16].name,  "SCHE_17", 7);
	memcpy(g_timer[1+16].name,  "SCHE_18", 7);
	memcpy(g_timer[2+16].name,  "SCHE_19", 7);
	memcpy(g_timer[3+16].name,  "SCHE_20", 7);
	memcpy(g_timer[4+16].name,  "SCHE_21", 7);
	memcpy(g_timer[5+16].name,  "SCHE_22", 7);
	memcpy(g_timer[6+16].name,  "SCHE_23", 7);
	memcpy(g_timer[7+16].name,  "SCHE_24", 7);
	memcpy(g_timer[8+16].name,  "SCHE_25", 7);
	memcpy(g_timer[9+16].name,  "SCHE_26", 7);
	memcpy(g_timer[10+16].name, "SCHE_27", 7);
	memcpy(g_timer[11+16].name, "SCHE_28", 7);
	memcpy(g_timer[12+16].name, "SCHE_29", 7);
	memcpy(g_timer[13+16].name, "SCHE_30", 7);
	memcpy(g_timer[14+16].name, "SCHE_31", 7);
	memcpy(g_timer[15+16].name, "SCHE_32", 7);

	memcpy(g_timer[0+32].name,  "SCHE_33", 7);
	memcpy(g_timer[1+32].name,  "SCHE_34", 7);
	memcpy(g_timer[2+32].name,  "SCHE_35", 7);
	memcpy(g_timer[3+32].name,  "SCHE_36", 7);
	memcpy(g_timer[4+32].name,  "SCHE_37", 7);
	memcpy(g_timer[5+32].name,  "SCHE_38", 7);
	memcpy(g_timer[6+32].name,  "SCHE_39", 7);
	memcpy(g_timer[7+32].name,  "SCHE_40", 7);
	memcpy(g_timer[8+32].name,  "SCHE_41", 7);
	memcpy(g_timer[9+32].name,  "SCHE_42", 7);
	memcpy(g_timer[10+32].name, "SCHE_43", 7);
	memcpy(g_timer[11+32].name, "SCHE_44", 7);
	memcpy(g_timer[12+32].name, "SCHE_45", 7);
	memcpy(g_timer[13+32].name, "SCHE_46", 7);
	memcpy(g_timer[14+32].name, "SCHE_47", 7);
	memcpy(g_timer[15+32].name, "SCHE_48", 7);
	
	memcpy(g_timer[0+48].name,  "SCHE_49", 7);
	memcpy(g_timer[1+48].name,  "SCHE_50", 7);
	memcpy(g_timer[2+48].name,  "SCHE_51", 7);
	memcpy(g_timer[3+48].name,  "SCHE_52", 7);
	memcpy(g_timer[4+48].name,  "SCHE_53", 7);
	memcpy(g_timer[5+48].name,  "SCHE_54", 7);
	memcpy(g_timer[6+48].name,  "SCHE_55", 7);
	memcpy(g_timer[7+48].name,  "SCHE_56", 7);
	memcpy(g_timer[8+48].name,  "SCHE_57", 7);
	memcpy(g_timer[9+48].name,  "SCHE_58", 7);
	memcpy(g_timer[10+48].name, "SCHE_59", 7);
	memcpy(g_timer[11+48].name, "SCHE_60", 7);
	memcpy(g_timer[12+48].name, "SCHE_61", 7);
	memcpy(g_timer[13+48].name, "SCHE_62", 7);
	memcpy(g_timer[14+48].name, "SCHE_63", 7);
	memcpy(g_timer[15+48].name, "SCHE_64", 7);

}

void TransProp(void)
{
	static U8 tran=0;
	U8 size;                                       // size of the source unit data

	switch(tran)
	{
	case 0:                                        // host prop
		size  = sizeof(g_platform);                    // size of host unit data
		Hex2Asc(&g_platform[0], size, &g_szPlatform[0]);   // encode BCD
		g_szPlatform[size*2] = 0x00;                   // add '\0' to the tail of string
		break;
	case 1:                                        // VRF system prop
		size  = sizeof(g_module);                     // size of VRF system data
		Hex2Asc(&g_module[0], size, &g_szModule[0]);  // encode BCD to VRF system prop
		g_szModule[size*2] = 0x00;                 // add '\0' to the tail of string
		break;
	case 2:                                        // 1# outdoor prop
		size  = sizeof(g_ervinfo);                     // size of VRF system data
		Hex2Asc(&g_ervinfo[0], size, &g_szErvinfo[0]);  // encode BCD to VRF system prop
		g_szErvinfo[size*2] = 0x00;                 // add '\0' to the tail of string
		break;
	case 3:                                        // 2# outdoor prop
//		size  = sizeof(g_ervcommand);                     // size of VRF system data
//		Hex2Asc(&g_ervcommand[0], size, &g_szErvcommand[0]);  // encode BCD to VRF system prop
//		g_szErvcommand[size*2] = 0x00;                 // add '\0' to the tail of string
		break;
	case 4:                                        // 3# outdoor prop
//		size  = sizeof(g_modulecommand);                     // size of VRF system data
//		Hex2Asc(&g_modulecommand[0], size, &g_szModulecommand[0]);  // encode BCD to VRF system prop
//		g_szModulecommand[size*2] = 0x00;                 // add '\0' to the tail of string
		break;
	default:                                       // indoor list prop from 1 to 8
		break;
	}
	tran++;                                        // next prop to be converted
	tran %= MAX_PROP_NUMBER;                                    // turn to head

	// TODO:DEBUG
//	static U32 SendCnt=0;
//	SendCnt++;
//	if(1000*30==SendCnt)
//	{
//		SendCnt=0;
//			for(U8 i=1; i<=PI_IDU_08; i++)
//			{
//				//if(prop_table[i].name && prop_table[i].name_len>0)
//				if(prop_table[i].name)
//				{
//					prop_table[i].send_mask = valid_dest_mask;
//					prop_table[i].echo = 1;
//				}
//			}
//	}
}

void ParseProp(U8 *command, U8 size)
{
	if(0x01==command[0] && size>=2)                // send specfied props to ayla cloud once
	{
		if(1==command[1] || 3==command[1] || 4==command[1])                          // send ERV units props once
		{
			// ERV unit prop
			prop_table[PI_ERVINFO].send_mask = valid_dest_mask;
			prop_table[PI_ERVINFO].echo = 1;
		}
		else if(2==command[1])                     // send system unit prop once
		{
			prop_table[PI_MODULE].send_mask = valid_dest_mask;
			prop_table[PI_MODULE].echo = 1;
		}
		else if(0xff==command[1])                  // send host/system/all indoors props once
		{
			prop_table[PI_ERVINFO].send_mask  = valid_dest_mask;
			prop_table[PI_ERVINFO].echo = 1;
			prop_table[PI_MODULE].send_mask = valid_dest_mask;
			prop_table[PI_MODULE].echo = 1;
			prop_table[PI_PLATFORM].send_mask  = valid_dest_mask;
			prop_table[PI_PLATFORM].echo = 1;
		}
	}
}

void set_ervcommand(struct  prop *prop, void *arg, void *valp, size_t len)
{
	// arg:  prop->arg
	// valp: tlv value point
	// len:  tlv value len
	
	// do nothing in the first 30 seconds after wifi module connectted to ads
	U32 timeSpan = clock_utc() - clock_boot_utc_time;
	if(timeSpan<30)
	{
		return;
	}
	
	if(len<1)
		return;

	if(len > sizeof(g_szErvcommand)-1)                // check string length maximum
		len = sizeof(g_szErvcommand)-1;
	memcpy(&g_szErvcommand[0],valp,len);              // copy receiver buffer to prop destination
	g_szErvcommand[len] = '\0';                       // don't forget the last '\0'

	if(len> sizeof(g_ervcommand)*2)                    // check matching half length of the string with the data size 
		len = sizeof(g_ervcommand)*2;

	len = Asc2Hex(g_szErvcommand, len, g_ervcommand);     // convert string to hexadecimal
	taskVRF->ParseRemote(g_ervcommand,len);            // call the routine that parse remote control of vrf task
}
void set_modulecommand(struct prop *prop, void *arg, void *valp, size_t len)
{
	// arg:  prop->arg
	// valp: tlv value point
	// len:  tlv value len
	
	// do nothing in the first 30 seconds after wifi module connectted to ads
	U32 timeSpan = clock_utc() - clock_boot_utc_time;
	if(timeSpan<30)
	{
		return;
	}
	
	if(len<1)                                      // len must be large than 1
		return;

	if(len > sizeof(g_szModulecommand)-1)               // check string length maximum
		len = sizeof(g_szModulecommand)-1;
	memcpy(&g_szModulecommand[0],valp,len);             // copy receiver buffer to prop destination
	g_szModulecommand[len] = '\0';                      // don't forget the last '\0'

	if(len> sizeof(g_modulecommand)*2)                   // check matching half length of the string with the data size
		len = sizeof(g_modulecommand)*2;
	len=Asc2Hex(g_szModulecommand, len, g_modulecommand);     // convert string to hexadecimal and check bcc
	
	if(0x01==g_modulecommand[0])	                       // host cmd(reset host/reset wifi module)
	{
		taskHost->ParseCommand((U8*)&g_modulecommand[1], len-1);
	}
	else if(0x02==g_modulecommand[0])	                   // prop cmd, send props(all/specified) to ayla cloud once
	{
		ParseProp((U8*)&g_modulecommand[1], len-1);
	}
	else if(0x03==g_modulecommand[0])
	{
		taskWifi->ParseConf((U8*)&g_modulecommand[1], len-1);
	}
}
void set_sched(struct prop *prop, void *arg, void *valp, size_t len)
{
	if(len>SCHED_TLV_LEN)                          // check sched_prop length maximum
		len=SCHED_TLV_LEN;
	struct sched_prop *timer = (struct sched_prop*)prop->arg;
	timer->len = len;                              // sched_prop length
	memcpy(timer->tlvs, valp, len);                // sched_prop tlvs

	sched_run_all(NULL);                           // call it to re-evaluate next action time of all schedules
}

void XApp_Init(void)
{
	// initialize global variables
	g_wifiReset = 1;                               // 1: request to reset wifi module
	g_hostReset = 0;                               // 1: request to reset self-host
	g_led2 = 0xff;	                               // led2 automatic mode
	g_led3 = 0xff;	                               // led3 automatic mode

	// initialize protocol data
	memset(g_platform,    sizeof(g_platform),   0);        // Host data
	memset(g_module,     sizeof(g_module),    0);        // VRF system data
	memset(g_ervinfo,      sizeof(g_ervinfo),     0);        // VRF outdoor data
	memset(g_ervcommand,      sizeof(g_ervcommand),     0);        // VRF indoor data
	memset(g_modulecommand,  sizeof(g_modulecommand), 0);        // VRF remote control data
	
	// initialize protocol prop
	InitProp();

	// construct all tasks
	taskVRF  = new CXTaskComVRF;                   // vrf task for usart communication with outdoor and indoor units
	taskHost = new CXTaskHost;                     // host task for sampling dip setiing, and led blink/on/off
	taskWifi = new CXTaskComWifi;                  // wifi task for communicating with wifi module

	// initialize all tasks
	taskVRF->InitTask();                           // initialize vrf task
	taskHost->InitTask();                          // initialize host task
	taskWifi->InitTask();                          // initialize wifi task
}

void XApp_Run(void)
{
	static U32 curTick=0;                          // current tick time(ms)
	static U32 taskTick[3]={0,0,0};                // job rotation tick time(ms)
	static U8  slot=0;                             // job rotation No
#if DEBUG_LOG == 1
	static U32 debug_tick = 0;
	static U32 debug_cnt=0;
#endif
	for (;;)                                       // main loop
	{
		curTick=HAL_GetTick();                     // current tick time
		if(curTick != taskTick[slot])              // check current task's timetick
		{
			switch(slot)                           // which job No
			{
				case 0:                            // job 1
					taskVRF->DoLoop(1);            // do task of VRF communication once 1 millisecond
					break;
				case 1:                            // job 2
					taskHost->DoLoop(1);           // do task of MCU host once 1 millisecond
					break;
				case 2:                            // job 3
					TransProp();                   // transfer one data to a prop by sequence
					break;
			}
			taskTick[slot] = curTick;              // save job's tick time
		}
		slot++;                                    // next job
		slot %= 3;                                 // turn to head job

		if(taskHost->m_PowerCnt>=5000)             // after 5000ms 
		{
			taskWifi->DoReal();                    // job4 usart communication with wifi module in realtime
		}
		else                                       // in 5000ms after MCU boot(scope:required 1800ms)
		{
			taskWifi->DetectFactory();             // detect factory reset 
		}
#if DEBUG_LOG == 1
		if(curTick != debug_tick)
		{	
			debug_cnt++;
			debug_tick = curTick;
		}
		if(debug_cnt > 10000)
		{
			debug_cnt = 0;
			taskVRF->debug();
		}
#endif
	}
}
