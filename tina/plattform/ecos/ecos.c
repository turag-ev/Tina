#include "cyg/kernel/kapi.h"

extern void user_code_start();

void cyg_user_start() {
	user_code_start();
	cyg_scheduler_start();
}
