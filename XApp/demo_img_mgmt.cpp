/*
 * Copyright 2013 Ayla Networks, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Ayla Networks, Inc.
 */
//#include <ayla/mcu_platform.h>
#include <ctype.h>
#include <string.h>
//#include <ayla/ayla_proto_mcu.h>
//#include <ayla/props.h>
//#include <ayla/serial_msg.h>

//#include <ayla/crc.h>
//#include <flash_layout.h>
//#include <ayla/host_ota.h>
#include "ayla_xxx.h"
#include "flash_layout.h"
#include "demo_img_mgmt.h"
#include "XApp.h"
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#define FLASH_ERRFLAGS (FLASH_FLAG_RDERR | FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_EOP)

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */

/*
 * Pointer to version string for image in inactive slot.
 */
static char *inactive_vers;

/*
 * Pointer to end of flash area where progress bar is. This location
 * is set to current template version once it has been reported to cloud.
 */
static u16 *tmpl_registered = (u16 *)MCU_IMG_TMPL_VER_LOC;
u8 template_req;

static size_t image_size;
static size_t cur_off;
u8 boot2inactive;
//extern const char version[10];

static int sector_size[8] = {16,16,16,16,64,128,128,128};

static int mcu_inactive_erase(int force);
static int mcu_flash_block_erased(u32 off , int sector);
static int mcu_boot(char *ver, int ver_len);


/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
	else
	{
		sector = FLASH_SECTOR_7;
	}

  return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
static uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;

  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3))
  {
    sectorsize = 16 * 1024;
  }
  else if(Sector == FLASH_SECTOR_4)
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}

static void mcu_set_inactive_version(int set)
{
	struct image *img;

	if (set) {
		img = (struct image *)(MCU_IMG_INACTIVE + IMAGE_HDR_OFF);
		inactive_vers = (char *)img->i_vers;
	} else {
		inactive_vers = NULL;
	}
}

int send_inactive_version(struct prop *prop, void *arg)
{
	return prop_send(prop, inactive_vers,
	    inactive_vers ? strlen(inactive_vers) : 0, arg);
}

void set_boot2inactive(struct prop *prop, void *arg, void *valp, size_t len)
{
	u8 val = *(u8 *)valp;

	if (val) {
		mcu_boot(NULL, 0);
	}
}

/*
 * Make template version to be 'demo 0.9' from 'demo 0.9 .... ....'
 */
static void mcu_tmpl_ver(char *tmpl_ver, int *len)
{
	int i, cnt;

	for (i = 0, cnt = 0; i < *len - 1; i++) {
		tmpl_ver[i] = version[i];
		if (version[i] == ' ' && ++cnt > 1) {
			break;
		}
	}
	memset(&tmpl_ver[i], 0, *len - i);
	tmpl_ver[i] = '\0';
	*len = i;
}

int send_template_version(struct prop *prop, void *arg)
{
	char tmpl_ver[MCU_IMG_TMPL_MAXLEN];
	int len;

	len = sizeof(tmpl_ver);
	mcu_tmpl_ver(tmpl_ver, &len);
	return prop_send(prop, tmpl_ver, strlen(tmpl_ver), arg);
}

static void mcu_img_mgmt_set_boot_state(enum mcu_boot_state mbs)
{
	int need_erase;

	if (mbs != MCU_BOOT_OK && mbs != MCU_BOOT_INACTIVE) {
		/* invalid option. xXXXx */
		mbs = MCU_BOOT_OK;
	}
	if (!mcu_flash_block_erased(MCU_IMG_PROG_BAR, MCU_IMG_PROG_BAR_SECTOR)) {
		need_erase = 1;
	} else {
		need_erase = 0;
	}
	
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_ERRFLAGS);
	if (need_erase) {
		FLASH_Erase_Sector(MCU_IMG_PROG_BAR_SECTOR,VOLTAGE_RANGE_3);
	}
	if (mbs == MCU_BOOT_INACTIVE) {
		HAL_FLASH_Program(TYPEPROGRAM_HALFWORD, MCU_IMG_PROG_BAR, 0);
	}
	HAL_FLASH_Lock();
}

static enum mcu_boot_state mcu_img_mgmt_get_boot_state(void)
{
	if (MCU_PBAR_ISSET(MCU_PBAR_OFF_FALLBACK)) {
		return MCU_BOOT_FALLBACK;
	}
	if (MCU_PBAR_ISSET(MCU_PBAR_OFF_FALLBACK_START)) {
		return MCU_BOOT_FALLBACK_START;
	}
	if (MCU_PBAR_ISSET(MCU_PBAR_OFF_TEST)) {
		return MCU_BOOT_TEST;
	}
	if (MCU_PBAR_ISSET(MCU_PBAR_OFF_INACTIVE)) {
		return MCU_BOOT_INACTIVE;
	}
	return MCU_BOOT_OK;
}

/*
 * Decide whether to send MCU OTA upgrade go-ahead.
 */
