#ifdef USE_CRASHRECOVERY
#include <string.h>
#include <cmenu/console_menu.h>
#include <debug/cooldebug.h>
#include "crashrecovery.h"
#include "xrtc.h"

// info in recovery section of RAM
CRASHRECOVERY_SECTION volatile _CrashRecovery_Info_t crashrecovery_info[2];
CRASHRECOVERY_SECTION _CrashRecovery_Info_t *crashrecovery_valid;

// normal RAM
static bool last_instance_crashed = false;

// restart automatically after crash (normally you want to do this)
bool crashrecovery_auto_restart = true;

/**
 * @brief   calculate crashrecovery checksum
 */
static uint32_t get_checksum(volatile const _CrashRecovery_Info_t *ptr)
{
    uint32_t c = 0xDEADDA7A;

    if (!ptr || !crashrecovery_valid_validptr())
        return 0xf0f0f0f0;

    // now it gets stupid
    c ^= ptr->t_startup;
    c ^= ptr->t_lastalive;
    c ^= ptr->t_matchstart;
    c ^= ptr->match_started;

    // don't check the crashed/wip flags

    c ^= 0xDECAFBAD;
    return c;
}

/**
 * @brief   verify crashrecovery checksum
 */
static bool valid_checksum(volatile const _CrashRecovery_Info_t *ptr)
{
    if (!ptr || !crashrecovery_valid_validptr())
        return false;
    return (ptr->checksum == get_checksum(ptr));
}

/**
 * @brief   call on system startup, RTC has to be inited
 */
void crashrecovery_init()
{
    last_instance_crashed = false;

    // check for validity of the valid pointer, it has to point to one of the buffers!
    if (!crashrecovery_valid || !crashrecovery_valid_validptr()) {
        // init buffers
        crashrecovery_clear();
        return; // invalid valid ptr, so there was noone before us
    }

    // so the valid ptr points to an info struct
    // check its checksum and WIP flag
    if (crashrecovery_valid->crashed && valid_checksum(crashrecovery_valid) && !crashrecovery_valid->write_in_progress) {
        // omg it's true, we crashed
        last_instance_crashed = true;
    } else {
        // nothing crashed so we just initialize the info structs
        crashrecovery_clear();
    }
}

/**
 * @brief   Call after init to check if a crash happened in the last run.
 *          In this case you can use the crashrecovery_valid pointer to access
 *          recovery information.
 */
bool crashrecovery_get_crashed()
{
    return last_instance_crashed;
}

/**
 * @brief   call after you detected a crash and recovered from it
 */
void crashrecovery_clear()
{
    // clear structs and ptr
    memset((void*)&crashrecovery_info[0], sizeof(_CrashRecovery_Info_t), 0);
    memset((void*)&crashrecovery_info[1], sizeof(_CrashRecovery_Info_t), 0);
    crashrecovery_valid = NULL;

    // store startup time
    crashrecovery_info[0].t_startup = rtc_getunixtime();
    crashrecovery_info[1].t_startup = rtc_getunixtime();

    // normal state
    last_instance_crashed = false;
}

/**
 * @brief   write current state to recovery data
 */
void crashrecovery_write()
{
    static volatile _CrashRecovery_Info_t *ptr = NULL;

    // select the other buffer
    if (!ptr || ptr == &crashrecovery_info[1])
        ptr = &crashrecovery_info[0];
    else
        ptr = &crashrecovery_info[1];

    // store this flag to see if we crashed while writing the struct
    // the other struct then holds the valid data;
    ptr->write_in_progress = true;
    ptr->checksum = 0;

    // write info
    ptr->crashed = false;
    ptr->t_lastalive = rtc_getunixtime();
    ptr->t_matchstart = 0;
    ptr->match_started = false;

    // calculate checksum
    ptr->checksum = get_checksum(ptr);

    // end of write
    ptr->write_in_progress = false;
    crashrecovery_valid = (_CrashRecovery_Info_t *)ptr;
}

/**
 * @brief   print recovery data
 */
void crashrecovery_dump()
{
    int i;
    SCPLT_ERROR("-----\r\n");
    SCPLT_ERROR("last_instance_crashed = %d\r\n", last_instance_crashed);
    SCPLT_ERROR("crashrecovery_valid = 0x%.8x\r\n", crashrecovery_valid);

    for (i = 0; i < 2; i++) {
        SCPLT_ERROR("&crashrecovery_info[%d] = 0x%.8x\r\n", i, &crashrecovery_info[i]);
        SCPLT_ERROR(" - t_startup = %d\r\n", crashrecovery_info[i].t_startup);
        SCPLT_ERROR(" - t_lastalive = %d\r\n", crashrecovery_info[i].t_lastalive);
        SCPLT_ERROR(" - t_matchstart = %d\r\n", crashrecovery_info[i].t_matchstart);
        SCPLT_ERROR(" - match_started = %d\r\n", crashrecovery_info[i].match_started);
        SCPLT_ERROR(" > crashed = %d\r\n", crashrecovery_info[i].crashed);
        SCPLT_ERROR(" > crash_reason = 0x%x\r\n", crashrecovery_info[i].crash_reason);
        SCPLT_ERROR(" > write_in_progress = %d\r\n", crashrecovery_info[i].write_in_progress);
        SCPLT_ERROR(" > checksum = 0x%.8x (valid: %d)\r\n", crashrecovery_info[i].checksum, valid_checksum(&crashrecovery_info[i]));
    }
}

#endif // USE_CRASHRECOVERY
