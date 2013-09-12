#ifndef _CRASHRECOVERY_H_
#define _CRASHRECOVERY_H_
#ifdef USE_CRASHRECOVERY

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <chtypes.h>

// use this to declare crash recovery info
#define CRASHRECOVERY_SECTION   __attribute__((section (".crashrecovery")))

extern bool crashrecovery_auto_restart;

typedef struct{
    time_t t_startup;
    time_t t_lastalive;
    time_t t_matchstart;
    bool match_started;

    bool crashed;
    uint32_t crash_reason;
    bool write_in_progress;
    uint32_t checksum;
} _CrashRecovery_Info_t;

extern volatile _CrashRecovery_Info_t crashrecovery_info[2];
extern _CrashRecovery_Info_t *crashrecovery_valid;

void crashrecovery_init(void);
bool crashrecovery_get_crashed(void);
void crashrecovery_clear(void);
void crashrecovery_write(void);
void crashrecovery_dump(void);

/**
 * @brief   call to validate crashrecovery_valid pointer
 */
static inline bool crashrecovery_valid_validptr(void) {
    if (!crashrecovery_valid) {
        return false;
    }
    if (crashrecovery_valid != &crashrecovery_info[0] && crashrecovery_valid != &crashrecovery_info[1]) {
        return false;
    }
    return true;
}

/**
 * @brief   call this from exception handlers
 */
static inline void crashrecovery_set_crashed(uint32_t reason) {
    crashrecovery_info[0].crashed = true;
    crashrecovery_info[0].crash_reason = reason;

    crashrecovery_info[1].crashed = true;
    crashrecovery_info[1].crash_reason = reason;
}

#ifdef __cplusplus
}
#endif

#endif // USE_CRASHRECOVERY
#endif // _CRASHRECOVERY_H_
