#ifndef _XCAN_H
#define _XCAN_H

#ifdef __cplusplus
extern "C" {
#endif

void xcan_init(void);

size_t xcan_getStackUsage(void);
size_t xcan_getStackSize(void);

#ifdef __cplusplus
}
#endif

#endif
