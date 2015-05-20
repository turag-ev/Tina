#ifndef ERRORLOGGER_H
#define ERRORLOGGER_H

#include "errorobserver.h"
#include <tina/debug/internal-debug.h>

namespace TURAG {
namespace Debug {


/// \brief Fehlerzähler, der selbstständig Fehlermeldung generiert
class ErrorLogger
{
public:
	ErrorLogger(char log_level, char log_source, const char* message, SystemTime min_interval, unsigned min_errors = 5);

	ErrorLogger(char log_source, const char* message, SystemTime min_interval, unsigned min_errors = 5) :
		ErrorLogger(TURAG_DEBUG_ERROR_PREFIX[0], log_source, message, min_interval, min_errors)
	{ }


	bool logResult(bool result);

	void setLogLevel(char log_level_char) { log_level_ = log_level_char; }
	char getLogLevel() const { return log_level_; }

private:
	ErrorObserver observer_;

	char log_level_;
	char log_source_;
	const char* message_;
	std::size_t message_length_;
};

#define ERROR_LOGGER(message, min_interval, min_errors) \
	TURAG::Debug::ErrorLogger(TURAG_DEBUG_LOG_SOURCE[0], message, min_interval, min_errors)

} // namespace Debug
} // namespace TURAG

#endif // ERRORLOGGER_H
