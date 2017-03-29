#include "XTaskSensorData.h"
#include "XApp.h"

// weak functions
__weak void RTC_Init(void) {}
__weak int is_RTC_RESET(void) {return 0;}
__weak void set_RTC(RTC_DATE_TIME *time) {}
__weak void get_RTC(RTC_DATE_TIME *time) {}
	
static RTC_DATE_TIME now_time;

CXTaskSensorData::CXTaskSensorData(void)
{
	m_enable = 0;
	m_curHours = 0;
	m_preHours = 0;
	m_sysTick = 0;
	m_startTick = 0;
	m_tmSampling = 500;																			
	m_hourState = 0;
	m_dayState = 0;
	m_updateRTC = 0;
	m_preTime = 0;
	m_lastTime = 0;
}

CXTaskSensorData::~CXTaskSensorData(void)
{
}

void CXTaskSensorData::InitTask(void)
{
	m_enable = 0;
	m_curHours = 0;
	m_preHours = 0;
	m_sysTick = 0;
	m_startTick = 0;
	m_tmSampling = 500;
	m_hourState = 0;
	m_dayState = 0;
	m_updateRTC = 0;
	m_preTime = 0;
	m_lastTime = 0;
}

void CXTaskSensorData::DoLoop(U16 tmOnce)
{
	struct clock_info time_struct;
	RTC_DATE_TIME time;
	
	if(m_updateRTC == 1)																		//update rtc
	{
		if(update_utc_time_enable == 0)												//check if end to get time from wifi module
		{
			if(m_preTime == 0)
			{
				m_preTime = conf_time;														//save first data from wifi module
				update_utc_time_enable = 1;
			}
			else if(m_lastTime == 0)
			{
				m_lastTime = conf_time;														//save second data from wifi module
			}	
			else if((m_lastTime - m_preTime) < 120)							//compare time if it is ok
			{
				u32 localTime = time_local((const U32 *)&m_lastTime);  //convert utc to local time
				clock_fill_details(&time_struct,localTime);
				time.years = time_struct.year;
				time.months = time_struct.month;
				time.days = time_struct.days;
				time.weaks = time_struct.day_of_week;
				time.hours = time_struct.hour;
				time.minute = time_struct.min;
				time.second = time_struct.sec;
				set_RTC(&time);																				//set RTC time
				m_enable = 1;																					//enable flag
				m_updateRTC = 0;																			//clear flag
				m_preTime = 0;																				//clear data
				m_lastTime = 0;																				//clear data
			}
			else																									//data exception
			{
				m_preTime = 0;																				//get data from wifi module again
				m_lastTime = 0;
				update_utc_time_enable = 1;
			}
		}
	
	}
	else if(m_enable == 0)
	{
		if(0 == is_RTC_RESET())												//check if RTC is reset
		{
			m_enable = 1;																//no reset , RTC is ok
		}
		else																					//RTC is reset 
		{
			update_utc_time_enable = 1;									//get time data from wifi module
			m_updateRTC = 1;
		}
		if(m_enable == 1)
		{
			get_RTC(&now_time);												//get RTC time first
			m_preHours = now_time.hours;							//save current hours
		}
	}
	if(m_enable == 1)
	{
		m_sysTick = ayla_tick();
		if((m_sysTick - m_startTick) > m_tmSampling)				//sampling every 5 second
		{
			m_startTick = m_sysTick;
			get_RTC(&now_time);
			if(now_time.hours != m_preHours)
				m_hourState = 1;															//get sensor data in hours
			if((now_time.hours == 23)&&(now_time.hours != m_preHours))
			{
				m_dayState = 1;																//get sensor data every day
				m_updateRTC = 1;															//update rtc time every day
				update_utc_time_enable = 1;										//update rtc time every day
			}
				
			m_preHours = now_time.hours;
		}
	}
	if(m_hourState == 1)
	{
		m_hourState = 0;
		
	}
	if(m_dayState == 1)
	{
		m_dayState = 0;
		
	}
}

U8 CXTaskSensorData::getTimeHours(void)
{
	RTC_DATE_TIME time;
	get_RTC(&time);
	return time.hours;
}

U32 CXTaskSensorData::getUTCTime(void)
{
	return conf_time;
}

/*
 * Get local time. Takes into account timezone + DST
 * Optional pointer to current UTC time to calculate
 * local time for.
 */
U32 CXTaskSensorData::time_local(const U32 *utc)
{
#if defined(SCHED_TEST) || defined(DEMO_TIME_LIB)
	u32 utc_time = *utc;
#else
	u32 utc_time = (utc) ? *utc : clock_utc();
#endif
	u32 local_time;

	if (!conf_timezone_settings.valid) {
		return utc_time;
	}
	local_time = utc_time - conf_timezone_settings.mins * 60;
	if (conf_daylight_info.valid &&
	    conf_daylight_info.active == (utc_time < conf_daylight_info.change)) {
		local_time += DAYLIGHT_OFFSET;
	}

	return local_time;
}
