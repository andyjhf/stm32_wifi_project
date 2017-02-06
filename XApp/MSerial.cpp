/*******************************************************************************
 * @file    MSerial.cpp
 * @author  jianchuan.zhao
 * @version V1.0.0
 * @date    12/12/2014
 * @brief   This file provides all ... functions.
 *******************************************************************************/
 
#include "MSerial.h"

// weak functions
__weak void USART6_OffReceive(void) {}                   // turn off receiver
__weak void USART6_PreReceive(void) {}                   // prepare for receiving
__weak void USART6_PreTransmit(void){}                   // prepare for transmitting
__weak void USART6_StartSend(void)  {}                   // start to transmit
__weak U8   USART6_GetByte(U8* pData)       { return 0;} // get a byte from USAR6T-buffer
__weak U8   USART6_Write(U8* pData, U8 len) { return 0;} // write some bytes to USART6-buffer
__weak U8   USART6_GetStatus(void)          { return 0;} // get usart6 state

CMSerial::CMSerial(void)
{
	m_state   = S_ONSEND;                         // state
	m_stateCnt= 0;                                 // time count of one state
	m_rxLen   = 0;                                 // received data size
	m_txLen   = 0;                                 // transmit data size
	m_idleCnt = 0;                                 // timer count of interval between one char and one char
	
	m_onesendCnt = 500;															//timer count of send frequency

	m_tmRxOver= 5;                                 // configured interval time
	m_tmNoAck = 200;                                // configured no acknowledge time
	m_tmWait  = 5;                                 // configured waiting time
}

CMSerial::~CMSerial(void)
{
}

void CMSerial::DoLoop(U16 tmOnce)
{
	U8 state=m_state;                              // hold current state
	switch(state)                                  // switch case
	{
		case S_ONSEND:                             // handle to send state
			OnSend();                              // handle to format what data should be sent or not
			break;
		case S_WAITING:                            // waiting state
			Waiting();                             // wait fot a little while then start to send(rs485 send state)
			break;
		case S_SENDING:                            // sending state
			Sending();                             // check whether the sending is completed
			break;
		case S_RECEIVE:                            // receiving state
			Receiving(tmOnce);                     // copy uart rx data to receiver buffer
			break;
		case S_ONRECV:                             // receive over state
			OnReceive();                           // handle to received data
			break;
	}
	if(state!=m_state)                             // state change
	{
		m_stateCnt=0;                              // clear state counter
	}
	m_stateCnt += tmOnce;                          // count-up state counter
}

void CMSerial::OnSend(void)
{
	m_txLen = 0;                                   // clear tranimit buffer
	if(m_stateCnt > m_onesendCnt)
	{
		if(OnNewSend())                                // handle whether there is data to send or not
		{
			m_state = S_WAITING;                       // if need to send data this time then turn to [waiting] state
			USART6_PreTransmit();                      // diable RXNEIE,enable TCIE and RS485-DE,clear buffer
			USART6_Write(m_txBuf, m_txLen);            // copy send data to uart tx buffer
		}
//		else
//		{
//			m_state = S_RECEIVE;                       // if there is no data to send, turn to [receive] state
//			m_rxLen = 0;                               // clear receiver buffer
//			USART6_PreReceive();                       // set uart to rx state
//		}
	}
}

void CMSerial::Waiting(void)
{
	if(m_stateCnt>=m_tmWait)                       // waiting time is over
	{
		m_state = S_SENDING;                       // turn to [sending] state
		USART6_StartSend();                        // starting to transmit
	}
}

void CMSerial::Sending(void)
{
	if(0==USART6_GetStatus())                      // send over
	{
		m_state = S_RECEIVE;                       // change to receive state
		m_rxLen = 0;                               // clear receiver buffer
		USART6_PreReceive();                       // set uart to rx state
	}
}

void CMSerial::Receiving(U16 tmOnce)
{
	while(USART6_GetByte(&m_rxBuf[m_rxLen]))       // get uart rx data to receiver buffer one by one
	{
		m_rxLen++;                                 // receiver length plus one
		m_idleCnt = 0;                             // clear idle counter
		if(m_rxLen==0xff)                          // out of buffer(maybe dirty data or bus noise of long time)
			break;
	}
	m_idleCnt += tmOnce;                           // count-up idle counter
	if((m_rxLen>0 && m_idleCnt>=m_tmRxOver) ||     // frame end, 
		m_idleCnt>m_tmNoAck || m_rxLen>=255)       // timeout, out of buffer
	{
		USART6_OffReceive();                       // close uart rx function
		m_state = S_ONRECV;                        // change to OnReceive state
	}
}

void CMSerial::OnReceive(void)
{
//	if(OnNewRecv())                                // handle to received data and cheak whether need to be sent
//	{
//		m_state = S_ONSEND;                        // turn to OnSend state
//		m_txLen = 0;                               // clear send buffer
//	}
//	else                                           // do not need to send data
//	{
//		m_state = S_RECEIVE;                       // turn to receive state
//		m_rxLen=0;                                 // clear receiver buffer
//		USART6_PreReceive();                       // set uart to rx state
//	}
	OnNewRecv();
	m_state = S_ONSEND;                        // turn to OnSend state
	m_txLen = 0;                               // clear send buffer
	
	m_idleCnt=0;                                   // clear idle counter
	m_stateCnt=0;                                  // clear state counter
}
