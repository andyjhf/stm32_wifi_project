#include "XTaskComVRF.h"
#include "XApp.h"

// note:
//#define	ID_IDLE_CNT		(20/CYCLE_ID)
//#define ID_NOACK_CNT	(80/CYCLE_ID)
//#define	ID_PRESEND_CNT	(10/CYCLE_ID)

CXTaskComVRF::CXTaskComVRF(void):CMSerial()
{
	m_tmRxOver = 5;
	m_tmNoAck  = 100;
	m_tmWait   = 5;

	m_recvCmd = 0;
	m_recvSrc = 0;
	m_Data14  = 0;

	//m_ctrlCmd
	m_queue   = new CMQueue(sizeof(tagXCtrlCmd),128);
	m_dipAddr = 0;
	m_errCnt  = 0;
	m_idNum   = 0;
	memset(m_idLive,sizeof(m_idLive),0);
	m_odDataNoCnt = 0;

}

CXTaskComVRF::~CXTaskComVRF(void)
{
}

void CXTaskComVRF::InitTask(void)
{
	m_tmRxOver = 5;
	m_tmNoAck  = 100;
	m_tmWait   = 5;

	m_recvCmd = 0;
	m_recvSrc = 0;
	m_Data14  = 0;

	//m_ctrlCmd
	//m_queue = new CMQueue(sizeof(tagXCtrlCmd),128);
	m_dipAddr = 0;
	m_errCnt  = 0;
	m_idNum   = 0;
	memset(m_idLive,sizeof(m_idLive),0);
	m_odDataNoCnt = 0;

}

U16 CXTaskComVRF::OnNewSend()
{
	if(CMD_S3_22 == m_recvCmd && 0x01==m_rxBuf[F_D0])
	{
		if(m_queue->Pop(&m_ctrlCmd))
		{
			if(XRMT_A==m_ctrlCmd.Type)
			{
				buildBroad_12(&m_ctrlCmd);
				return 1;
			}
			else if(XRMT_B==m_ctrlCmd.Type)
			{
				buildReply_83(0x04, &m_ctrlCmd.Group[0]);
				return 1;
			}
		}
		else
		{
			buildReply_83(0x03,0);
			return 1;
		}
	}
	else if(CMD_S3_22 == m_recvCmd && 0x02==m_rxBuf[F_D0])
	{
		buildReply_83(0x03,0);
		return 1;
	}
	else if(CMD_FCT_55 == m_recvCmd)
	{
		buildFct_AA();
		return 1;
	}

	return 0;
}

U16 CXTaskComVRF::OnNewRecv()
{
	static U16 onNewCnt=0;

	if(onNewCnt<5)	                               // 0.waiting for address of dip(SW1_5 and SW1_6)
	{
		onNewCnt++;                                // [20(bus idle)+23(sending data)]*4<=x<=4*100(timeout)
		if(onNewCnt>=2)                            // 20(bus idle)+23(sending data)<=x<=2*100(timeout)
		{
			m_dipAddr = (g_host[1]&0x03)+1;
		}
	}

	if (0==checkFrame())                           // 1.check frame(length,header/tail,CRC)
	{
		onError();
		return 0;
	}
	g_sys[4] |= OD_DAT_READY;                      // outdoor ready OK and clear error counter
	m_errCnt = 0;
	m_recvCmd = m_rxBuf[F_CMD];	                   // the command of an effective frame	
	m_recvSrc = m_rxBuf[F_SRC];

	if(CMD_S1_20 == m_recvCmd && 23 == m_rxLen)
	{
		onOdPoll_20();                             // 2.poll-1 poll IDU
	}
	else if(CMD_R1_80 == m_recvCmd && 1 <= m_recvSrc && m_recvSrc <= 64  && 23 == m_rxLen)
	{
		onIdReply_80();	                           // 3.Reply1 IDU data1(main)
	}
	else if(CMD_B1_10 == m_recvCmd  && 23 == m_rxLen)
	{                                              // 4.Broad1 EXV opening of Cool/Heat starting and the minmum 
		m_Data14 = m_rxBuf[F_D0+14];               // part of ODU fault code(commerical used, residential reserved)
	}
	else if(CMD_B3_24 == m_recvCmd && m_rxLen>=150)
	{
		onOdBroad_24();                            // 5.Broad3 ODU data
	}
	else if(CMD_S3_22 == m_recvCmd && (11==m_rxLen || 23==m_rxLen))
	{
		if(onOdPoll_22())                          // 6.Poll3 Poll PC(check pc online or not)
			return 1;                              // TODO: main aylaWifi-device is offline
	}
	else if(CMD_R3_83 == m_recvCmd)
	{
		// TODO: how many wifi-devices             // 7.Reply3 Id/PC request
	}
	else if(CMD_FCT_55==m_recvCmd && 23 == m_rxLen)
	{
		onPcFct_55();                              // 8.FCT PC request
		return 1;
	}

	return 0;
}

