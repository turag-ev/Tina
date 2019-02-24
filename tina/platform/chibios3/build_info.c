#ifdef TURAG_USE_BUILDINFO

#include <ch.h>
#include <hal.h>

#include <tina/debug.h>
#include <tina/build_info.h>

void turag_build_info(void)
{
    turag_info("-- Build Information --");

#ifdef CH_KERNEL_VERSION
    turag_infof("Kernel:       %s", CH_KERNEL_VERSION);
#endif
#ifdef CH_LICENSE_TYPE_STRING
    turag_infof("License type: %s", CH_LICENSE_TYPE_STRING);
#endif
#ifdef CH_LICENSE_ID_STRING
    turag_infof("License ID:   %s", CH_LICENSE_ID_STRING);
#endif
#ifdef CH_LICENSE_ID_CODE
    turag_infof("License code: %s", CH_LICENSE_ID_CODE);
#endif
#ifdef PORT_COMPILER_NAME
    turag_infof("Compiler:     %s", PORT_COMPILER_NAME);
#endif
#ifdef PORT_ARCHITECTURE_NAME
    turag_infof("Architecture: %s", PORT_ARCHITECTURE_NAME);
#endif
#ifdef PORT_CORE_VARIANT_NAME
    turag_infof("Core Variant: %s", PORT_CORE_VARIANT_NAME);
#endif
#ifdef PORT_INFO
    turag_infof("Port Info:    %s", PORT_INFO);
#endif
#ifdef PLATFORM_NAME
    turag_infof("Platform:     %s", PLATFORM_NAME);
#endif
#ifdef BOARD_NAME
    turag_infof("Board:        %s", BOARD_NAME);
#endif

#ifdef GIT_COMMIT_HASH
    turag_infof("Git Hash:     %s", GIT_COMMIT_HASH);
#endif

    // timestamp
#ifdef __DATE__
#ifdef __TIME__
    turag_infof("Build time:   %s - %s", __DATE__, __TIME__);
#endif
#endif
}

#endif // TURAG_USE_BUILDINFO
