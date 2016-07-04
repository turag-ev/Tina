#include "errorobserver.h"

#include <tina/debug/internal-debug.h>

namespace TURAG {
namespace Debug {

bool ErrorObserver::doErrorOutput(bool success)
{
	if (success)
	{
		// Erfolg
		++success_counter_;
	}
	else
	{
		++failure_counter_;

		// Fehler
		SystemTime now = SystemTime::now();
		if (now - last_error_message_ > interval_ || failure_counter_ <= min_errors_) {

			// Fehlermeldung ausgeben
			last_error_message_ = now;
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
	last_error_message_ = SystemTime(0);
}

} // namespace Debug
} // namespace TURAG
