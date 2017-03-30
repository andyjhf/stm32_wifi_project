#include "XTaskHost.h"
#include "XApp.h"

CXTaskHost::CXTaskHost(void)
{
	m_tmSampling = 5;
	m_tmBlink1   = 250;
	m_tmBlink2   = 2000;

	m_PowerCnt    = 0;
	m_samplingCnt = 0;
	m_blinkCnt1   = 0;
	m_blinkCnt2   = 0;

	m_swiBit[0]=m_swiBit[1]=m_swiBit[2]=m_swiBit[3]=m_swiBit[4]=m_swiBit[5] = 0;
	m_readyBit  = 0;
	m_linkBit   = 0;

	m_wifiReady = 0;                               // 1: wifi module ready(started)
	m_wifiLink  = 0;                               // 1: wifi module linked to ayla cloud
	m_ervReady  = 0;                               // 1: erv comm ready
	m_ervLink   = 0;                               // 1: erv outdoor broadcast it's data
}

CXTaskHost::~CXTaskHost(void)
{
}

void CXTaskHost::InitTask(void)
{
	m_tmSampling = 5;
	m_tmBlink1   = 250;
	m_tmBlink2   = 2000;

	m_PowerCnt    = 0;
	m_samplingCnt = 0;
	m_blinkCnt1   = 0;
	m_blinkCnt2   = 0;

	m_swiBit[0]=m_swiBit[1]=m_swiBit[2]=m_swiBit[3]=m_swiBit[4]=m_swiBit[5] = 0;
	m_readyBit  = 0;
	m_linkBit   = 0;

	m_wifiReady = 0;                               // 1: wifi module ready(started)
	m_wifiLink  = 0;                               // 1: wifi module linked to ayla cloud
	m_ervReady  = 0;                               // 1: erv comm ready
	m_ervLink   = 0;                               // 1: erv outdoor broadcast it's data
}

void CXTaskHost::DoLoop(U16 tmOnce)
{
	// sampling
	if(m_samplingCnt>=m_tmSampling)
	{
		m_samplingCnt = 0;                         // clear sampling counter(5ms)
		sampling();                                // take a sample
	}

	// led indicator(blink quickly)
	if(m_blinkCnt1>=m_tmBlink1)
	{
		m_ervReady = g_rs485;//(g_ervinfo[ERVINFO_INDEX_BASE_STATE] & OD_DAT_READY)? 1:0;       // erv ready state
		m_ervLink  = (g_ervinfo[ERVINFO_INDEX_BASE_STATE] & OD_DAT_LINK)? 1:0;       // erv link state
		m_blinkCnt1 = 0;                           // clear quick blinking counter
		if(0xff==g_led2)                           // LED2 is in auto mode
		{
			if(0==m_wifiReady)                     // wifi ready state is not OK
			{
				LED_Toggle(LED2);                  // keep led2 blink by the interval of 250ms
			}
		}
		else                                       // LED2 is in manual mode
		{
			LED_Set(LED2,g_led2);
		}
		if(0xff==g_led3)                           // LED3 is in auto mode
		{
			if(0==m_ervReady)                      // ERV Comm ready state is not OK
			{
				LED_Toggle(LED3);                  // keep led3 blink by the interval of 250ms
			}
		}
		else                                       // LED3 is in manual mode
		{
			LED_Set(LED3,g_led3);
		}
	}

	// led indicator(blink slowly)
	if(m_blinkCnt2>=m_tmBlink2)
	{
		m_ervReady = g_rs485;//(g_ervinfo[ERVINFO_INDEX_BASE_STATE] & OD_DAT_READY)? 1:0;       // erv ready state
		m_ervLink  = (g_ervinfo[ERVINFO_INDEX_BASE_STATE] & OD_DAT_LINK)? 1:0;       // erv link state
		m_blinkCnt2=0;                             // clear slow blinking counter
		if(1==m_wifiReady && 0xff==g_led2)         // wifi ready state is OK and led2 is in auto mode
		{
			if(1==m_wifiLink)                      // wifi has been connectted to ADS
			{
				LED_Set(LED2, LED_ON);             // keep led2 on
			}
			else
			{
				LED_Toggle(LED2);                  // toggle led2(blink)
			}
		}
		if(1==m_ervReady && 0xff==g_led3)
		{
			if(1==m_ervLink)
			{
				LED_Set(LED3, LED_ON);             // keep led3 on
			}
			else
			{
				LED_Toggle(LED3);                  // toggle led3(blink)
			}
		}
//		// TODO: TBD
//		static U16 readyCnt=0;
//		if(0==m_wifiReady)
//		{
//			readyCnt += m_tmBlink2;
//			if(readyCnt>=50000)
//			{
//				readyCnt = 0;
//				m_wifiReset=1;
//			}
//		}
//		else
//		{
//			readyCnt=0;
//		}
	}	

	if(g_wifiReset)                                // reset wifi module flag
	{
		g_wifiReset= 0;
		WIFI_Reset(1);                             // set wifi module reset pin low level
		for (U16 i=0; i<2000; i++);                // keep a short time(167.5us, wifi module reset OK)
		WIFI_Reset(0);                             // return to high level
	}
	else
	{
		WIFI_Reset(0);                             // keep wifi module reset pin high level
	}

	if(0==g_hostReset && ayla_tick()<AYLA_YEAR_MAX)// reset host flag (watch dog)
	{
		IWDG_Feed();
	}
	
	if(1==g_hostReset || ayla_tick()>=AYLA_YEAR_MAX)
	{
		g_hostReset = 0;                           // do it once
		IWDG_InitEx();                             // initialize independent watchdog but not feed it
	}

	if(m_PowerCnt<6000)	                           // 6000ms counter waiting for WiFi's initilization after power on
	{
		m_PowerCnt += tmOnce;
	}
	m_samplingCnt  += tmOnce;                      // 5ms counter for sampling
	m_blinkCnt1    += tmOnce;                      // 250ms counter for slow blinking 
	m_blinkCnt2    += tmOnce;                      // 2000ms counter for quick blinking
}

