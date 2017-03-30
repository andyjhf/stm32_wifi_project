#include "XTaskComERV.h"
#include "XApp.h"

// note:
//#define	ID_IDLE_CNT		(20/CYCLE_ID)
//#define ID_NOACK_CNT	(80/CYCLE_ID)
//#define	ID_PRESEND_CNT	(10/CYCLE_ID)

CXTaskComERV::CXTaskComERV(void):CMSerial()
{
	m_tmRxOver = 5;
	m_tmNoAck  = 200;
	m_tmWait   = 5;
	m_sendSlaveSddr = 0;

	m_recvCmd = 0;
	m_recvSrc = 0;

	//m_ctrlCmd
	m_queue   = new CMQueue(sizeof(tagXERVCtrlCmd),128);
	m_errCnt  = 0;
#if DEBUG_LOG == 1
	m_debug = 0;
#endif
}

CXTaskComERV::~CXTaskComERV(void)
{
}

void CXTaskComERV::InitTask(void)
{
	m_tmRxOver = 5;
	m_tmNoAck  = 200;
	m_tmWait   = 5;
	m_sendSlaveSddr = 0;
	
	m_recvCmd = 0;
	m_recvSrc = 0;

	//m_ctrlCmd
	//m_queue = new CMQueue(sizeof(tagXCtrlCmd),128);

	m_errCnt  = 0;
#if DEBUG_LOG == 1
	m_debug = 0;
#endif
}

U16 CXTaskComERV::OnNewSend()
{
	if(m_queue->Pop(&m_ervctrlCmd))
	{
		m_txBuf[0] = m_ervctrlCmd.addr;												//slave address
		m_txBuf[1] = m_ervctrlCmd.rw_mode;										//Fuctions
		m_txBuf[2] = HIBYTE(m_ervctrlCmd.reg_addr);						//Starting address HI
		m_txBuf[3] = LOBYTE(m_ervctrlCmd.reg_addr);						//Starting address LO
		m_txBuf[4] = 0x00;																		//Num of Registers Hi
		m_txBuf[5] = 0x01;																		//Num of Registers LO
		m_txBuf[6] = 0x02;																		//Byte count
		m_txBuf[7] = HIBYTE(m_ervctrlCmd.reg_data);						//Data HI
		m_txBuf[8] = LOBYTE(m_ervctrlCmd.reg_data);						//Data LO
		
		U16 usCRC16=IDCOM_CRC16(&m_txBuf[0], 0x09);
		m_txBuf[9] = LOBYTE(usCRC16);												//CRC LO
		m_txBuf[10] = HIBYTE(usCRC16);													//CRC HI
		
		m_txLen = 11;
		
		m_sendSlaveSddr = m_txBuf[0];
	}
#if DEBUG_LOG == 1
	else if(m_debug == 1)
	{
		m_debug = 0;
		m_txBuf[0] = 0x02;
		memcpy(&m_txBuf[1],g_szErvinfo,30);
		m_txLen = 31;
	}
#endif
	else{
		m_txBuf[0] = 1;																				//slave address
		m_txBuf[1] = X_READ;																	//Fuctions
		m_txBuf[2] = 0x00;																		//Starting address HI
		m_txBuf[3] = 0x00;																		//Starting address LO
		m_txBuf[4] = 0x00;																		//Num of Registers Hi
		m_txBuf[5] = REG_ADDR_MAX_NUM;												//Num of Registers LO

		U16 usCRC16=IDCOM_CRC16(&m_txBuf[0], 0x06);
		m_txBuf[6] = LOBYTE(usCRC16);													//CRC LO
		m_txBuf[7] = HIBYTE(usCRC16);													//CRC HI

		m_txLen = 8;
		
		m_sendSlaveSddr = m_txBuf[0];
	}
	return 1;
}

