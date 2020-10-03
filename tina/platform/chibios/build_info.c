#ifdef TURAG_USE_BUILDINFO

#include <ch.h>
#include <hal.h>

#include <tina/debug.h>
#include <tina/build_info.h>

void turag_build_info(void)
{
    turag_info("-- Build Information --");

#ifdef CH_KERNEL_VERSION
    turag_info("Kernel version: " CH_KERNEL_VERSION);
#endif
#ifdef CH_LICENSE_TYPE_STRING
    turag_info("License type:   " CH_LICENSE_TYPE_STRING);
#endif
#ifdef CH_LICENSE_ID_STRING
    turag_info("License ID:     " CH_LICENSE_ID_STRING);
#endif
#ifdef CH_LICENSE_ID_CODE
    turag_info("License code:   " CH_LICENSE_ID_CODE);
#endif
#ifdef PORT_COMPILER_NAME
    turag_info("Compiler:       " PORT_COMPILER_NAME);
#endif
#ifdef PORT_ARCHITECTURE_NAME
    turag_info("Architecture:   " PORT_ARCHITECTURE_NAME);
#endif
#ifdef PORT_CORE_VARIANT_NAME
    turag_info("Core Variant:   " PORT_CORE_VARIANT_NAME);
#endif
#ifdef PORT_INFO
    turag_info("Port Info:      " PORT_INFO);
#endif
#ifdef PLATFORM_NAME
    turag_info("Platform:       " PLATFORM_NAME);
#endif
#ifdef HAL_VERSION
    turag_info("HAL version:    " HAL_VERSION);
#endif
#ifdef BOARD_NAME
    turag_info("Board:          " BOARD_NAME);
#endif
#ifdef GIT_COMMIT_HASH
    turag_info("Git Hash:       " GIT_COMMIT_HASH);
#endif
#if defined(__DATE__) && defined(__TIME__)
    turag_info("Build time:     " __DATE__ " - "  __TIME__);
#endif
}

#endif // TURAG_USE_BUILDINFO
