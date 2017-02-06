#ifndef __MIDWARE_MUTILITY_H
#define	__MIDWARE_MUTILITY_H

#ifdef __cplusplus
	extern "C" {
#endif

#include <string.h>

typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
//typedef long long       S64;
//typedef unsigned long long U64;
typedef unsigned char   BIT;
typedef unsigned int    BOOL;

#ifndef NULL
	#define NULL      (void *) 0
#endif
  
#ifndef MAX
	#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
	#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define MAKEWORD(a, b)      ((U16)(((U8)(a)) | ((U16)((U8)(b))) << 8))
#define MAKELONG(a, b)      ((U32)(((U16)(a)) | ((U32)((U16)(b))) << 16))
#define LOWORD(l)           ((U16)(l))
#define HIWORD(l)           ((U16)(((U32)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((U8)(w))
#define HIBYTE(w)           ((U8)(((U16)(w) >> 8) & 0xFF))

U16 IDCOM_CRC16(const U8 *ptr,U16 len);            // generate crc16
U8  MakeBcc(U8 *data, U8 size);                    // generate bcc
U8  Hex2Asc(U8 *hex, U8 hexSize, U8 *szChar);      // encode BCD
U8  Asc2Hex(U8 *szChar, U8 charSize, U8 *hex);     // decode BCD

class CMQueue
{
public:
	CMQueue(U8 unit, U8 max);                      // TotalBytes=NodeSize*NodeNum
	~CMQueue(void);
	U8 Push(void* pNode);                          // add to tail
	U8 Pop(void* pNode);                           // remove from head
	void Clear(void);                              // clear all nodes
	U8 GetNum(void);                               // get node number

private:
	U8 m_head;                                     // head node pos of FIFO list
	U8 m_tail;                                     // tail node pos of FIFO list
	U8 m_num;                                      // node number of FIFO list

	U8  m_unit;                                    // the size of one node unit
	U8  m_max;                                     // node number maximum
	U8* m_buff;                                    // poiter of node list
};

#ifdef __cplusplus
}
#endif

#endif	/* __MIDWARE_MUTILITY_H */
