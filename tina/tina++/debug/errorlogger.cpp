#include "errorlogger.h"

#include <tina++/algorithm.h>

namespace TURAG {
namespace Debug {

ErrorLogger::ErrorLogger(char log_level,
        char log_source,
        const char* message,
        SystemTime min_interval,
        uint16_t min_errors)
	:
	observer_(min_interval, min_errors),
	message_(message),
	log_level_(log_level),
	log_source_(log_source)
{
}

bool ErrorLogger::logResult(bool result)
{
	if (observer_.doErrorOutput(result))
	{
        turag_debug_printf(TURAG_DEBUG_LINE_PREFIX "%c%c%s (zum %u. mal)" TURAG_DEBUG_NEWLINE,
                           log_source_, log_level_, message_, observer_.getErrorCount());
	}

	return result;
}

CheapErrorLogger::CheapErrorLogger(char log_level, char log_source, const char *message, SystemTime min_interval) :
	message_(message), last_error_message_ms_(0), failuresSinceLastOutput_(0),
	log_level_(log_level), log_source_(log_source)
{
	interval_ms_ = static_cast<uint16_t>(std::min(min_interval.toMsec(), 0xffffU));
}

bool CheapErrorLogger::logResult(bool result)
{
	if (!result)
	{
		unsigned now_ms = static_cast<uint32_t>(SystemTime::now().toMsec());
		++failuresSinceLastOutput_;

		if ((last_error_message_ms_ == 0) || (failuresSinceLastOutput_ == 0xffff) || (now_ms - last_error_message_ms_ > interval_ms_))
		{
            turag_debug_printf(TURAG_DEBUG_LINE_PREFIX "%c%c%s (%u mal seit letzter Ausgabe)" TURAG_DEBUG_NEWLINE,
                               log_source_, log_level_, message_, failuresSinceLastOutput_);

			last_error_message_ms_ = now_ms;
			failuresSinceLastOutput_ = 0;
		}
	}

	return result;
}

} // namespace Debug
} // namespace TURAG
