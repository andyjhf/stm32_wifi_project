#ifndef __APP_XTASKCOMVRF_H
#define	__APP_XTASKCOMVRF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "MUtility.h"
#include "MSerial.h"

#define F_SLAVEADDR       0                             // start flag
#define F_FUCTIONS        1                             // command type
#define F_REGISTER        2                             // destination address
#define F_BYTECOUNT       2                             // destination address

#define X_READ				0x03						//read command
#define X_WRITE				0x10						//write command

#define NO_SENSOR									(-20000)
#define FAULT_SENSOR							(-10000)

#define REG_ADDR_POWER						0x00
#define REG_ADDR_SLEEP						0x01
#define REG_ADDR_FAN							0x02
#define REG_ADDR_TEMP							0x03
#define REG_ADDR_HUMI							0x04
#define REG_ADDR_CO2							0x05
#define REG_ADDR_TVOC							0x06
#define REG_ADDR_PM2_5						0x07
#define REG_ADDR_HCHO							0x08
#define REG_ADDR_FAULT						0x09
#define REG_ADDR_DEV_INFO					0x0a
#define REG_ADDR_DEV_STATE				0x0b
#define REG_ADDR_MAX_NUM					0x0c

typedef struct tagXERVCtrlCmd{                    // struct of ERV control command 
	U8  addr;                                      // ERV addr
	U8  rw_mode;                                   // mode bus mode
	U16 reg_addr;                                  // register addr
	U16 reg_data;                                  // register data
}XERVCtrlCmd;

typedef struct tagXRemoteCmd{                   // struct of ERV control command 
	U8  group;                                  // erv group number
	U8  mask;                                  	// erv mask bit
	U8  data;                                   // erv data
}XRemoteCmd;

class CXTaskComERV: public CMSerial
{
public:
	CXTaskComERV(void);
	~CXTaskComERV(void);

public:
	void InitTask(void);
	void ParseRemote(U8 *szRemote, U8 size);
#if DEBUG_LOG == 1
	void debug(void);
#endif
	virtual U16 OnNewSend();
	virtual U16 OnNewRecv();

private:
	U8 checkFrame(void);
	void onError(void);

private:
	U8 m_recvCmd;
	U8 m_recvParaCnt;
	U8 m_recvSrc;

	tagXERVCtrlCmd   m_ervctrlCmd;
	CMQueue *m_queue;

	U16 m_errCnt;
	U8	m_sendSlaveSddr;
#if DEBUG_LOG == 1
	U8 m_debug;
#endif
};

#ifdef __cplusplus
}
#endif
#endif /* __APP_XTASKCOMVRF_H */
