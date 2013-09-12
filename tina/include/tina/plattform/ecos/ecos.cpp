#include "systemcontrol.h"
#include "util/ecoshelpers.h"
#include "plattform/hardware.h"

extern void user_code_start();

extern "C" void cyg_user_start() {
	user_code_start();
	cyg_scheduler_start();
}