U16 CXTaskComERV::OnNewRecv()
{
	if (0==checkFrame())                           // 1.check frame(length,header/tail,CRC)
	{
		onError();
		m_sendSlaveSddr = 0;
		return 0;
	}
//	g_ervinfo[ERVINFO_INDEX_BASE_STATE] |= OD_DAT_READY;                      // outdoor ready OK and clear error counter
	m_errCnt = 0;
	m_recvCmd = m_rxBuf[F_FUCTIONS];	                   // the command of an effective frame	
	
	
	if(m_recvCmd == X_WRITE)
	{
		
	}else if(m_recvCmd == X_READ)
	{
		m_recvParaCnt = m_rxBuf[F_BYTECOUNT];
		U8 *data = &m_rxBuf[F_BYTECOUNT+1];
		U16 param;
		g_ervinfo[ERVINFO_INDEX_GROUP_NUM] = m_rxBuf[F_SLAVEADDR];
		for(U8 i=0;i<m_recvParaCnt;i+=2)
		{
			param = data[i]*256 + data[i+1];
			switch(i/2)             
			{
				case REG_ADDR_POWER:
				{
					if(param > 0)
						g_ervinfo[ERVINFO_INDEX_BASE_STATE] |= POWER_STATE;
					else
						g_ervinfo[ERVINFO_INDEX_BASE_STATE] &= ~POWER_STATE;
				}
				break;
				case REG_ADDR_SLEEP:
				{
					if(param > 0){
						g_ervinfo[ERVINFO_INDEX_FAN_SLEEP] |= SLEEP_STATE;
						g_ervinfo[ERVINFO_INDEX_FAN_SLEEP] |= SLEEP_AVAILABLE_STATE;
					}
					else{
						g_ervinfo[ERVINFO_INDEX_FAN_SLEEP] &= ~SLEEP_STATE;
						g_ervinfo[ERVINFO_INDEX_FAN_SLEEP] |= SLEEP_AVAILABLE_STATE;
					}
				}
				break;
				case REG_ADDR_FAN:
				{
					g_ervinfo[ERVINFO_INDEX_FAN_SLEEP] &= ~FAN_STATE;
					g_ervinfo[ERVINFO_INDEX_FAN_SLEEP] |= ((param << 1) & FAN_STATE);
					g_ervinfo[ERVINFO_INDEX_FAN_SLEEP] |= FAN_AVAILABLE_STATE;
				}
				break;
				case REG_ADDR_TEMP:
				{
					S16 temp = (S16)param;
					if(temp == NO_SENSOR)
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] &= ~TEMP_AVAILABLE_STATE;
					else
					{
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] |= TEMP_AVAILABLE_STATE;
						if(temp != FAULT_SENSOR)
							g_ervinfo[ERVINFO_INDEX_TEMP_VAL] = (S8)(temp/10);
					}
				}
				break;
				case REG_ADDR_HUMI:
				{
					S16 humi = (S16)param;
					if(humi == NO_SENSOR)
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] &= ~HUMI_AVAILABLE_STATE;
					else
					{
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] |= HUMI_AVAILABLE_STATE;
						if(humi != FAULT_SENSOR)
							g_ervinfo[ERVINFO_INDEX_HUMI_VAL] = (S8)(humi/10);
					}
				}
				break;
				case REG_ADDR_CO2:
				{
					S16 co2 = (S16)param;
					if(co2 == NO_SENSOR)
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] &= ~CO2_AVAILABLE_STATE;
					else
					{
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] |= CO2_AVAILABLE_STATE;
						if(co2 != FAULT_SENSOR)
						{
							g_ervinfo[ERVINFO_INDEX_CO2_VAL_HI] = data[i];
							g_ervinfo[ERVINFO_INDEX_CO2_VAL_LO] = data[i+1];
						}
					}
				}
				break;
				case REG_ADDR_TVOC:
				{
					S16 tvoc = (S16)param;
					if(tvoc == NO_SENSOR)
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] &= ~TVOC_AVAILABLE_STATE;
					else
					{
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] |= TVOC_AVAILABLE_STATE;
						if(tvoc != FAULT_SENSOR)
						{
							g_ervinfo[ERVINFO_INDEX_TVOC_VAL] = (U8)param;
						}
					}
				}
				break;
				case REG_ADDR_PM2_5:
				{
					S16 pm2_5 = (S16)param;
					if(pm2_5 == NO_SENSOR)
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] &= ~PM2_5_AVAILABLE_STATE;
					else
					{
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] |= PM2_5_AVAILABLE_STATE;
						if(pm2_5 != FAULT_SENSOR)
						{
							g_ervinfo[ERVINFO_INDEX_PM2_5_VAL_HI] = data[i];
							g_ervinfo[ERVINFO_INDEX_PM2_5_VAL_LO] = data[i+1];
						}
					}
				}
				break;
				case REG_ADDR_HCHO:
				{
					S16 hcho = (S16)param;
					if(hcho == NO_SENSOR)
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] &= ~HCHO_AVAILABLE_STATE;
					else
					{
						g_ervinfo[ERVINFO_INDEX_SENSOR_AVAILABLE] |= HCHO_AVAILABLE_STATE;
						if(hcho != FAULT_SENSOR)
						{
							g_ervinfo[ERVINFO_INDEX_HCHO_VAL] = (U8)param;
						}
					}
				}
				break;
				case REG_ADDR_FAULT:
				{
					if(param != 0x00)
						g_ervinfo[ERVINFO_INDEX_BASE_STATE] |= FAULT_STATE;
					else
						g_ervinfo[ERVINFO_INDEX_BASE_STATE] &= ~FAULT_STATE;
					g_ervinfo[ERVINFO_INDEX_FAULT_HI] = data[i];
					g_ervinfo[ERVINFO_INDEX_FAULT_LO] = data[i+1];
				}
				break;
				case REG_ADDR_DEV_INFO:
				{
					if(param & 0x01)
						g_ervinfo[ERVINFO_INDEX_HEAT_CLEAN] |= HEATING_AVAILABLE_STATE;
					else
						g_ervinfo[ERVINFO_INDEX_HEAT_CLEAN] &= ~HEATING_AVAILABLE_STATE;
					if(param & 0x02)
						g_ervinfo[ERVINFO_INDEX_HEAT_CLEAN] |= CLEAN_AVAILABLE_STATE;
					else
						g_ervinfo[ERVINFO_INDEX_HEAT_CLEAN] &= ~CLEAN_AVAILABLE_STATE;
				}
				break;
				case REG_ADDR_DEV_STATE:
				{
					if(param & 0x01)
						g_ervinfo[ERVINFO_INDEX_HEAT_CLEAN] |= HEATING_STATE;
					else
						g_ervinfo[ERVINFO_INDEX_HEAT_CLEAN] &= ~HEATING_STATE;
					if(param & 0x02)
						g_ervinfo[ERVINFO_INDEX_HEAT_CLEAN] |= CLEAN_STATE;
					else
						g_ervinfo[ERVINFO_INDEX_HEAT_CLEAN] &= ~CLEAN_STATE;
				}
				break;
				default:
					
				break;
			}	
		}
		
	}else{
		
		
	}
	g_rs485 = 1;
	g_ervinfo[ERVINFO_INDEX_BASE_STATE] |= OD_DAT_LINK;
	m_sendSlaveSddr = 0;
	return 1;
}

