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
		// Fehler
		SystemTime now = SystemTime::now();
		if (now - last_error_message_ > interval_ || failure_counter_ == 0) {

			// Fehlermeldung ausgeben
			last_error_message_ = now;
			++failure_counter_;
			return true;
		}

		++failure_counter_;
	}

	return false;
}

void ErrorObserver::reset()
{
	failure_counter_ = 0;
	success_counter_ = 0;
	last_error_message_ = SystemTime(0);
}

} // namespace Debug
} // namespace TURAG
