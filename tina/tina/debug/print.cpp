#include "print.h"

#include <stdarg.h>
#include "game_time.h"
#include <tina++/time.h>

TURAG_C_API_BEGIN

#if TURAG_DEBUG_LEVEL > 0

const char* turag_log_source_table[127] = {0};

void turag_debug_print_logsources()
{
	for (int i = 33; i < 127; i++) {
		if (turag_log_source_table[i] != 0) {
			turag_debug_printf(TURAG_DEBUG_LINE_PREFIX "%c" TURAG_DEBUG_REPORT_LOG_SOURCE_PREFIX "%s" TURAG_DEBUG_NEWLINE, i, turag_log_source_table[i]);
		}
	}
}

namespace {

using namespace TURAG;

class GameTime {
public:
	explicit GameTime(void) :
		auto_output_(false) {}

	void print()
	{ turag_print_gametime_ms(getGameTime().toMsec()); }

	void onBeforePrint();

	bool isAutomaticPrintActive() const
	{ return auto_output_; }

	SystemTime getGameStartTime() const
	{ return game_start_time_; }

	SystemTime getGameTime() const
	{ return SystemTime::now() - getGameStartTime(); }

	void setAutomaticPrint(bool value)
	{ auto_output_ = value; }

	void reset()
	{
		game_start_time_ = SystemTime::now();
		turag_print_gametime_ms(0);
	}

private:
	SystemTime game_start_time_;
	SystemTime last_gametime_output_;
	bool auto_output_;
};


void GameTime::onBeforePrint()
{
	if (!isAutomaticPrintActive())
		return;

	SystemTime now = SystemTime::now();

	if (now == last_gametime_output_)
		return;

	last_gametime_output_ = now;
	turag_print_gametime_ms((now - getGameStartTime()).toMsec());
}

GameTime game_time;

}

void _turag_print_system_gametime()
{
	game_time.print();
}

void turag_print_system_gametime_reset()
{
	game_time.reset();
}

void turag_print_gametime_automatic_enable()
{
	game_time.setAutomaticPrint(true);
}

void turag_print_gametime_automatic_disable()
{
	game_time.setAutomaticPrint(false);
}

TuragSystemTime turag_debug_system_gametime_get()
{
	return game_time.getGameTime();
}

void turag_log_puts(const char* s)
{
	game_time.onBeforePrint();

	turag_debug_puts(s);
}

void turag_log_printf(const char* fmt, ...)
{
	game_time.onBeforePrint();

	va_list vargs;
	va_start(vargs, fmt);
	turag_debug_vprintf(fmt, vargs);
	va_end(vargs);
}

void turag_debug_printf(const char* fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt);
	turag_debug_vprintf(fmt, vargs);
	va_end(vargs);
}

#else

void turag_print_system_gametime_reset() { }
void turag_print_gametime_automatic_enable() { }
void turag_print_gametime_automatic_disable() { }

TuragSystemTime turag_debug_system_gametime_get()
{
	return TURAG::SystemTime::now();
}

#endif

TURAG_C_API_END
