#include "errorlogger.h"

#include <cstring>
#include <tina++/algorithm.h>

namespace TURAG {
namespace Debug {

namespace {

// kann kein constexpr sein!
#define LOG_STATISTICS " (zum %u.-mal)"
#define LOG_STATISTICS2 " (%u mal seit letzter Ausgabe)"

} // namespace

ErrorLogger::ErrorLogger(
		char log_level,
		char log_source,
		const char* message,
		SystemTime min_interval,
		unsigned min_errors)
	:
	observer_(min_interval, min_errors),
	message_length_(std::strlen(message)),
	message_(message),
	log_level_(log_level),
	log_source_(log_source)
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
		//
		// Edit: Das ist zwar für den Roboter-Code richtig, wirft aber in der Simulation
		// Probleme auf, da wir dort nicht die notwendigen Annahmen über den Scheduler
		// treffen können. Daher könnten Ausgaben durcheinander kommen.
		
		// Fehlermeldung ausgeben
		std::size_t message_length = message_length_ + (length(LOG_STATISTICS)-1);
		char paket[3 + message_length + newline_size + 1];

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

		turag_debug_printf(paket, observer_.getErrorCount());
	}

	return result;
}

CheapErrorLogger::CheapErrorLogger(char log_level, char log_source, const char *message, SystemTime min_interval) :
	message_(message), last_error_message_ms_(0), failuresSinceLastOutput_(0),
	message_length_(static_cast<uint16_t>(std::strlen(message))), log_level_(log_level), log_source_(log_source)
{
	interval_ms_ = static_cast<uint16_t>(std::min(min_interval.toMsec(), 0xffffU));
}

bool CheapErrorLogger::logResult(bool result)
{
	static constexpr std::size_t newline_size = length(TURAG_DEBUG_NEWLINE) - 1;

	if (!result)
	{
		unsigned now_ms = static_cast<uint32_t>(SystemTime::now().toMsec());
		++failuresSinceLastOutput_;

		if ((last_error_message_ms_ == 0) || (failuresSinceLastOutput_ == 0xffff) || (now_ms - last_error_message_ms_ > interval_ms_))
		{
			// Fehlermeldung ausgeben
			std::size_t message_length = message_length_ + (length(LOG_STATISTICS2)-1);
			char paket[3 + message_length + newline_size + 1];

			// 3 header bytes
			paket[0] = TURAG_DEBUG_LINE_PREFIX[0];
			paket[1] = log_source_;
			paket[2] = log_level_;

			// Nachricht
			std::memcpy(paket+3, message_, message_length_);
			std::memcpy(paket+3+message_length_, LOG_STATISTICS2, length(LOG_STATISTICS2));

			// end
			if (newline_size == 1) {
				paket[message_length+3] = TURAG_DEBUG_NEWLINE[0];
			} else {
				paket[message_length+3] = TURAG_DEBUG_NEWLINE[0];
				paket[message_length+3+1] = TURAG_DEBUG_NEWLINE[1];
			}
			paket[message_length+3+newline_size] = '\0';

			turag_debug_printf(paket, failuresSinceLastOutput_);


			last_error_message_ms_ = now_ms;
			failuresSinceLastOutput_ = 0;
		}
	}

	return result;
}

} // namespace Debug
} // namespace TURAG
