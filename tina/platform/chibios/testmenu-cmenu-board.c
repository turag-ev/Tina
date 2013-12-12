#include <ch.h>
#include <hal.h>
#ifdef CHIBIOS_BENCHMARK
#include <test.h>
#endif
#include <steuerungm.h>

#include <cmenu/console_menu.h>
#include <debug/cdebug-testmenu.h>
#include "testmenu-cmenu-board.h"
#include "xserial.h"
#include "xadc.h"
#include "platform/hardware.h"
#ifdef USE_CRASHRECOVERY
#include <chrtclib.h>
#include "crashrecovery.h"
#endif

static void _TM_LedSetColorHandler(char cmd);

CMENU(_TM_LedColor, "set led color")
    CMENU_ITEM('o', _TM_LedSetColorHandler, "off")
    CMENU_ITEM('r', _TM_LedSetColorHandler, "red")
    CMENU_ITEM('g', _TM_LedSetColorHandler, "green")
    CMENU_ITEM('b', _TM_LedSetColorHandler, "blue")
    CMENU_ITEM('y', _TM_LedSetColorHandler, "yellow")
    CMENU_ITEM('c', _TM_LedSetColorHandler, "cyan")
    CMENU_ITEM('m', _TM_LedSetColorHandler, "magenta")
    CMENU_ITEM('w', _TM_LedSetColorHandler, "white")
CMENU_END

void _TM_LedSetColorHandler(char cmd){
    switch(cmd){
    case 'r':
        Led_SetColor(COLOR_RED);
        CMenu_Printf("Led red\n");
        break;
    case 'g':
        Led_SetColor(COLOR_GREEN);
        CMenu_Printf("Led green\n");
        break;
    case 'b':
        Led_SetColor(COLOR_BLUE);
        CMenu_Printf("Led blue\n");
        break;
    case 'y':
        Led_SetColor(COLOR_YELLOW);
        CMenu_Printf("Led yellow\n");
        break;
    case 'c':
        Led_SetColor(COLOR_CYAN);
        CMenu_Printf("Led cyan\n");
        break;
    case 'm':
        Led_SetColor(COLOR_MAGENTA);
        CMenu_Printf("Led magenta\n");
        break;
    case 'w':
        Led_SetColor(COLOR_WHITE);
        CMenu_Printf("Led white\n");
        break;
    case 'o':
    default:
        Led_SetColor(COLOR_OFF);
        CMenu_Printf("Led off\n");
    }
}

static void cmd_mem(char cmd) {
#if CH_USE_HEAP
  size_t n, size;
#endif
  (void)cmd;

#if CH_USE_HEAP
  n = chHeapStatus(NULL, &size);
#endif
  CMenu_Printf("core free memory : %u bytes\r\n", chCoreStatus());
#if CH_USE_HEAP
  CMenu_Printf("heap fragments   : %u\r\n", n);
  CMenu_Printf("heap free total  : %u bytes\r\n", size);
#endif
}

static void cmd_threads(char cmd) {
    static const char *states[] = {THD_STATE_NAMES};
    Thread *tp;
    (void)cmd;

    CMenu_Printf("    addr    stack prio     state time      name\r\n");
    tp = chRegFirstThread();
    do {
        CMenu_Printf("%.8lx %.8lx %4lu %9s %-9lu %s\r\n",
            (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
            (uint32_t)tp->p_prio,
            states[tp->p_state], (uint32_t)tp->p_time,
            chRegGetThreadName(tp) ? chRegGetThreadName(tp) : "n/a");
        tp = chRegNextThread(tp);
    } while (tp != NULL);
}


static void cmd_info(char cmd) {

  (void)cmd;

  CMenu_Printf("Kernel:       %s\r\n", CH_KERNEL_VERSION);
#ifdef CH_COMPILER_NAME
  CMenu_Printf("Compiler:     %s\r\n", CH_COMPILER_NAME);
#endif
  CMenu_Printf("Architecture: %s\r\n", CH_ARCHITECTURE_NAME);
#ifdef CH_CORE_VARIANT_NAME
  CMenu_Printf("Core Variant: %s\r\n", CH_CORE_VARIANT_NAME);
#endif
#ifdef CH_PORT_INFO
  CMenu_Printf("Port Info:    %s\r\n", CH_PORT_INFO);
#endif
#ifdef PLATFORM_NAME
  CMenu_Printf("Platform:     %s\r\n", PLATFORM_NAME);
#endif
#ifdef BOARD_NAME
  CMenu_Printf("Board:        %s\r\n", BOARD_NAME);
#endif
#ifdef __DATE__
#ifdef __TIME__
  CMenu_Printf("Build time:   %s%s%s\r\n", __DATE__, " - ", __TIME__);
#endif
#endif
}

static void cmd_systime(char cmd)
{
    (void)cmd;
    CMenu_Printf("%lu\r\n", (unsigned long)chTimeNow());
}

#ifdef CHIBIOS_BENCHMARK
#define TEST_WA_SIZE    THD_WA_SIZE(256)
static void cmd_test(BaseSequentialStream *chp) {
  Thread *tp;

  tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(),
                           TestThread, chp);
  if (tp == NULL) {
    CMenu_Printf("out of memory\r\n");
    return;
  }
  chThdWait(tp);
}