void CXTaskComVRF::ParseRemote(U8 *remote, U8 size)
{
	XCtrlCmd  ctrlCmd;
	XRemote   xRemote;

	if(size>sizeof(xRemote))
	{
		size=sizeof(xRemote);
	}
	memset((U8*)&xRemote,      0, sizeof(xRemote));
	memcpy((U8*)&xRemote, remote, size);

	// type1 remote
	ctrlCmd.Type = xRemote.Type;
	if(XRMT_A == xRemote.Type && size>=4 &&        // type A and length is OK
	  ((1<=xRemote.Cmd[0]&&xRemote.Cmd[0]<=64) || 0xff==xRemote.Cmd[0] || 0xfe==xRemote.Cmd[0])) 
	{
		if(0xff==xRemote.Cmd[0])                   // all groups
		{
			ctrlCmd.Group[0] = 0xff;
			ctrlCmd.Group[1] = 0xff;
			ctrlCmd.Group[2] = 0xff;
			ctrlCmd.Group[3] = 0xff;
			ctrlCmd.Group[4] = 0xff;
			ctrlCmd.Group[5] = 0xff;
			ctrlCmd.Group[6] = 0xff;
			ctrlCmd.Group[7] = 0xff;
		}
		else if(0xfe==xRemote.Cmd[0])              //  multi-groups
		{
			ctrlCmd.Group[0] = xRemote.Group[7];
			ctrlCmd.Group[1] = xRemote.Group[6];
			ctrlCmd.Group[2] = xRemote.Group[5];
			ctrlCmd.Group[3] = xRemote.Group[4];
			ctrlCmd.Group[4] = xRemote.Group[3];
			ctrlCmd.Group[5] = xRemote.Group[2];
			ctrlCmd.Group[6] = xRemote.Group[1];
			ctrlCmd.Group[7] = xRemote.Group[0];
		}
		else                                       // specified one group
		{
			ctrlCmd.Group[0] = ctrlCmd.Group[1] = ctrlCmd.Group[2] = ctrlCmd.Group[3] = 0;
			ctrlCmd.Group[4] = ctrlCmd.Group[5] = ctrlCmd.Group[6] = ctrlCmd.Group[7] = 0;
			ctrlCmd.Group[7-(xRemote.Cmd[0]-1)/8] = 1<<((xRemote.Cmd[0]-1)%8);
		}
		if(0x40 & xRemote.Cmd[1])                  // set temp
		{
			ctrlCmd.Data[0] = 4;
			ctrlCmd.Data[1] = 16 + ((xRemote.Cmd[2]&0x78)>>3);
			ctrlCmd.Data[2] = 0;
			ctrlCmd.Data[3] = 0;
			m_queue->Push(&ctrlCmd);
		}
		if(0x20 & xRemote.Cmd[1])                  // set mode
		{
			if((xRemote.Cmd[2]&0x07))
			{
				ctrlCmd.Data[0] = 2;
				ctrlCmd.Data[1] = (xRemote.Cmd[2]&0x07)-1; 
				ctrlCmd.Data[2] = 0;
				ctrlCmd.Data[3] = 0;
			}
			else                                   // Off
			{
				ctrlCmd.Data[0] = 0;
				ctrlCmd.Data[1] = 0;
				ctrlCmd.Data[2] = 0;
				ctrlCmd.Data[3] = 0;
			}
			m_queue->Push(&ctrlCmd);
		}
		if(0x10 & xRemote.Cmd[1])                  // set fan speed
		{
			ctrlCmd.Data[0] = 3;
			ctrlCmd.Data[1] = (xRemote.Cmd[3]&0xE0)>>5;
			ctrlCmd.Data[2] = 0;
			ctrlCmd.Data[3] = 0;
			m_queue->Push(&ctrlCmd);
		}
		if(0x08 & xRemote.Cmd[1])                  // lock flag
		{
			ctrlCmd.Data[0] = 1;
			ctrlCmd.Data[1] = (xRemote.Cmd[3]&0x10)>>4;
			ctrlCmd.Data[2] = 0;
			ctrlCmd.Data[3] = 0;
			m_queue->Push(&ctrlCmd);
		}
		if(0x04 & xRemote.Cmd[1])                  // louver flag
		{
			ctrlCmd.Data[0] = 8;
			ctrlCmd.Data[1] = (xRemote.Cmd[3]&0x08)>>3;
			ctrlCmd.Data[2] = 0;
			ctrlCmd.Data[3] = 0;
			m_queue->Push(&ctrlCmd);
		}
		// turn on or off the indoors
		if((0x80 & xRemote.Cmd[1]))                // On/Off
		{
			ctrlCmd.Data[0] = 0;
			ctrlCmd.Data[1] = (xRemote.Cmd[2]&0x80)>>7;
			ctrlCmd.Data[2] = 0;
			ctrlCmd.Data[3] = 0;
			m_queue->Push(&ctrlCmd);
		}
	}
	// type2 remote
	else if(XRMT_B==xRemote.Type && size>=6 && 
		((1<=xRemote.Cmd[0] && xRemote.Cmd[0]<=64) || 0xff==xRemote.Cmd[0] || 0xfe==xRemote.Cmd[0]))
	{
		if(0xff==xRemote.Cmd[0])                   // all groups
		{
			ctrlCmd.Group[0] = 0xff;
			ctrlCmd.Group[1] = 0xff;
			ctrlCmd.Group[2] = 0xff;
			ctrlCmd.Group[3] = 0xff;
			ctrlCmd.Group[4] = 0xff;
			ctrlCmd.Group[5] = 0xff;
			ctrlCmd.Group[6] = 0xff;
			ctrlCmd.Group[7] = 0xff;
		}
		else if(0xfe==xRemote.Cmd[0])              //  multi-groups
		{
			ctrlCmd.Group[0] = xRemote.Group[7];
			ctrlCmd.Group[1] = xRemote.Group[6];
			ctrlCmd.Group[2] = xRemote.Group[5];
			ctrlCmd.Group[3] = xRemote.Group[4];
			ctrlCmd.Group[4] = xRemote.Group[3];
			ctrlCmd.Group[5] = xRemote.Group[2];
			ctrlCmd.Group[6] = xRemote.Group[1];
			ctrlCmd.Group[7] = xRemote.Group[0];
		}
		else                                       // specified one group
		{
			ctrlCmd.Group[0] = ctrlCmd.Group[1] = ctrlCmd.Group[2] = ctrlCmd.Group[3] = 0;
			ctrlCmd.Group[4] = ctrlCmd.Group[5] = ctrlCmd.Group[6] = ctrlCmd.Group[7] = 0;
			ctrlCmd.Group[7-(xRemote.Cmd[0]-1)/8]= 1<<((xRemote.Cmd[0]-1)%8);
		}
		ctrlCmd.Data[0] = xRemote.Cmd[1];          // mode
		ctrlCmd.Data[1] = xRemote.Cmd[2];          // sleep/louver/lock
		ctrlCmd.Data[2] = xRemote.Cmd[3];          // fan
		ctrlCmd.Data[3] = xRemote.Cmd[4];          // SetTemp

		m_queue->Push(&ctrlCmd);
	}
}

