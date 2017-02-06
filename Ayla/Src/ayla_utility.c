#include "ayla_utility.h"

/*********************************************************************************************************/
/* MCU platform system */
u32 ayla_tick(void)
{
	return HAL_GetTickX10();
}

/*********************************************************************************************************/
/* XXX assumes little-endian system */
be16 htons(short x)
{
    return (be16)((x << 8) | ((x >> 8) & 0xff));
}

u16 ntohs(be16 x)
{
    return ((u16)x << 8) | (((u16)x >> 8) & 0xff);
}

be32 htonl(u32 x)
{
    return (be32)((x << 24) |
        ((x << 8) & 0xff0000) |
        ((x >> 8) & 0xff00) |
        ((x >> 24) & 0xff));
}

u32 ntohl(be32 x)
{
     return (u32)(((u32)x << 24) |
        (((u32)x << 8) & 0xff0000) |
        (((u32)x >> 8) & 0xff00) |
        (((u32)x >> 24) & 0xff));
}
/*********************************************************************************************************/
/*
 * Compare two times or sequence numbers with wrap-around.
 */
int cmp_gt(u32 a, u32 b)
{
	return (int)(a - b) > 0;
}

/*********************************************************************************************************/

/*
 * Put up to 4 UTF-8 bytes to a buffer from a int value up to 0x10ffff.
 *
 * 4-byte UTF-8 can handle up to 21-bit values, but Unicode further restricts
 * this to values up to 0x10ffff.
 */
int utf8_put_wchar(u8 *bp, u32 val)
{
	if (val > 0x10ffff) {
		return -1;
	}
	if (val >= 0x10000) {
		/* 21 bits */
		bp[0] = 0xf0 + ((val >> 18) & 7);
		bp[1] = 0x80 + ((val >> 12) & 0x3f);
		bp[2] = 0x80 + ((val >> 6) & 0x3f);
		bp[3] = 0x80 + (val & 0x3f);
		return 4;
	}
	if (val >= 0x800) {
		/* 16 bits */
		bp[0] = 0xe0 + ((val >> 12) & 0xf);
		bp[1] = 0x80 + ((val >> 6) & 0x3f);
		bp[2] = 0x80 + (val & 0x3f);
		return 3;
	}
	if (val >= 0x80) {
		/* 11 bits */
		bp[0] = 0xc0 + ((val >> 6) & 0x1f);
		bp[1] = 0x80 + (val & 0x3f);
		return 2;
	}
	/* 7 bits */
	bp[0] = val;
	return 1;
}

/*
 * Get next UTF-8 token from a buffer.
 * Returns length, or zero if invalid UTF-8 code encountered.
 */
size_t utf8_get(u32 *result, u8 *buf, size_t len)
{
	u32 val = 0;
	size_t rlen = 0;
	u8 test[4];
	int i;
	u8 c;

	if (len == 0) {
		return 0;
	}
	c = buf[0];
	if (c < 0x80) {
		*result = c;
		return 1;
	}
	if ((c & 0xf8) == 0xf0) {
		val = c & 7;
		rlen = 4;
	} else if ((c & 0xf0) == 0xe0) {
		val = c & 0xf;
		rlen = 3;
	} else if ((c & 0xe0) == 0xc0) {
		if (c == 0xc0 || c == 0xc1) {
			return 0;
		}
		val = c & 0x1f;
		rlen = 2;
	} else if ((c & 0xc0) == 0x80) {
		return 0;
	}
	if (len < rlen) {
		return 0;
	}
	for (i = 1; i < rlen; i++) {
		c = buf[i];
		if ((c & 0xc0) != 0x80) {
			return 0;
		}
		val = (val << 6) | (c & 0x3f);
	}

	/*
	 * Check for over-long coding, which is invalid.
	 */
	if (utf8_put_wchar(test, val) != rlen) {
		return 0;
	}
	*result = val;
	return rlen;
}

/*
 * Get multiple UTF-8 wide characters from a buffer.
 * Returns the number of characters put in result, or -1 on error.
 */
int utf8_gets(u32 *result, int rcount, u8 *buf, size_t len)
{
	int tlen;
	int count;

	for (count = 0; count < rcount && len > 0; count++) {
		tlen = utf8_get(&result[count], buf, len);
		if (tlen <= 0) {
			return -1;
		}
		len -= tlen;
		buf += tlen;
	}
	if (len > 0) {
		return -1;
	}
	return count;
}

/*********************************************************************************************************/

/*
 * Assuming little-endian (or unknown) for now.
 */
 
void put_ua_be32(void *dest, u32 val)
{
	u8 *byte = dest;

	byte[0] = val >> 24;
	byte[1] = val >> 16;
	byte[2] = val >> 8;
	byte[3] = val;
}

void put_ua_be16(void *dest, u16 val)
{
	u8 *byte = dest;

	byte[0] = val >> 8;
	byte[1] = val;
}

u32 get_ua_be32(void *src)
{
	u8 *byte = src;

	return ((u32)byte[0] << 24) | ((u32)byte[1] << 16) |
	    ((u32)byte[2] << 8) | byte[3];
}

u16 get_ua_be16(void *src)
{
	u8 *byte = src;

	return ((u16)byte[0] << 8) | byte[1];
}

int get_ua_with_len(void *src, u8 len, u32 *dest)
{
	switch (len) {
	case 1:
		*dest = *(u8 *)src;
		break;
	case 2:
		*dest = get_ua_be16(src);
		break;
	case 4:
		*dest = get_ua_be32(src);
		break;
	default:
		return -1;
	}
	return 0;
}

/*********************************************************************************************************/

static const u16 crc16_table[16] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

/*
 * Compute CRC-8 with IEEE polynomial
 * LSB-first.  Use 4-bit table.
 */
u16 crc16(const void *buf, size_t len, u16 crc)
{
	const u8 *bp = buf;

	while (len-- > 0) {
		crc ^= *bp++ << 8;
		crc = (crc << 4) ^ crc16_table[crc >> 12];
		crc = (crc << 4) ^ crc16_table[crc >> 12];
	}
	return crc;
}
