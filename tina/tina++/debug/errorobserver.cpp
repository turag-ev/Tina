#include "errorobserver.h"

#include <tina/debug/internal-debug.h>

namespace TURAG {

bool ErrorObserver::failure()
{
	SystemTime now = SystemTime::now();
	if (now - last_error_message_ > interval_ || counter_ == 0) {

		// Fehlermeldung ausgeben
		last_error_message_ = now;
		++counter_;
		return true;
	}

	++counter_;
	return false;
}

} // namespace TURAG