U8 CXTaskComVRF::checkFrame(void)
{
	// check frame length
	if(m_rxLen<7)
	{
		return 0;
	}

	// check frame head and tail
	if(m_rxBuf[F_STX]!=0x02 || m_rxBuf[m_rxLen-1]!=0x03)
	{
		return 0;
	}

	// check frame CRC (TODO:do not check CRC, reduce the execution time)
//	if(IDCOM_CRC16(&m_rxBuf[1], m_rxLen-4)!= ((m_rxBuf[m_rxLen-3]<<8)|m_rxBuf[m_rxLen-2]))
//	{
//		return 0;
//	}

	return 1;
}

void CXTaskComVRF::onError(void)
{
	if(m_errCnt<0xffff)
		m_errCnt++;

	// blink quickly for 30 secs when frame check is error
	if(m_errCnt>=50)  // 5 seconds
	{
		g_sys[4] &= ~OD_DAT_READY;
	}

	// determine the error kind after 30 secs(A/B wires reversed or A/B wires are open)
	if(m_errCnt>=300)
	{
		// update g_sys[15] data(outdoor ready over and link state)
		if(0==m_rxLen)
		{
			g_sys[4] |= OD_DAT_READY;              // the A/B wires are open,blink slowly
		}
		else
		{
			g_sys[4] &= ~OD_DAT_READY;             // the A/B wires are opposite,blink quickly
		}
		g_sys[4] &= ~OD_DAT_LINK;                  // outdoor offline

		// all indoors offline
		m_idLive[0] = 0;                           // 01-08 b0:01#......b7:08#
		m_idLive[1] = 0;                           // 09-16 b0:09#......b7:16#
		m_idLive[2] = 0;                           // 17-24 b0:17#......b7:24#
		m_idLive[3] = 0;                           // 25-32 b0:25#......b7:32#
		m_idLive[4] = 0;                           // 33-40 b0:33#......b7:40#
		m_idLive[5] = 0;                           // 41-48 b0:41#......b7:48#
		m_idLive[6] = 0;                           // 49-56 b0:49#......b7:56#
		m_idLive[7] = 0;                           // 57-64 b0:57#......b7:64#

		// update indoor online flag in g_id[64][0]
		U8 mask=0;
		U8 idx=0;
		for(U8 n=0;n<64;n++)                       // loop all indoors
		{
			idx  = n/8;                            // index of id online array
			mask = 1<<(n%8);                       // bit of specified indoor
			if(0 == (m_idLive[idx]& mask))         // offline
			{
				g_id[n][0] &= ~0x80;               // clear bit7
			}
		}
		m_queue->Clear();                          // clear remote ctrl list
	}
}

