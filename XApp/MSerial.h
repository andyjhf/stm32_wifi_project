#ifndef __MIDWARE_MSERIAL_H
#define	__MIDWARE_MSERIAL_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "MUtility.h"

#define S_ONSEND     0                             // OnSend state
#define S_WAITING    1                             // waiting state
#define S_SENDING    2                             // sending state
#define S_RECEIVE    3                             // receing state
#define S_ONRECV     4                             // receive over state


void USART6_OffReceive(void);                      // turn off receiver
void USART6_PreReceive(void);                      // prepare for receiving
void USART6_PreTransmit(void);                     // prepare for transmitting
void USART6_StartSend(void);                       // start to transmit
U8   USART6_GetByte(U8* pData);                    // get a byte from USAR6T-buffer
U8   USART6_Write(U8* pData, U8 len);              // write some bytes to USART6-buffer
U8   USART6_GetStatus(void);                       // get usart6 state

class CMSerial
{
public:
	CMSerial(void);
	~CMSerial(void);

	void DoLoop(U16 tmOnce);

public:
	virtual U16 OnNewSend()  {return 0;}           // FALSE:no data to send
	virtual U16 OnNewRecv()  {return 0;}           // FALSE:do not deal the received data

private:
	void OnSend();                                 // handle the data should be sent
	void Waiting();                                // wait a lillte interval between receive completion and send starting, data line output mode
	void Sending();                                // send one by one and check whether the end of transmitting 
	void Receiving(U16 tmOnce);                    // receive one by one and check whether the completion of receiving
	void OnReceive();                              // handle the received data

protected:
	U8  m_state;                                   // state
	U16 m_stateCnt;                                // time count of one state

	U8  m_rxBuf[256];                              // received data buffer
	U8  m_rxLen;                                   // received data size

	U8  m_txBuf[256];                              // transmit data buffer
	U8  m_txLen;                                   // transmit data size

	U16 m_idleCnt;                                 // timer count of interval between one char and one char
	U16 m_onesendCnt;															//timer count of send frequency

	U16 m_tmRxOver;                                // configured interval time
	U16 m_tmNoAck;                                 // configured no acknowledge time
	U16 m_tmWait;                                  // configured waiting time
};

#ifdef __cplusplus
}
#endif

#endif /* __MIDWARE_MSERIAL_H */
