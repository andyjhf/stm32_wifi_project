/*
 * Copyright 2011-2012 Ayla Networks, Inc.  All rights reserved.
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

#include "stm32f4xx_hal.h"
#include "flash_layout.h"
#include "stm32f4xx.h"

#define VERSION "1.0"


static int img_mgmt_mark_progress(int off);

#ifdef AYLA_KEIL
extern void bb_enter_prog(void *addr);
#endif

typedef enum 
{
	LED2 = 0,
	LED3 = 1
} LED_TypeDef;

#define LEDn                       2
#define LED1_PIN                   GPIO_PIN_15             // LED-2
#define LED1_PORT                  GPIOB
#define LED1_CLK                   RCC_AHB1ENR_GPIOBEN
#define LED2_PIN                   GPIO_PIN_14             // LED-3
#define LED2_PORT                  GPIOB
#define LED2_CLK                   RCC_AHB1ENR_GPIOBEN

static GPIO_TypeDef*  const GPIO_LED_PORT[LEDn] = {LED1_PORT,LED2_PORT};
static uint16_t       const GPIO_LED_PIN[LEDn]  = {LED1_PIN, LED2_PIN};
static uint32_t       const GPIO_LED_CLK[LEDn]  = {LED1_CLK, LED2_CLK};
static uint8_t        GPIO_LED_STATUS[LEDn]     = {0,0};

void LED_On(LED_TypeDef led);
void LED_Off(LED_TypeDef led);

void LED_Init(LED_TypeDef led)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	// Check the parameters
  assert_param((led<LEDn));
	
	// GPIO_LED Peripheral Clock enable
	RCC->AHB1ENR |= (GPIO_LED_CLK[led]);

	// Configure the GPIO_LED pin
	GPIO_InitStructure.Pin   = GPIO_LED_PIN[led];
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Speed  = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIO_LED_PORT[led], &GPIO_InitStructure);
	
	LED_Off(led);
}

void LED_On(LED_TypeDef led)
{
	// Check the parameters
  assert_param((led<LEDn));
	
	// reset the corresponding ODx bit for led light on	
	HAL_GPIO_WritePin(GPIO_LED_PORT[led], GPIO_LED_PIN[led], GPIO_PIN_RESET);
	
	GPIO_LED_STATUS[led] = 1;
}

void LED_Off(LED_TypeDef led)
{
	// Check the parameters
  assert_param((led<LEDn));

	// set the corresponding ODRx bit for led light off
	HAL_GPIO_WritePin(GPIO_LED_PORT[led], GPIO_LED_PIN[led], GPIO_PIN_SET);
	
	GPIO_LED_STATUS[led] = 0;
}

static void boot(void)
{
#ifdef AYLA_KEIL
	bb_enter_prog((void *)MCU_IMG_ACTIVE);
#elif defined(AYLA_GCC)
	u32 addr = *((u32 *)MCU_IMG_ACTIVE);

	/*
	 * Set stack pointer.
	 */
	__asm__( "mov sp, %0"
	    :
	    : "r" (addr));

	/*
	 * Jump to newly loaded image. Offset 4 contains the address of the
	 * reset handler from the vector table.
	 */
	void (**func)(void) = (void (**)(void))(MCU_IMG_ACTIVE + 4);
	(*func)();
#else
#error loader not supported by this compiler
#endif
}

static void img_mgmt_set_boot_state(enum mcu_boot_state mbs)
{
	int off;

	HAL_FLASH_Unlock();
	switch (mbs) {
	case MCU_BOOT_TEST:
		off = MCU_PBAR_OFF_TEST;
		break;
	case MCU_BOOT_FALLBACK_START:
		off = MCU_PBAR_OFF_FALLBACK_START;
		break;
	case MCU_BOOT_FALLBACK:
		off = MCU_PBAR_OFF_FALLBACK;
		break;
	default:
		/* xXXXx */
		off = MCU_PBAR_OFF_FALLBACK;;
		break;
	}
	img_mgmt_mark_progress(off);
	HAL_FLASH_Lock();
}

static enum mcu_boot_state img_mgmt_get_boot_state(void)
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

static int img_mgmt_progress(void)
{
	u16 *ptr;
	int i;

	ptr = (u16 *)MCU_IMG_PROG_BAR;
	for (i = 0; i <= MCU_PBAR_OFF_MAX; i++) {
		if (ptr[i] == 0xffff) {
			break;
		}
	}
	return i;
}

static int img_mgmt_mark_progress(int off)
{
	int rc;

	rc = HAL_FLASH_Program(TYPEPROGRAM_HALFWORD, MCU_IMG_PROG_BAR + off * sizeof(u16), 0);
	return rc;
}