void CXTaskComVRF::onOdPoll_20(void)
{
	static U8 IdNumMax  = 0;                       // in order to find the id number maximum in every polling
	static U8 PrevIdNum = 0;                       // id number when previous polling
	static U8 CurrIdNum = 0;                       // id number when current polling
	U8 *p = &g_sys[0];                             // pointer to g_sys[15]

	// update the number maximum
	if(IdNumMax<m_rxBuf[F_DST] && m_rxBuf[F_DST]<=64)
	{
		IdNumMax = m_rxBuf[F_DST];
	}

	// determine the IDU number according to IdNumMax and the dip address
	if(m_dipAddr!=0 && 1==m_rxBuf[F_DST])
	{
		CurrIdNum = IdNumMax ;
		// update effective indoor number
		if(CurrIdNum == PrevIdNum && CurrIdNum !=m_idNum)
		{
			m_idNum = CurrIdNum;
			p[3] = m_idNum;
		}
		PrevIdNum = CurrIdNum;

		// for next polling 
		IdNumMax = 0;
	}

	// update outdoor status and error code in g_od[15]
	if(1==m_rxBuf[F_DST])
	{
		// ODU offline
		if(m_odDataNoCnt<10)
		{
			m_odDataNoCnt++;
			if(m_odDataNoCnt>=5)
			{
				p[4] &= ~OD_DAT_LINK;
			}
		}
		
		// ODU mode & oil return flag
		if((m_rxBuf[F_D1]&0x01) && 1==m_rxBuf[F_D0])
		{
			p[4] &= ~0x0f;
			p[4] |= 4;                             // cool oil return
		}
		else if((m_rxBuf[F_D1]&0x01) && 2==m_rxBuf[F_D0])
		{
			p[4] &= ~0x0f;
			p[4] |= 5;                             // heat oil return
		}
		else
		{
			p[4] &= ~0x0f;
			p[4] |= m_rxBuf[F_D0];                 // 0stop/1cool/2heat/3defrost
		}
		// ODU FaultCode (SW Ver)>V1.6(broad1-Data14)
		if(m_rxBuf[F_D8]&0x01)
		{
			if(m_rxBuf[F_D0+8]&0xfe)
			{
				p[5] = (m_rxBuf[F_D8]>>1);
				p[6] = m_rxBuf[F_D7];
			}
			else
			{
				p[5] = 0x00;
				p[6] = m_Data14;
			}
		}
		else
		{
			p[5] = 0x00;
			p[6] = 0x00;
		}
	}
}