static void mcu_img_upgrade_check(char *ver, int ver_len, size_t img_sz)
{
	int rc = 0;

	if (img_sz > MCU_IMG_MAX_SZ) {
		/*
		 * Too big of an image.
		 */
		host_ota_stat(AERR_LEN_ERR);
		return;
	}
	image_size = img_sz;
	cur_off = 0;

	if (ver) {
		if (ver_len == strlen(version) &&
		    !memcmp(ver, version, ver_len)) {
			host_ota_stat(AERR_ALREADY);
			return;
		}
	}
	/*
	 * Erase the inactive image slot if needed.
	 */
	rc = mcu_inactive_erase(0);
	if (rc) {
		host_ota_stat(AERR_INTERNAL);
		return;
	}
	host_ota_go();
}

static int mcu_img_crc_check(u32 loc)
{
	struct image *hdr;
	u16 crc;
	int tmp;
	size_t off;

	hdr = (struct image *)(loc + IMAGE_HDR_OFF);
	if (hdr->i_magic != htonl(IMAGE_MAGIC)) {
		return AERR_INVAL_TLV;
	}
	if (htonl(hdr->i_len) >= MCU_IMG_MAX_SZ) {
		return AERR_LEN_ERR;
	}
	crc = crc16((void *)loc, IMAGE_HDR_CRC_OFF, CRC16_INIT);
	tmp = 0;
	crc = crc16(&tmp, sizeof(hdr->i_crc), crc);
	tmp = htonl(hdr->i_len) - IMAGE_HDR_CRC_OFF - sizeof(u16);
	off = loc + IMAGE_HDR_CRC_OFF + sizeof(u16);
	crc = crc16((void *)off, tmp, crc);

	if (ntohs(hdr->i_crc) != crc) {
		return AERR_CHECKSUM;
	}
	return 0;
}

/*
 * Version strings can either be direct match:
 * "demo 0.9" vs "demo 0.9"
 * or with a whitespace after flash image version
 * "demo 0.9" vs "demo 0.9 02/22/2013 08:11:21 PST marko"
 * but not another digit
 * "demo 0.9" vs "demo 0.91 02/22/2013 08:11:21 PST marko"
 */
static int mcu_find_ver(char *ver, int ver_len, u32 loc)
{
	struct image *hdr;

	ver_len = min(ver_len, sizeof(hdr->i_vers) - 1);

	hdr = (struct image *)(loc + IMAGE_HDR_OFF);
	if (!memcmp(hdr->i_vers, ver, ver_len) &&
	    (hdr->i_vers[ver_len] == '\0' || isspace(hdr->i_vers[ver_len]))) {
		/*
		 * Version string match. Verify integrity of the image then.
		 */
		return mcu_img_crc_check(loc);
	}
	return AERR_INVAL_NAME;
}


static int mcu_flash_block_erased(u32 off , int sector)
{
	u32 end;

	end = off + sector_size[sector] * 1024;
	for (; off < end; off += sizeof(u32)) {
		if (*(u32 *)off != 0xffffffff) {
			return 0;
		}
	}
	return 1;
}

static int mcu_inactive_erase(int force)
{
	u32 off;

	mcu_set_inactive_version(0);
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_ERRFLAGS);
	for (off = MCU_IMG_INACTIVE; off < MCU_IMG_SCRATCH;
	     off += MCU_IMG_BLK_SZ) {
		if (force || !mcu_flash_block_erased(off, MCU_IMG_INACTIVE_SECTOR)) {
			FLASH_Erase_Sector(MCU_IMG_INACTIVE_SECTOR,VOLTAGE_RANGE_3);
			if (!force) {
				HAL_FLASH_Lock();
				return -1;
			}
		}
	}
	HAL_FLASH_Lock();
	return 0;
}

/*
 * We should be getting image in blocks (max 255 bytes at a time).
 * STM32F1 doesn't allow to program a flash location twice, and
 * we're doing programming 32 bits at a time. So we'll need to hold
 * on to few bytes when crossing a block boundary, and program
 * all in one go.
 */
static void mcu_img_load(size_t off, void *data, size_t len)
{
	static u32 tmp;
	u32 blen = 0, xtra;
	size_t rem;
	u8 *cp;
	int rc;

	if (off != cur_off) {
		host_ota_stat(AERR_INVAL_OFF);
		return;
	}
	cp = (u8 *)data;
	rem = len;
	xtra = off % sizeof(u32);
	HAL_FLASH_Unlock();
	if (xtra) {
		/*
		 * Bytes stashed from previous round, not written to
		 * flash yet.
		 */
		memcpy((((u8 *)&tmp) + xtra), data, sizeof(u32) - xtra);
		rc = HAL_FLASH_Program(TYPEPROGRAM_WORD, MCU_IMG_INACTIVE + off - xtra, tmp);
		if (rc != HAL_OK) {
			host_ota_stat(AERR_INTERNAL);
			goto out;
		}
		xtra = sizeof(u32) - xtra;
		rem -= xtra;
		cp += xtra;
		off += xtra;
	}
	while (rem) {
		blen = min(sizeof(u32), rem);
		tmp = 0xffffffff;
		memcpy(&tmp, cp, blen);
		if (blen < sizeof(u32)) {
			break;
		}
		rc = HAL_FLASH_Program(TYPEPROGRAM_WORD, MCU_IMG_INACTIVE + off, tmp);
		if (rc != HAL_OK) {
			host_ota_stat(AERR_INTERNAL);
			goto out;
		}
		rem -= blen;
		cp += blen;
		off += blen;
	}
	cur_off += len;
	if (cur_off == image_size) {
		if (blen) {
			/*
			 * Program the remaining data.
			 */
			rc = HAL_FLASH_Program(TYPEPROGRAM_WORD, MCU_IMG_INACTIVE + off, tmp);
			if (rc != HAL_OK) {
				host_ota_stat(AERR_INTERNAL);
				goto out;
			}
		}
		/*
		 * Done. Check integrity of the image then, and report it.
		 */
		rc = mcu_img_crc_check(MCU_IMG_INACTIVE);
		host_ota_stat(rc);
		if (rc == 0) {
			mcu_set_inactive_version(1);
		}
	}
out:
	HAL_FLASH_Lock();
}

