#include "errorlogger.h"

#include <cstring>
#include <tina++/algorithm.h>

namespace TURAG {
namespace Debug {

namespace {

// kann kein constexpr sein!
#define LOG_STATISTICS " (zum %d.-mal)"

} // namespace


bool ErrorLogger::log(bool result)
{
	return observer.log(result);
}

void ErrorLogger::printMessage(void) const
{
	char preamble[] = {TURAG_DEBUG_LINE_PREFIX[0], log_source_, log_level_, 0};
	turag_debug_puts_impl(preamble);
	turag_debug_puts_impl(message_);

	if (observer.getErrorCount() <= observer.getMinErrors())
	{
		turag_debug_puts_impl(TURAG_DEBUG_NEWLINE);
	}
	else
	{
		switch (messageType_) {
		case MessageType::Simpel:
			turag_debug_printf_impl(" (zum %u.-mal)" TURAG_DEBUG_NEWLINE, observer.getErrorCount());
			break;

		case MessageType::Statistics:
			turag_debug_printf_impl(" (%u mal seit letzter Ausgabe, gesamt: %u/%u erfolgreich (%u %%))" TURAG_DEBUG_NEWLINE,
									observer.getErrorCountSinceLastOutput(),
									observer.getSuccessCount(),
									observer.getErrorCount(),
									observer.getSuccessRatio());
			break;
		}
	}
}



bool CheapErrorLogger::logResult(bool result)
{
	if (!result)
	{
		uint32_t now_ms = static_cast<uint32_t>(SystemTime::now().toMsec());
		++failuresSinceLastOutput_;

		if ((last_error_message_ms_ == 0) || (failuresSinceLastOutput_ == 0xffff) || (now_ms - last_error_message_ms_ > interval_ms_))
		{
			char preamble[] = {TURAG_DEBUG_LINE_PREFIX[0], log_source_, log_level_, 0};
			turag_debug_puts_impl(preamble);
			turag_debug_puts_impl(message_);
			if (failuresSinceLastOutput_ > 1)
			{
				turag_debug_printf_impl(" (%u mal" TURAG_DEBUG_NEWLINE, failuresSinceLastOutput_);
			}
			else
			{
				turag_debug_puts_impl(TURAG_DEBUG_NEWLINE);
			}

			last_error_message_ms_ = now_ms;
			failuresSinceLastOutput_ = 0;
		}
	}

	return result;
}


} // namespace Debug
} // namespace TURAG