void CXTaskComVRF::onIdReply_80(void)
{
	// format IDU data
	U8 *p = &g_id[m_recvSrc-1][0];

	S16 sTemp = 0;
	S8  cTemp = 0;

	p[0] = 0x80;                                   // onlineflag bit7      (reply=online)
	p[0] |= (m_rxBuf[F_D3]&0x70);                  // actual fan bit6,5,4  (D3-H4)
	p[0] |= (m_rxBuf[F_D3]&0x07)<<1;               // set fan bit3,2,1     (D3-L4)
	p[0] |= ((m_rxBuf[F_D2]&0x04)?0x01:0);         // lock bit0            (D2-bit2)
	p[1]  = ((m_rxBuf[F_D2]&0x08)?0x80:0);         // standby bit7         (D2-bit3)
	p[1] |= ((m_rxBuf[F_D12]-16)&0x0f)<<3;         // set temp. bit6,5,4,3 (D12)
	p[1] |= (m_rxBuf[F_D1]&0x07);                  // mode bit2,1,0        (D1-bit2,1,0)

	p[2]  = m_rxBuf[F_D14];                        // group No.            (D14)
	sTemp = (m_rxBuf[F_D6]<<8) | m_rxBuf[F_D7];    // room temp.           (D6,D7)
	cTemp = (sTemp>=0) ? (S8)((sTemp+5)/10) : (S8)((sTemp-5)/10);
	p[3]  = (U8) cTemp;
	p[4]  = m_rxBuf[F_D13];                        // faule code           (D13)
}

U8 CXTaskComVRF::onOdPoll_22(void)
{
	static U16 CallCnt=0;
	CallCnt++;
	if(0x02==m_rxBuf[F_D0])
	{
		CallCnt=0;
	}
	// check whether there is a control command to send or need to response online
//	if((m_queue->GetNum() && (m_dipAddr==CallCnt))||  // control command
//		(1==m_dipAddr && (0x02==m_rxBuf[F_D0] || 6==CallCnt))) // response online
//	{
//		CallCnt=0;
//		return 1;
//	}
	if(m_queue->GetNum() && m_dipAddr==CallCnt)    // control command
	{
		CallCnt=0;
		return 1;
	}                                              // response online
	else if (1==m_dipAddr && (0x02==m_rxBuf[F_D0] || 6==CallCnt))
	{
		CallCnt=0;
		return 1;
	}
	return 0;
}

