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
}

void CXTaskSensorData::DoLoop(U16 tmOnce)
{
	struct clock_info time_struct;
	RTC_DATE_TIME time;
	if(m_enable == 0)
	{
		if(0 == is_RTC_RESET())
		{
			m_enable = 1;
		}
		else if (clock_boot_rel_time != 0)	                       // wifi ready pin is OK
		{
			u32 curTime = clock_utc();
			u32 localTime = clock_local((const u32 *)&curTime);
			clock_fill_details(&time_struct,localTime);
			time.years = time_struct.year;
			time.months = time_struct.month;
			time.days = time_struct.days;
			time.weaks = time_struct.day_of_week;
			time.hours = time_struct.hour;
			time.minute = time_struct.min;
			time.second = time_struct.sec;
			set_RTC(&time);
			m_enable = 1;
		}
		if(m_enable == 1)
		{
			get_RTC(&now_time);
			m_preHours = now_time.hours;
		}
	}else
	{
		m_sysTick = ayla_tick();
		if((m_sysTick - m_startTick) > m_tmSampling)
		{
			m_startTick = m_sysTick;
			get_RTC(&now_time);
			if(now_time.hours != m_preHours)
				m_hourState = 1;
			if((now_time.hours == 23)&&(now_time.hours != m_preHours))
				m_dayState = 1;
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


