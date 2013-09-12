#ifndef _XADC_H
#define _XADC_H

#ifdef __cplusplus
extern "C" {
#endif

void xadc_init(void);
uint16_t adc_get_mon7v(void);

size_t xadc_getStackUsage(void);
size_t xadc_getStackSize(void);

#ifdef __cplusplus
}
#endif

#endif
