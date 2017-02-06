#ifndef __APP_XTASKCOMVRF_H
#define	__APP_XTASKCOMVRF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "MUtility.h"
#include "MSerial.h"

#define CMD_S1_20    0x20                          // Poll1 Poll IDU
#define CMD_S2_21    0x21                          // Poll2 Ctrl IDU
#define CMD_S3_22    0x22                          // Poll3 Poll PC
#define CMD_S4_23    0x23                          // Poll4 Send Para
#define CMD_SX_30    0x30                          // (NA)

#define CMD_B1_10    0x10                          // Broad1 EXV opening of Cool/Heat starting and the minmum 
#define CMD_B2_11    0x11                          // Broad2 EXV opening of bleed and oil return
#define CMD_B3_24    0x24                          // Broad3 ODU data
#define CMD_B4_12    0x12                          // Broad4 BAS ctrl command

#define CMD_R1_80    0x80                          // Reply1 IDU data1(main)
#define CMD_R2_81    0x81                          // Reply2 Set ODU clock
#define CMD_R3_83    0x83                          // Reply3 Id/PC request(R&W para,PC-Online,CtrlCmd,Address,Force)
#define CMD_R4_88    0x88                          // Reply4 IDU data2
#define CMD_R5_89    0x89                          // Reply5 IDU data3(RT,TT...)

#define CMD_A1_5A    0x5A                          // Addr1:starting to assign
#define CMD_A2_5B    0x5B                          // Addr2:check the IDU-address(FF) or not 
#define CMD_A2_A5    0xA5                          // Addr2:ACK of check
#define CMD_A3_5C    0x5C                          // Addr3:Give IDU an address
#define CMD_A3_A6    0xA6                          // Addr3:Ack of Give

#define CMD_FCT_55   0x55                          // fct test request
#define CMD_FCT_AA   0xAA                          // fct test reply

#define F_STX        0                             // start flag
#define F_CMD        1                             // command type
#define F_DST        2                             // destination address
#define F_SRC        3                             // source address
#define F_D0         4                             // data-0
#define F_D1         5                             // data-1
#define F_D2         6                             // data-2
#define F_D3         7                             // data-3
#define F_D4         8                             // data-4
#define F_D5         9                             // data-5
#define F_D6         10                            // data-6
#define F_D7         11                            // data-7
#define F_D8         12                            // data-8
#define F_D9         13                            // data-9
#define F_D10        14                            // data-10
#define F_D11        15                            // data-11
#define F_D12        16                            // data-12
#define F_D13        17                            // data-13
#define F_D14        18                            // data-14
#define F_D15        19                            // data-15
#define F_CRH        20                            // CRC16 hibyte
#define F_CRL        21                            // CRC16 lobyte
#define F_ETX        22                            // end flag
#define F_LEN        23                            // normal format length

#define XRMT_A       1                             // type A of remote control command 
#define XRMT_B       2                             // type B of remote control command 

typedef struct tagXRemote{                         // struct of remote control command 
	U8  Type;                                      // prop remote cmd type
	U8  Cmd[5];                                    // prop remote cmd data
	U8  Group[8];                                  // prop remote group bits
}XRemote;

typedef struct tagXCtrlCmd{                        // struct of VRF control command 
	U8  Type;                                      // vrf cmd type
	U8  Group[8];                                  // vrf group bit
	U8  Data[4];                                   // vrf cmd data
}XCtrlCmd;


//typedef union {
//	U8 BYTE;
//	struct {         /* little endian	*/
//		U8 B0:1;       /* b0: */
//		U8 B1:1;       /* b1: */
//		U8 LOUVER:1;   /* b2: */
//		U8 LOCK:1;		 /* b3:	*/
//		U8 FAN:1;      /* b4: */
//		U8 MODE:1;     /* b5: */
//		U8 TEMP:1;     /* b6: */
//		U8 ONOFF:1;    /* b7: */
//	} BIT;
//} SET_MASK;

//typedef union {
//	U8 BYTE;
//	struct {         /* little endian	*/
//		U8 Mode:3;     /* b2-b0: */
//		U8 Temp:4;     /* b6-b3: */
//		U8 OnOff:1;    /* b7: */
//	} BIT;
//} SET_DATA1;

//typedef union {
//	U8 BYTE;
//	struct {         /* little endian	*/
//		U8 b0:1;       /* b0: */
//		U8 b1:1;       /* b1: */
//		U8 b2:1;       /* b2: */
//		U8 Louver:1;   /* b3: */
//		U8 Lock:1;     /* b4: */
//		U8 Fan:3;      /* b7-b5: */
//	} BIT;
//} SET_DATA2;


class CXTaskComVRF: public CMSerial
{
public:
	CXTaskComVRF(void);
	~CXTaskComVRF(void);

public:
	void InitTask(void);
	void ParseRemote(U8 *szRemote, U8 size);

	virtual U16 OnNewSend();
	virtual U16 OnNewRecv();

private:
	U8 checkFrame(void);
	void buildBroad_12(XCtrlCmd *pCmd);
	void buildReply_83(U8 Data0, U8 *pData);

	void onError(void);
	void onOdPoll_20(void);
	void onIdReply_80(void);
	void onOdBroad_24(void);
	U8   onOdPoll_22(void);

	void onPcFct_55(void);
	void buildFct_AA(void);

private:
	U8 m_recvCmd;
	U8 m_recvSrc;
	U8 m_Data14;

	tagXCtrlCmd   m_ctrlCmd;
	CMQueue *m_queue;

	U16 m_errCnt;
	U8  m_dipAddr;
	U8  m_idNum;
	U8  m_idLive[8];

	U8  m_odDataNoCnt;

};

#ifdef __cplusplus
}
#endif
#endif /* __APP_XTASKCOMVRF_H */
