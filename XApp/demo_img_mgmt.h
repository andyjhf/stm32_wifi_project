#ifndef __AYLA_DEMO_IMG_H__
#define __AYLA_DEMO_IMG_H__

#ifdef __cplusplus
	extern "C" {
#endif
//#include "XAppDef.h"
#ifdef DEMO_IMG_MGMT
extern u8 boot2inactive;
extern u8 template_req;
void mcu_img_mgmt_init(void);
int send_inactive_version(struct prop *, void *arg);
void set_boot2inactive(struct prop *, void *arg, void *valp, size_t len);
int send_template_version(struct prop *, void *arg);
void template_version_sent(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __APP_XTASKCOMVRF_H */
