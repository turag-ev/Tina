#ifndef ERRORLOGGER_H
#define ERRORLOGGER_H

#include "errorobserver.h"
#include <tina/debug/internal-debug.h>

namespace TURAG {
namespace Debug {


/// \brief Fehlerzähler, der selbstständig Fehlermeldung generiert
/// \ingroup Debug
///
/// Ist dem \ref ErrorObserver ähnlich, aber arbeitet auf einer höheren
/// Abstraktionsebene, so wird im Konstruktor eine Fehlermeldung angegeben, die
/// wie bei Benutzung von \ref ErrorObserver in regelmäßigen Abständen die
/// Ausgabe selbsttätig ausgibt mit zustätzlicher Statistik zur Häufigkeit des Fehlers.
///
/// Das \ref ErrorLogger Objekt kann über das Makro ERROR_LOGGER erstellt
/// werden. Damit entfällt die Angabe der Logquelle (TURAG_DEBUG_LOG_SOURCE),
/// die über das Makro eingelesen und dem Konstruktur übergeben wird.
///
/// Mit der Funktion \ref logResult wird das Ergebnis der letzten Operation übergeben.
/// Diese Funktion gibt wenn nötig eine die Fehlermeldung mit Statistik aus.
/// Zurückgegeben wird von der Funktion, der Parameter, sodass der LogResult-Aufruf
/// direkt in eine if-Bedingung eingefügt werden kann, wo sonst nur der Parameter
/// stehen würde.
///
/// Beispielcode:
/// \code
/// // Definition
/// static ErrorLogger error_logger
///     = ERROR_LOGGER("Konnte keine GPS-Position lesen.", SystemTime::fromSec(25), 5);
/// void getGpsPos() {
///   Point point;
///   if (error_observer.logResult(GPS::getPosition(&point))) {
///     // Operation `GPS::getPosition(&point)` war erfolgreich:
///     // was mit `point` machen:
///     global.updateGPSPosition(point);
///   }
/// }
/// \endcode
///
/// \sa ErrorObserver
class ErrorLogger
{
public:
	///
	/// \brief	Fehlerzähler erstellen
	///
	/// Es wird empfohlen, das Hilfsmakro \ref ERROR_LOGGER zu benutzen.
	/// \p min_interval und \p min_errors werden intern an den Konstruktor von \ref ErrorObserver
	/// übergeben.
	///
	/// \param log_level Loglevel (z.B. `TURAG_DEBUG_ERROR_PREFIX[0]`)
	/// \param log_source Logquelle: `TURAG_DEBUG_LOG_SOURCE[0]`
	/// \param message Fehlermeldung
	/// \param min_interval minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	/// \param min_errors Fehler die mindestens ausgegeben werden, bevor die Ausgabe reduziert wird.
	///
	/// \bug Prozentzeichen in \p message müssen gedoppelt werden (wie in printf).
	///
	/// \sa ERROR_LOGGER, ErrorObserver::ErrorObserver
	ErrorLogger(char log_level, char log_source, const char* message, SystemTime min_interval, unsigned min_errors = 5);

	///
	/// \brief	Fehlerzähler für Fehlermeldungen erstellen
	///
	/// entspricht dem Aufruf mit dem ersten Parameter als `TURAG_DEBUG_ERROR_PREFIX[0]`.
	ErrorLogger(char log_source, const char* message, SystemTime min_interval, unsigned min_errors = 5) :
		ErrorLogger(TURAG_DEBUG_ERROR_PREFIX[0], log_source, message, min_interval, min_errors)
	{ }

	/// \brief Ergebnis von letzter Operation loggen
	/// \param result Ergebnis von letzter Operation
	/// \return \p result
	///
	/// \sa ErrorObserver::doErrorOutput
	bool logResult(bool result);

	/// \brief Loglevel verändern
	/// \param log_level_char TURAG_DEBUG_ERROR_PREFIX[0],
	///   TURAG_DEBUG_CRITICAL_PREFIX[0], TURAG_DEBUG_WARN_PREFIX[0],
	///   TURAG_DEBUG_INFO_PREFIX[0] oder TURAG_DEBUG_DEBUG_PREFIX[0]
	void setLogLevel(char log_level_char) { log_level_ = log_level_char; }

	/// \brief Loglevel zurückgeben
	/// \returns TURAG_DEBUG_ERROR_PREFIX[0], TURAG_DEBUG_CRITICAL_PREFIX[0],
	///    TURAG_DEBUG_WARN_PREFIX[0], TURAG_DEBUG_INFO_PREFIX[0] oder
	///    TURAG_DEBUG_DEBUG_PREFIX[0]
	char getLogLevel() const { return log_level_; }

private:
	/// der ErrorObserver, der benutzt wird, für die richtige Arbeit
	ErrorObserver observer_;

	/// Loglevel
	char log_level_;

	/// Logquelle
	char log_source_;

	/// Fehlermeldung
	const char* message_;

	/// Länge der Fehlermeldung
	std::size_t message_length_;
};

/// \brief Helfer zum Erstellen von \ref TURAG::Debug::ErrorLogger
///
/// \relates TURAG::Debug::ErrorLogger
///
/// Ruft Konstruktor von \ref TURAG::Debug::ErrorLogger auf mit `TURAG_DEBUG_LOG_SOURCE[0]` als ersten Parameter.
/// \param message Fehlermeldung
/// \param min_interval minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
/// \param min_errors Fehler die mindestens ausgegeben werden, bevor die Ausgabe reduziert wird.
///
/// Beispielcode:
/// \code
/// static ErrorLogger error_logger
///     = ERROR_LOGGER("meine schöne Fehlermeldung", SystemTime::fromSec(25), 5);
/// \endcode
///
/// \sa TURAG_DEBUG_LOG_SOURCE, TURAG::Debug::ErrorLogger::ErrorLogger
#define ERROR_LOGGER(message, min_interval, min_errors) \
	TURAG::Debug::ErrorLogger(TURAG_DEBUG_LOG_SOURCE[0], message, min_interval, min_errors)

} // namespace Debug
} // namespace TURAG

#endif // ERRORLOGGER_H