void CXTaskComERV::ParseRemote(U8 *remote, U8 size)
{
	XERVCtrlCmd	 ERVCtrlCmd;
	XRemoteCmd   RemoteCmd;
	
	if(size>sizeof(RemoteCmd))
	{
		size=sizeof(RemoteCmd);
	}
	memset((U8*)&RemoteCmd,      0, sizeof(RemoteCmd));
	memcpy((U8*)&RemoteCmd, remote, size);
	
	if(RemoteCmd.mask & ERV_CMD_LOCK_AVAILABLE_MASK)
	{
		
	}
	if(RemoteCmd.mask & ERV_CMD_POWER_AVAILABLE_MASK)
	{
		ERVCtrlCmd.addr = RemoteCmd.group;
		ERVCtrlCmd.rw_mode = X_WRITE;
		ERVCtrlCmd.reg_addr = REG_ADDR_POWER;
		ERVCtrlCmd.reg_data = (RemoteCmd.data & ERV_CMD_POWER_DATA_MASK) >> 1;
		m_queue->Push(&ERVCtrlCmd);
	}
	if(RemoteCmd.mask & ERV_CMD_SLEEP_AVAILABLE_MASK)
	{
		ERVCtrlCmd.addr = RemoteCmd.group;
		ERVCtrlCmd.rw_mode = X_WRITE;
		ERVCtrlCmd.reg_addr = REG_ADDR_SLEEP;
		ERVCtrlCmd.reg_data = (RemoteCmd.data & ERV_CMD_SLEEP_DATA_MASK) >> 2;
		m_queue->Push(&ERVCtrlCmd);
	}
	if(RemoteCmd.mask & ERV_CMD_FAN_AVAILABLE_MASK)
	{
		ERVCtrlCmd.addr = RemoteCmd.group;
		ERVCtrlCmd.rw_mode = X_WRITE;
		ERVCtrlCmd.reg_addr = REG_ADDR_FAN;
		ERVCtrlCmd.reg_data = (RemoteCmd.data & ERV_CMD_FAN_DATA_MASK) >> 4;
		m_queue->Push(&ERVCtrlCmd);
	}
}

U8 CXTaskComERV::checkFrame(void)
{
	// check frame length
	if(m_rxLen<8)
	{
		return 0;
	}
	if(m_sendSlaveSddr != m_rxBuf[0])
	{
		return 0;
	}

	// check frame CRC (TODO:do not check CRC, reduce the execution time)
	if(IDCOM_CRC16(&m_rxBuf[0], m_rxLen-2)!= ((m_rxBuf[m_rxLen-1]<<8)|m_rxBuf[m_rxLen-2]))
	{
		return 0;
	}

	return 1;
}

void CXTaskComERV::onError(void)
{
	if(m_errCnt<0xffff)
		m_errCnt++;

	// blink quickly for 30 secs when frame check is error
//	if(m_errCnt>=8)  // 5 seconds
//	{
//		g_ervinfo[ERVINFO_INDEX_BASE_STATE] &= ~OD_DAT_READY;
//	}

	if(0==m_rxLen)
	{
		g_rs485 = 1;
		g_ervinfo[ERVINFO_INDEX_BASE_STATE] &= ~OD_DAT_LINK;                  // outdoor offline              // the A/B wires are open,blink slowly
	}
	else
	{
		g_rs485 = 0;
	}
	g_ervinfo[ERVINFO_INDEX_BASE_STATE] &= ~OD_DAT_LINK;                  // outdoor offline
	
	// determine the error kind after 30 secs(A/B wires reversed or A/B wires are open)
	if(m_errCnt>=300)
	{
		// update g_sys[15] data(outdoor ready over and link state)
		m_queue->Clear();                          // clear remote ctrl list
	}
}
#if DEBUG_LOG == 1
void CXTaskComERV::debug(void)
{
	m_debug = 1;
}
#endif