static int mcu_boot(char *ver, int ver_len)
{
	int rc;

	if (ver) {
		/*
		 * Find the version.
		 */
		rc = mcu_find_ver(ver, ver_len, MCU_IMG_ACTIVE);
		if (rc == 0) {
			/*
			 * We're already running it.
			 */
			return AERR_ALREADY;
		}
		rc = mcu_find_ver(ver, ver_len, MCU_IMG_INACTIVE);
		if (rc) {
			return rc;
		}
	} else {
		/*
		 * Boot to image in inactive slot, if it is valid.
		 */
		rc = mcu_img_crc_check(MCU_IMG_INACTIVE);
		if (rc) {
			return rc;
		}
	}
	mcu_img_mgmt_set_boot_state(MCU_BOOT_INACTIVE);

	HAL_NVIC_DisableIRQ(SysTick_IRQn);
	HAL_NVIC_ClearPendingIRQ(SysTick_IRQn);
	
	HAL_NVIC_SystemReset();
//	IWDG_InitEx();
//	while(1);
	return 0;
}

/*
 * Update what we've sent as our template version number.
 *
 * This is in the same area as the progress bar, so it will get cleared
 * if update is done with OTA. The flash block will not have been
 * erased, however, if active image has been replaced with JTAG. In that
 * case we might need to wipe this flash block (if the template version
 * had changed).
 */
void template_version_sent(void)
{
	u32 foff;
	char tmpl_ver[MCU_IMG_TMPL_MAXLEN];
	int len, need_erase;

	len = sizeof(tmpl_ver);
	mcu_tmpl_ver(tmpl_ver, &len);

	template_req = 0;
	if (!memcmp(tmpl_ver, tmpl_registered, MCU_IMG_TMPL_MAXLEN)) {
		return;
	}
	if (!mcu_flash_block_erased(MCU_IMG_PROG_BAR, MCU_IMG_PROG_BAR_SECTOR)) {
		need_erase = 1;
	} else {
		need_erase = 0;
	}
	HAL_FLASH_Unlock();
	if (need_erase) {
		FLASH_Erase_Sector(MCU_IMG_PROG_BAR_SECTOR,VOLTAGE_RANGE_3);
	}
	foff = (u32)tmpl_registered;
	for (len = 0; len < sizeof(tmpl_ver); len += sizeof(u32)) {
		HAL_FLASH_Program(TYPEPROGRAM_WORD, foff + len, *(u32 *)&tmpl_ver[len]);
	}
	HAL_FLASH_Lock();
}

void mcu_img_mgmt_init(void)
{
	enum mcu_boot_state mbs;
	char tmpl_ver[MCU_IMG_TMPL_MAXLEN];
	int len;

	host_ota_start_cb = mcu_img_upgrade_check;
	host_ota_load_cb = mcu_img_load;
	host_ota_boot_cb = mcu_boot;

	len = sizeof(tmpl_ver);
	mcu_tmpl_ver(tmpl_ver, &len);

	if (mcu_img_crc_check(MCU_IMG_INACTIVE) == 0) {
		mcu_set_inactive_version(1);
	}

	mbs = mcu_img_mgmt_get_boot_state();
	if (mbs == MCU_BOOT_TEST) {
		/*
		 * Success.
		 */
		mcu_img_mgmt_set_boot_state(MCU_BOOT_OK);
		host_ota_stat(0);
	} else if (mbs == MCU_BOOT_FALLBACK || mbs == MCU_BOOT_INACTIVE) {
		/*
		 * We tried booting, but didn't get this far.
		 *
		 * MCU_BOOT_INACTIVE shouldn't happen, unless if there was a
		 * malfunction in the bootloader. 
		 */
		mcu_img_mgmt_set_boot_state(MCU_BOOT_OK);
		host_ota_stat(AERR_BOOT);
	}
	if (memcmp(tmpl_ver, tmpl_registered, MCU_IMG_TMPL_MAXLEN)) {
		/*
		 * Need to report version number of template.
		 * We don't need to publish this to local clients, but
		 * ADS should be told.
		 */
		prop_lookup("oem_host_version")->send_mask = ADS_BIT;
		template_req = 1;
	}
}
