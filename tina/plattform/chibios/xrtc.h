#ifndef _XRTC_H
#define _XRTC_H
#ifdef USE_CRASHRECOVERY

#include <time.h>

time_t rtc_getunixtime(void);

#endif // USE_CRASHRECOVERY
#endif // _XRTC_H
