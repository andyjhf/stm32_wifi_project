#ifndef __APP_XTASKWIFI_H
#define	__APP_XTASKWIFI_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "MUtility.h"

class CXTaskComWifi
{
public:
	CXTaskComWifi(void);
	~CXTaskComWifi(void);

public:
	void InitTask(void);                           // task initialization
	void DetectFactory(void);                      // detect factory setting
	void DoReal(void);                             // task loop
	void ParseConf(U8 *command, U8 size);          // command routine for wifi module
private:
	U8 m_factory;                                  // factory reset flag(by dip)
	U8 m_enable;                                   // enable to receive and transmit props
	U8 m_startAp;                                  // start wifi module to ap mode
	U8 m_factory2;                                 // factory2 reset flag(by cmd)
};

#ifdef __cplusplus
}
#endif

#endif /* __APP_XTASKWIFI_H */