void CXTaskComVRF::onOdBroad_24(void)
{
	U8 *p    = &g_sys[0];
	U8  unit = m_rxBuf[F_D0];

	// update outdoor link state
	m_odDataNoCnt = 0;
	g_sys[4] |= OD_DAT_LINK;

	// ODU unit No.maximum is ODU number
	if(p[2]<unit && unit<4)
	{
		p[2] = unit;                               // ODU number
	}

	// unit No.1
	if(1==unit)
	{
		p[0] = m_rxBuf[F_D0+95];                   // SW version high byte
		p[1] = m_rxBuf[F_D0+96];                   // SW version low byte			
		p[7] = m_rxBuf[F_D0+15];                   // ambient temp. high byte
		p[8] = m_rxBuf[F_D0+16];                   // ambient temp. low byte

		// update IDU list online flags whenever anyone has been changed
		if( m_idLive[0] != m_rxBuf[F_D0+140] ||    // 01-08 b0:01#......b7:08#
			m_idLive[1] != m_rxBuf[F_D0+139] ||    // 09-16 b0:09#......b7:16#
			m_idLive[2] != m_rxBuf[F_D0+138] ||    // 17-24 b0:17#......b7:24#
			m_idLive[3] != m_rxBuf[F_D0+137] ||    // 25-32 b0:25#......b7:32#
			m_idLive[4] != m_rxBuf[F_D0+136] ||    // 33-40 b0:33#......b7:40#
			m_idLive[5] != m_rxBuf[F_D0+135] ||    // 41-48 b0:41#......b7:48#
			m_idLive[6] != m_rxBuf[F_D0+132] ||    // 49-56 b0:49#......b7:56#
			m_idLive[7] != m_rxBuf[F_D0+141])      // 57-64 b0:57#......b7:64#
		{
			m_idLive[0] = m_rxBuf[F_D0+140];
			m_idLive[1] = m_rxBuf[F_D0+139];
			m_idLive[2] = m_rxBuf[F_D0+138];
			m_idLive[3] = m_rxBuf[F_D0+137];
			m_idLive[4] = m_rxBuf[F_D0+136];
			m_idLive[5] = m_rxBuf[F_D0+135];
			m_idLive[6] = m_rxBuf[F_D0+132];
			m_idLive[7] = m_rxBuf[F_D0+131];

			// update indoor offline flag in g_id[64][0]
			U8 mask=0;
			U8 idx=0;
			for(U8 n=0;n<m_idNum;n++)
			{
				idx  = n/8;
				mask = 1<<(n%8);
				if(0 == (m_idLive[idx]& mask))
				{
					g_id[n][0] &= ~0x80;           // update online flag if indoor reply
				}
			}
		}
	}

	if(1<=unit && unit<=4)
	{
		p = &g_od[unit-1][0];
		// TODO:
	}
}

void CXTaskComVRF::buildBroad_12(XCtrlCmd *pCmd)
{
	m_txBuf[F_STX] = 0x02;
	m_txBuf[F_CMD] = CMD_B4_12;
	m_txBuf[F_DST] = 0xff;
	m_txBuf[F_SRC] = 0x00;

	m_txBuf[F_D0] = pCmd->Group[0];
	m_txBuf[F_D1] = pCmd->Group[1];
	m_txBuf[F_D2] = pCmd->Group[2];
	m_txBuf[F_D3] = pCmd->Group[3];
	m_txBuf[F_D4] = pCmd->Group[4];
	m_txBuf[F_D5] = pCmd->Group[5];
	m_txBuf[F_D6] = pCmd->Group[6];
	m_txBuf[F_D7] = pCmd->Group[7];

	m_txBuf[F_D8] = pCmd->Data[0];
	m_txBuf[F_D9] = pCmd->Data[1];
	m_txBuf[F_D10]= pCmd->Data[2];
	m_txBuf[F_D11]= pCmd->Data[3];
	m_txBuf[F_D12]= 0x00;
	m_txBuf[F_D13]= 0x00;
	m_txBuf[F_D14]= 0x00;
	m_txBuf[F_D15]= 0x00;

	U16 usCRC16=IDCOM_CRC16(&m_txBuf[1], F_D15);
	m_txBuf[F_CRH] = HIBYTE(usCRC16);
	m_txBuf[F_CRL] = LOBYTE(usCRC16);
	m_txBuf[F_ETX] = 0x03;

	m_txLen = 23;
}