static int img_copy_block(u32 src, u32 dst)
{
	int i, rc;
	uint16_t val;

	for (i = 0; i < MCU_IMG_SCRATCH_SZ / sizeof(u16); i++) {
		val = ((u16 *)src)[i];
		rc = HAL_FLASH_Program(TYPEPROGRAM_HALFWORD, dst + i * sizeof(u16), val);
	}
	return rc;
}

static void img_mgmt_erase_block(u32 start)
{
	int i;

//	for (i = 0; i < MCU_IMG_SCRATCH_SZ / MCU_IMG_BLK_SZ; i++) {
//		FLASH_ErasePage(start + i * MCU_IMG_BLK_SZ);
		FLASH_Erase_Sector(start,VOLTAGE_RANGE_3);
//	}
}

static void img_mgmt_swap(enum mcu_boot_state state)
{
	int i, j, step;
	u32 src, dst, scratch;

	/*
	 * Swap is done 4 pages at a time.
	 *
	 * - erase scratch area
	 * - copy from source to scratch
	 * - mark progress
	 * - erase source
	 * - copy from dest to source
	 * - mark progress
	 * - erase dest
	 * - copy from scratch to dest
	 * - mark progress
	 *
	 * Copy progress is stored as an array of 16-bit values.
	 * 
	 */
	i = img_mgmt_progress();
	if (state == MCU_BOOT_INACTIVE) {
		j = i - 1;
	} else if (state == MCU_BOOT_FALLBACK_START) {
		j = i - (MCU_PBAR_OFF_FALLBACK_START + 1);
	} else {
		/* xXXXx */
		return;
	}
	step = j % 3;
	src = MCU_IMG_INACTIVE + (j / 3) * MCU_IMG_BLK_SZ;
	dst = MCU_IMG_ACTIVE + (j / 3) * MCU_IMG_BLK_SZ;
	scratch = MCU_IMG_SCRATCH;

	/*
	 * Copying. If not finished with a page, jump to middle of the loop.
	 */
	HAL_FLASH_Unlock();
	if (step == 0) {
		goto step0;
	} else if (step == 1) {
		goto step1;
	} else if (step == 2) {
		goto step2;
	}

	while (src < MCU_IMG_INACTIVE + MCU_IMG_MAX_SZ) {
	step0:
		LED_On(LED2);
		img_mgmt_erase_block(MCU_IMG_SCRATCH_SECTOR);
		LED_Off(LED2);
		img_copy_block(src, scratch);
		LED_On(LED2);
		img_mgmt_mark_progress(i++);
	step1:
		img_mgmt_erase_block(MCU_IMG_INACTIVE_SECTOR);
		LED_Off(LED2);
		img_copy_block(dst, src);
		LED_On(LED2);
		img_mgmt_mark_progress(i++);
	step2:
		img_mgmt_erase_block(MCU_IMG_ACTIVE_SECTOR);
		LED_Off(LED2);
		img_copy_block(scratch, dst);
		LED_On(LED2);
		img_mgmt_mark_progress(i++);
		LED_Off(LED2);

		src += MCU_IMG_SCRATCH_SZ;
		dst += MCU_IMG_SCRATCH_SZ;
	}
	HAL_FLASH_Lock();
}

int main(int argc, char **argv)
{
	enum mcu_boot_state state;

	LED_Init(LED2);                                // config LED2 pin
	LED_Init(LED3);                                // config LED3 pin
	LED_On(LED3);
	state = img_mgmt_get_boot_state();
	switch (state) {
	case MCU_BOOT_OK:
	case MCU_BOOT_FALLBACK:
		break;
	case MCU_BOOT_INACTIVE:
		/*
		 * Boot to image in inactive slot.
		 */
		LED_On(LED2);
		img_mgmt_swap(state);
		img_mgmt_set_boot_state(MCU_BOOT_TEST);
		break;
	case MCU_BOOT_TEST:
		/*
		 * Boot failed! Swap the old image back.
		 */
		img_mgmt_set_boot_state(MCU_BOOT_FALLBACK_START);
		state = MCU_BOOT_FALLBACK_START;
		/* fall-through */
	case MCU_BOOT_FALLBACK_START:
		/*
		 * Boot to image in inactive slot.
		 */
		img_mgmt_swap(state);
		img_mgmt_set_boot_state(MCU_BOOT_FALLBACK);
		break;
	}
	LED_Off(LED3);
	boot();
	/* never reached */
	return 0;
}
