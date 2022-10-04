#include "errorobserver.h"

#include <tina/debug/internal-debug.h>


namespace TURAG {
namespace Debug {

bool ErrorObserver::doErrorOutput(bool success)
{
	if (success)
	{
		// Success
		++success_counter_;
	}
	else
	{
		++failure_counter_;

		// Failure
		unsigned now_ms = SystemTime::now().toMsec();

		// advise message output if
		// - min_interval has exceeded or
		// - the failures happen for the very first time or
		// - there have been too many failures since the last output
		if (now_ms - last_error_message_ms_ > interval_ms_ ||
				failure_counter_ <= min_errors_ ||
				(max_errors_ != 0 && failure_counter_ - failuresOnLastOutput_ > max_errors_))
		{
			// advise message output
			last_error_message_ms_ = now_ms;
			failuresSinceLastOutput_ = failure_counter_ - failuresOnLastOutput_;
			successesSinceLastOutput_ = success_counter_ - successesOnLastOutput_;
			failuresOnLastOutput_ = failure_counter_;
			successesOnLastOutput_ = success_counter_;
			return true;
		}
	}

	return false;
}

void ErrorObserver::reset()
{
	failure_counter_ = 0;
	success_counter_ = 0;
	failuresOnLastOutput_ = 0;
	successesOnLastOutput_ = 0;
	failuresSinceLastOutput_ = 0;
	successesSinceLastOutput_ = 0;
	last_error_message_ms_ = 0;
}



bool CheapErrorObserver::doErrorOutput(bool success)
{
	if (!success)
	{
		unsigned now_ms = static_cast<uint32_t>(SystemTime::now().toMsec());
		++failures_;

		if ((last_error_message_ms_ == 0) || (now_ms - last_error_message_ms_ > interval_ms_))
		{
			last_error_message_ms_ = now_ms;
			return true;
		}
	}

	return false;
}

} // namespace Debug
} // namespace TURAG