void CXTaskComVRF::buildReply_83(U8 Data0, U8 *pData)
{
	m_txBuf[F_STX] = 0x02;
	m_txBuf[F_CMD] = CMD_R3_83;
	m_txBuf[F_DST] = 0x00;
	m_txBuf[F_SRC] = 0x00;                         // 0x00-PC/0xff-UnAssigned addr
	m_txBuf[F_D0]  = 0x03;                         // 1Read Para/2Write Para/3PC Online
	m_txBuf[F_D1]  = 0x00;                         // 4Ctrl IDU/5Start Address/6Exit Address
	m_txBuf[F_D2]  = 0x00;
	m_txBuf[F_D3]  = 0x00;
	m_txBuf[F_D4]  = 0x00;
	m_txBuf[F_D5]  = 0x00;
	m_txBuf[F_D6]  = 0x00;
	m_txBuf[F_D7]  = 0x00;
	m_txBuf[F_D8]  = 0x00;
	m_txBuf[F_D9]  = 0x00;
	m_txBuf[F_D10] = 0x00;
	m_txBuf[F_D11] = 0x00;
	m_txBuf[F_D12] = 0x00;
	m_txBuf[F_D13] = 0x00;
	m_txBuf[F_D14] = 0x00;
	m_txBuf[F_D15] = 0x00;
	m_txBuf[F_CRH] = 0x09;
	m_txBuf[F_CRL] = 0x4F;
	m_txBuf[F_ETX] = 0x03;

	m_txLen=23;

	if(0x03!=Data0 || NULL!=pData)
	{
		m_txBuf[F_D0] = Data0;
		if(0x01==Data0)                              // 1 read parameter
		{
			m_txBuf[F_D1]  = pData[0];
			m_txBuf[F_D2]  = pData[1];
		}
		else if(0x02==Data0)                         // 2 write parameter
		{
			m_txBuf[F_D1]  = pData[0];
			m_txBuf[F_D2]  = pData[1];	
			m_txBuf[F_D3]  = pData[2];
			m_txBuf[F_D4]  = pData[3];
			m_txBuf[F_D5]  = pData[4];
			m_txBuf[F_D6]  = pData[5];
		}
		else if(0x04==Data0)                         // 4.indoor control command id
		{
			m_txBuf[F_D1]  = pData[0];
			m_txBuf[F_D2]  = pData[1];	
			m_txBuf[F_D3]  = pData[2];
			m_txBuf[F_D4]  = pData[3];
			m_txBuf[F_D5]  = pData[4];
			m_txBuf[F_D6]  = pData[5];
			m_txBuf[F_D7]  = pData[6];
			m_txBuf[F_D8]  = pData[7];
			m_txBuf[F_D9]  = pData[8];
			m_txBuf[F_D10] = pData[9];
			m_txBuf[F_D11] = pData[10];
			m_txBuf[F_D12] = pData[11];
		}
		U16 usCRC16=IDCOM_CRC16(&m_txBuf[1], F_D15);
		m_txBuf[F_CRH] = HIBYTE(usCRC16);
		m_txBuf[F_CRL] = LOBYTE(usCRC16);
	}
}

void CXTaskComVRF::onPcFct_55(void)
{
	// 00 read
	// 00 00 Host(SWI/LED)
	// 00 01 VER
	// 00 02 model
	// 00 03 mfg_model
	// 00 04 mfg_serial
	// 00 05 DSN
	// 00 06 oem 
	// 00 07 oem model
	// 00 08 ssid

	// 01 write
	// 01 01/02(led2 on/off)  01/02(led3 on/off)  01/02(rst host/wifi)
	if(1==m_rxBuf[F_D0+0])
	{
		switch(m_rxBuf[F_D0+1])	                   // control LED2 manually
		{
			case 1:  g_led2=1;    break;
			case 2:  g_led2=0;    break;
			default: g_led2=0xff; break;
		}
		switch(m_rxBuf[F_D0+2])                    // control LED3 manually
		{
			case 1:  g_led3=1;    break;
			case 2:  g_led3=0;    break;
			default: g_led3=0xff; break;
		}	
		switch (m_rxBuf[F_D0+3])                   // 0x01 reset host, 0x02 reset wifi-module
		{
		case 1:
			g_hostReset=1;                         // 0x01 reset host
			break;
		case 2:
			g_wifiReset=1;                         // 0x02 reset wifi-module
			break;
		default:
			break;
		}
	}
}