void CXTaskHost::ParseCommand(U8 *command, U8 size)
{
	if(size>=1)
	{
		switch (command[0])
		{
			case 1:
				g_hostReset=1;                     // 0x01 reset host
				break;
			case 2:
				g_wifiReset=1;                     // 0x02 reset wifi-module
				break;
			default:
				break;
		}
	}
}

void CXTaskHost::sampling(void)
{
	// dip switch(dip switch's No. same as SWI number) 
	m_swiBit[0] <<= 1;
	m_swiBit[1] <<= 1;
	m_swiBit[2] <<= 1;
	m_swiBit[3] <<= 1;
	m_swiBit[4] <<= 1;
	m_swiBit[5] <<= 1;
	m_swiBit[0] |= SWI_GetState(SW1_1);
	m_swiBit[1] |= SWI_GetState(SW1_2);
	m_swiBit[2] |= SWI_GetState(SW1_3);
	m_swiBit[3] |= SWI_GetState(SW1_4);
	m_swiBit[4] |= SWI_GetState(SW1_5);
	m_swiBit[5] |= SWI_GetState(SW1_6);

	// wifi state
	m_readyBit <<= 1;
	m_linkBit  <<= 1;
	m_readyBit |= WIFI_Ready();
	m_linkBit  |= WIFI_Link();

	// update effective swi value
	for(U8 i=0;i<6;i++)
	{
		// high level 4 times continuously
		if(0x0f == (m_swiBit[i]&0x0f))
		{
			g_platform[PLATFORM_INDEX_SW_VAL] |= (1<<(5-i));
		}
		// low level 4 times continuously
		else if(0==(m_swiBit[i]&0x0f))
		{
			g_platform[PLATFORM_INDEX_SW_VAL] &= ~(1<<(5-i));
		}
	}

	// update effective wifi-ready state
	if(1==(m_readyBit&0x0f))                       // wifi module initilization is OK
	{
		m_wifiReady = 1;
		g_platform[PLATFORM_INDEX_WIFI_STATE] |= 0x01;
	}
	else if(0==(m_readyBit&0x0f))                  // wifi module is initilizing
	{
		m_wifiReady = 0;
		g_platform[PLATFORM_INDEX_WIFI_STATE] &= ~(0x01);
	}

	// update effective wifi-link value
	if(1==(m_linkBit&0x0f))                        // wifi module connectted to ADS
	{
		m_wifiLink = 1;
		g_platform[PLATFORM_INDEX_WIFI_STATE] |= 0x02;
	}
	else if(0==(m_linkBit&0x0f))                   // wifi module disconnectted to ADS
	{
		m_wifiLink = 0;
		g_platform[PLATFORM_INDEX_WIFI_STATE] &= ~(0x02);
	}

	// led2(red) status
	if(LED_Get(LED2))
		g_platform[PLATFORM_INDEX_LED_STATE] |= 0x01;                         // led2 on
	else
		g_platform[PLATFORM_INDEX_LED_STATE] &= ~0x01;                        // led2 off

	// led3(green) status
	if(LED_Get(LED3))
		g_platform[PLATFORM_INDEX_LED_STATE] |= 0x02;                         // led3 on
	else
		g_platform[PLATFORM_INDEX_LED_STATE] &= ~0x02;                        // led3 off
}
