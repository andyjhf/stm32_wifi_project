#ifndef __APP_XTASKSENSORDATA_H
#define	__APP_XTASKSENSORDATA_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "MUtility.h"

class CXTaskSensorData
{
public:
	CXTaskSensorData(void);
	~CXTaskSensorData(void);

public:
	void InitTask(void);                          // task initialization
	void DoLoop(U16 tmOnce);                      // task loop

private:
	U8 getTimeHours(void);                        // sampling routine
	U32 getUTCTime(void);
	U32	time_local(const U32 *utc);

private:
	U8	m_enable;																	//RTC config state
	U8	m_curHours;
	U8	m_preHours;
	U32	m_sysTick;
	U32	m_tmSampling;
	U32 m_startTick;
	U8	m_hourState;
	U8	m_dayState;
	U8	m_updateRTC;
	U32	m_preTime;
	U32	m_lastTime;
};

#ifdef __cplusplus
}
#endif

#endif /* __APP_XTASKSENSORDATA_H */
