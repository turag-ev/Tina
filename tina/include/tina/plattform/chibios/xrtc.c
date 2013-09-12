#ifdef USE_CRASHRECOVERY

#include <ch.h>
#include <hal.h>
#include <time.h>
#include <chrtclib.h>
#include "xrtc.h"

time_t rtc_getunixtime()
{
    return rtcGetTimeUnixSec(&RTCD1);
}

#endif // USE_CRASHRECOVERY
