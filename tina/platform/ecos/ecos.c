#include "cyg/kernel/kapi.h"

extern void user_code_start(void);

void cyg_user_start(void) {
	user_code_start();
	cyg_scheduler_start();
}