void CXTaskComVRF::buildFct_AA(void)
{
//	00 00 Host(SWI/LED)
//	00 01 VER		       "V0.04_20160422"
//	00 02 model          AY001MTM1
//	00 03 mfg_model      CMWC1ZZABR
//	00 04 mfg_serial     scs0218
//	00 05 DSN             AC000W000470454
//	00 06 oem             802501b4
//	00 07 oem model       i-YORK-VRF-dev-cn
//	00 08 ssid            YORK-10a5d098f0a3	
//	00 09 OEM_HOST_VER   "YES_V1.04"

	U8 cmd  = m_rxBuf[F_D0];
	U8 text = m_rxBuf[F_D1];

	memset(m_txBuf,0,sizeof(m_txBuf));
	if(0==cmd && 0!=text)
	{
		m_txBuf[F_STX] = 0x02;
		m_txBuf[F_CMD] = CMD_FCT_AA;
		m_txBuf[F_DST] = 0xff;
		m_txBuf[F_SRC] = 0x00;
		
		U8 dataSize=0;
		switch(text)
		{
			case 1:
				memcpy(&m_txBuf[F_D0], FW_VER,          strlen(FW_VER));
				dataSize = strlen(FW_VER);
				break;
			case 2: 
				memcpy(&m_txBuf[F_D0], conf_model,      strlen(conf_model));
				dataSize = strlen(conf_model);
				break;
			case 3: 
				memcpy(&m_txBuf[F_D0], conf_mfg_model,  strlen(conf_mfg_model));
				dataSize = strlen(conf_mfg_model);
				break;
			case 4: 
				memcpy(&m_txBuf[F_D0], conf_mfg_serial, strlen(conf_mfg_serial));
				dataSize = strlen(conf_mfg_serial);
				break;
			case 5: 
				memcpy(&m_txBuf[F_D0], conf_dsn,        strlen(conf_dsn));
				dataSize = strlen(conf_dsn);
				break;
			case 6: 
				memcpy(&m_txBuf[F_D0], conf_oem,        strlen(conf_oem));
				dataSize = strlen(conf_oem);
				break;
			case 7:
				memcpy(&m_txBuf[F_D0], conf_oem_model,  strlen(conf_oem_model));
				dataSize = strlen(conf_oem_model);
				break;
			case 8:
				memcpy(&m_txBuf[F_D0], conf_ssid,       strlen(conf_ssid));
				dataSize = strlen(conf_ssid);
				break;
			case 9: 
				memcpy(&m_txBuf[F_D0], OEM_HOST_VER,    strlen(OEM_HOST_VER));
				dataSize = strlen(OEM_HOST_VER);
				break;
			default:
				memcpy(&m_txBuf[F_D0], FW_VER,          strlen(FW_VER));
				dataSize = strlen(FW_VER);
				break;
		}
		m_txBuf[F_D0+dataSize+0]='\0';
		dataSize++;

		dataSize = (dataSize<16) ? 16 : dataSize;
		U16 usCRC16=IDCOM_CRC16(&m_txBuf[1], dataSize+F_D0-1);
		m_txBuf[F_D0+dataSize+0] = HIBYTE(usCRC16);
		m_txBuf[F_D0+dataSize+1] = LOBYTE(usCRC16);
		m_txBuf[F_D0+dataSize+2] = 0x03;

		m_txLen = F_D0+dataSize+3;
	}
	//if((0==cmd && 0==text) || 1==cmd)
	else 
	{
		m_txBuf[F_STX] = 0x02;
		m_txBuf[F_CMD] = CMD_FCT_AA;
		m_txBuf[F_DST] = 0xff;
		m_txBuf[F_SRC] = 0x00;

		m_txBuf[F_D0]  = g_host[0];
		m_txBuf[F_D1]  = g_host[1];
		m_txBuf[F_D2]  = g_host[2];
		m_txBuf[F_D3]  = g_host[3];
		m_txBuf[F_D4]  = g_host[4];
		m_txBuf[F_D5]  = 0x00;
		m_txBuf[F_D6]  = 0x00;
		m_txBuf[F_D7]  = 0x00;
		m_txBuf[F_D8]  = 0x00;
		m_txBuf[F_D9]  = 0x00;
		m_txBuf[F_D10] = 0x00;
		m_txBuf[F_D11] = 0x00;
		m_txBuf[F_D12] = 0x00;
		m_txBuf[F_D13] = 0x00;
		m_txBuf[F_D14] = 0x00;
		m_txBuf[F_D15] = 0x00;

		U16 usCRC16=IDCOM_CRC16(&m_txBuf[1], F_D15);
		m_txBuf[F_CRH] = HIBYTE(usCRC16);
		m_txBuf[F_CRL] = LOBYTE(usCRC16);
		m_txBuf[F_ETX] = 0x03;

		m_txLen = 23;
	}
}