static void cmd_benchmark(char cmd) {
    (void)cmd;
    cmd_test((BaseSequentialStream*)&SDU1);
}
#endif

#ifdef USE_CRASHRECOVERY
// bootloader
static void cmd_bootloaderflag(char cmd) {
    (void)cmd;
    uint32_t flag = *((uint32_t*)0x1000fffc);

    CMenu_Printf("Bootloader Flag: 0x%.8x\r\n", flag);
}

// RTC
static void cmd_rtc_getrawtime(char cmd) {
    (void)cmd;
    RTCTime ts;

    rtcGetTime(&RTCD1, &ts);
    CMenu_Printf("Date: %d Time: %d Msec: %d\r\n", ts.tv_date, ts.tv_time, ts.tv_msec);
}

static void cmd_rtc_getunixtime(char cmd) {
    time_t unix_time;
    struct tm timp;
    (void)cmd;
    unix_time = rtcGetTimeUnixSec(&RTCD1);

    if (unix_time == -1){
      CMenu_Printf("incorrect time in RTC cell\r\n");
    } else{
      CMenu_Printf("unixtime: %D\r\n", unix_time);
      rtcGetTimeTm(&RTCD1, &timp);
      CMenu_Printf("asctime (UTC): %s\r", asctime(&timp));
    }
}

static void cmd_rtc_setunixtime(char cmd) {
    time_t unix_time;
    unsigned long rtime = 0;
    (void)cmd;

	if (!CMenu_ReadUnsignedWord("Enter Unix Timestamp (try 'date +%s')", &rtime)) {
        CMenu_Printf("\r\nbad timestamp, try something like 1366584013\r\n");
        return;
    }

    unix_time = rtime;
    if (unix_time > 0){
        rtcSetTimeUnixSec(&RTCD1, unix_time);
        CMenu_Printf("\r\nTime set to: %d\r\n", unix_time);
    }
}

CMENU(RTC_Menu, "RTC menu")
    CMENU_ITEM('t', cmd_rtc_getrawtime, "Get Raw Time")
    CMENU_ITEM('r', cmd_rtc_getunixtime, "Get Time")
    CMENU_ITEM('R', cmd_rtc_setunixtime, "Set Time")
CMENU_END

// Crash Recovery
static void cmd_crashdump(char cmd) {
    (void)cmd;
    crashrecovery_dump();
}

// toggle auto restart
static void cmd_crashautorestart(char cmd) {
    (void)cmd;

    crashrecovery_auto_restart = !crashrecovery_auto_restart;
    CMenu_Printf("crashrecovery_auto_restart: %d\r\n", crashrecovery_auto_restart);
}

// crash the system
static void cmd_crash(char cmd) {
    void (*rofl)(void) = NULL;
    (void)cmd;

    CMenu_Printf("CRASHING THE FUCKING FUCK\r\n");
    chThdSleepMilliseconds(50); // wait for string output
    rofl();
}

static void cmd_crashclear(char cmd) {
    (void)cmd;
    crashrecovery_clear();
    CMenu_Printf("Recovery Data cleared\r\n");
}

CMENU(CrashRecovery_Menu, "Crash Recovery menu")
    CMENU_ITEM('b', cmd_bootloaderflag, "show bootloader flag")
    CMENU_ITEM('c', cmd_crashdump, "dump crash recovery data")
    CMENU_ITEM('C', cmd_crashclear, "clear crash recovery data")
    CMENU_ITEM('r', cmd_crashautorestart, "toggle auto restart after crash (default: true)")
    CMENU_ITEM('!', cmd_crash, "CRASH THE SYSTEM!")
CMENU_END
#endif

static void cmd_iminit(char cmd) {
    (void)cmd;
    CMenu_Printf("init Hardware\r\n");
    Hardware_Init();
    CMenu_Printf("init Hardware done\r\n");
}

static void cmd_adc(char cmd) {
    (void)cmd;
    CMenu_Printf("Monitor 7V raw value: %d\r\n", adc_get_mon7v());
}

extern const CMenu_t mainMenu __attribute__((weak, alias("boardTestMenu")));

CMENU(boardTestMenu, "board test menu")
    CMENU_ENTERSUBMENUITEM('1', _CDebug_Settings)
    CMENU_ITEM('d', _CDebug_AlloffSC, "CD: deactivate all except SC")
    CMENU_ITEM('D', _CDebug_Quicktoggle, "CD: toggle all debug sources")

    CMENU_ITEM('i', cmd_info, "ChibiOS info")
    CMENU_ITEM('s', cmd_systime, "system time")
    CMENU_ITEM('t', cmd_threads, "active threads")
    CMENU_ITEM('m', cmd_mem, "memory usage")

#ifdef USE_CRASHRECOVERY
    CMENU_ENTERSUBMENUITEM('c', CrashRecovery_Menu)
    CMENU_ENTERSUBMENUITEM('r', RTC_Menu)
#endif

    CMENU_ITEM('Q', cmd_iminit, "Hardware init (don't do this)")
    CMENU_ENTERSUBMENUITEM('l', _TM_LedColor)
#if CHIBIOS_BENCHMARK
    CMENU_ITEM('B', cmd_benchmark, "ChibiOS Benchmark")
#endif

    CMENU_ITEM('a', cmd_adc, "ADC values")
CMENU_END
