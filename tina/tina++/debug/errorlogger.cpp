#include "errorlogger.h"

#include <cstring>
#include <tina++/algorithm.h>

namespace TURAG {
namespace Debug {

namespace {

// kann kein constexpr sein!
#define LOG_STATISTICS " (zum %d.-mal)"

} // namespace

ErrorLogger::ErrorLogger(
		char log_level,
		char log_source,
		const char* message,
		SystemTime min_interval,
		unsigned min_errors)
	:
	observer_(min_interval, min_errors),
	log_level_(log_level),
	log_source_(log_source),
	message_(message),
	message_length_(std::strlen(message))
{
}

bool ErrorLogger::logResult(bool result)
{
	static constexpr std::size_t newline_size = length(TURAG_DEBUG_NEWLINE) - 1;

	if (observer_.doErrorOutput(result))
	{
		
		// FIXME: es ist nicht nötig die komplette Nachricht in einem Puffer zusammenzubauen.
		// Alle Zeichen können direkt ausgegeben werden, was Stack spart und einfache
		// Prozentzeichen in der vom User angegebenen Fehlermeldung erlaubt.
		
		// Fehlermeldung ausgeben
		std::size_t message_length = message_length_ + (length(LOG_STATISTICS)-1);
		std::uint8_t paket[3 + message_length + newline_size + 1];

		// 3 header bytes
		paket[0] = TURAG_DEBUG_LINE_PREFIX[0];
		paket[1] = log_source_;
		paket[2] = log_level_;

		// Nachricht
		std::memcpy(paket+3, message_, message_length_);
		std::memcpy(paket+3+message_length_, LOG_STATISTICS, length(LOG_STATISTICS)); // TODO: sprintf benutzen

		// end
		if (newline_size == 1) {
			paket[message_length+3] = TURAG_DEBUG_NEWLINE[0];
		} else {
			paket[message_length+3] = TURAG_DEBUG_NEWLINE[0];
			paket[message_length+3+1] = TURAG_DEBUG_NEWLINE[1];
		}
		paket[message_length+3+newline_size] = '\0';

		turag_debug_printf(reinterpret_cast<const char*>(paket), observer_.getErrorCount());
	}

	return result;
}

} // namespace Debug
} // namespace TURAG
