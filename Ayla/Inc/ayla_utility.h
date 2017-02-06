#ifndef __AYLA_UTILITY_H__
#define __AYLA_UTILITY_H__

#ifdef __cplusplus
	extern "C" {
#endif

#include <string.h>
#include <stdarg.h>

#include "stm32f4xx_it.h"

#define AYLA_VER_NAME "oem_host_version"   // dash board template version
#define AYLA_TICK_HZ  100                  // 10ms
#define AYLA_YEAR_MAX (3153600000U)        // 365*24*3600*100 based 10ms
		
//#ifndef HAVE_UTYPES
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned long	u32;
//#if 0
//typedef unsigned long long u64;
//#endif

typedef signed char	s8;
typedef short		s16;
typedef long	    s32;
//#if 0
//typedef long long	s64;
//#endif
//#endif

typedef u16		be16;
typedef u32		be32;
//#if 0
//typedef u64		be64;
//#endif 

#define MAX_U8		0xffU
#define	MAX_U16		0xffffU
#define MAX_U32		0xffffffffU

#define MAX_S8		0x7f
#define MAX_S16		0x7fff
#define MAX_S32		0x7fffffff

#define MIN_S8		((s8)0x80)
#define MIN_S16		((s16)0x8000)
#define MIN_S32		((s32)0x80000000)

#ifndef PACKED
#define PACKED __attribute__((__packed__))
#endif /* PACKED */

#ifndef ARRAY_LEN
#define ARRAY_LEN(x) (sizeof(x) / sizeof(*(x)))
#endif /* ARRAY_LEN */

#define TSTAMP_CMP(cmp, a, b)   ((s32)((a) - (b)) cmp 0)

#define TSTAMP_LT(a, b)		((s32)((a) - (b)) < 0)
#define TSTAMP_GT(a, b)		((s32)((a) - (b)) > 0)
#define TSTAMP_LEQ(a, b)	((s32)((a) - (b)) <= 0)
#define TSTAMP_GEQ(a, b)	((s32)((a) - (b)) >= 0)

#ifndef ABS
#define	ABS(a)			(((s32)(a) < 0) ? -(a) : (a))
#endif /* ABS */


/*********************************************************************************************************/
/* XXX assumes little-endian system */
be16 htons(short x);
u16 ntohs(be16 x);
be32 htonl(u32 x);
u32 ntohl(be32 x);
/*********************************************************************************************************/
int cmp_gt(u32 a, u32 b);

/*********************************************************************************************************/
int utf8_gets(u32 *result, int rcount, u8 *buf, size_t len);
int utf8_put_wchar(u8 *bp, u32 val);
size_t utf8_get(u32 *result, u8 *buf, size_t len);
/*********************************************************************************************************/
/* Assuming little-endian (or unknown) for now. */
void put_ua_be32(void *dest, u32 val);
void put_ua_be16(void *dest, u16 val);
u32 get_ua_be32(void *src);
u16 get_ua_be16(void *src);
int get_ua_with_len(void *src, u8 len, u32 *dest);
/*********************************************************************************************************/
#define	CRC16_INIT	0xffffU
u16 crc16(const void *buf, size_t len, u16 init_crc);

/*********************************************************************************************************/

u32 ayla_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* __AYLA_UTILITY_H__ */
