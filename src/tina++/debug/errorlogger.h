#ifndef TINAPP_DEBUG_ERRORLOGGER_H
#define TINAPP_DEBUG_ERRORLOGGER_H

#include "errorobserver.h"
#include <tina/debug/internal-debug.h>

namespace TURAG {
namespace Debug {


/// \ingroup Debug
/// \brief Universeller Fehlerzähler, der selbstständig Fehlermeldung generiert.
///
/// Ist dem \ref ErrorObserver ähnlich, verfügt aber zusätzlich über eine
/// Reihe vordefinierter Ausgabemeldungen, die automatisch an eine
/// benutzerdefinierte Ausgabe angehängt werden.
///
/// \todo Ausgabemeldung ist derzeit noch nicht konfigurierbar.
///
/// Das \ref ErrorLogger Objekt kann über das Makro \ref TURAG_ERROR_LOGGER erstellt
/// werden. Damit entfällt die Angabe der Logquelle (\ref TURAG_DEBUG_LOG_SOURCE),
/// die über das Makro eingelesen und dem Konstruktur übergeben wird.
///
/// Mit der Funktion \ref logResult wird das Ergebnis der letzten Operation übergeben.
/// Diese Funktion gibt wenn nötig eine die Fehlermeldung mit Statistik aus.
/// logResult() gibt den ihr übergebenen Parameter zurück, sodass der logResult-Aufruf
/// direkt in eine if-Bedingung eingefügt werden kann, wo sonst nur der Parameter selbst
/// stehen würde.
///
/// Beispielcode:
/// \code
/// // Definition
/// static ErrorLogger error_logger
///     = TURAG_ERROR_LOGGER("Konnte keine GPS-Position lesen.", SystemTime::fromSec(25), 5);
/// void getGpsPos() {
///   Point point;
///   if (error_logger.logResult(GPS::getPosition(&point))) {
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
	/// Es wird empfohlen, das Hilfsmakro \ref TURAG_ERROR_LOGGER zu benutzen.
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
	/// \sa TURAG_ERROR_LOGGER, ErrorObserver::ErrorObserver
    ErrorLogger(char log_level, char log_source, const char* message, SystemTime min_interval, uint16_t min_errors = 5);

	///
	/// \brief	Fehlerzähler für Fehlermeldungen erstellen
	///
	/// entspricht dem Aufruf mit dem ersten Parameter als `TURAG_DEBUG_ERROR_PREFIX[0]`.
    ErrorLogger(char log_source, const char* message, SystemTime min_interval, uint16_t min_errors = 5) :
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

	/// Fehlermeldung
	const char* message_;

	/// Loglevel
	char log_level_;

	/// Logquelle
	char log_source_;
};


/// \ingroup Debug
/// \brief Einfacher Fehlerzähler mit geringem Speicherbedarf.
///
/// Ist dem \ref CheapErrorObserver ähnlich, gibt allerdings selbsttätig eine
/// konfigurierbare Meldung, erweitert um die Anzahl der seit der letzten
/// Ausgabe aufgetretenen Fehler aus.
///
/// Das \ref CheapErrorLogger Objekt kann über das Makro \ref TURAG_CHEAP_ERROR_LOGGER erstellt
/// werden. Damit entfällt die Angabe der Logquelle (\ref TURAG_DEBUG_LOG_SOURCE),
/// die über das Makro eingelesen und dem Konstruktur übergeben wird.
///
/// Mit der Funktion \ref logResult wird das Ergebnis der letzten Operation übergeben.
/// Diese Funktion gibt wenn nötig eine Fehlermeldung mit Statistik aus.
/// log() gibt den ihr übergebenen Parameter zurück, sodass der logResult-Aufruf
/// direkt in eine if-Bedingung eingefügt werden kann, wo sonst nur der Parameter selbst
/// stehen würde.
///
/// Diese Klasse ist im Vergleich zu ErrorLogger deutlich leichtgewichtiger im
/// Speicherverbrauch (üblicherweise 16 Byte).
///
/// Beispielcode:
/// \code
/// // Definition
/// static CheapErrorLogger error_logger
///     = TURAG_CHEAP_ERROR_LOGGER("Konnte keine GPS-Position lesen.", SystemTime::fromSec(25));
/// void getGpsPos() {
///   Point point;
///   if (error_logger.logResult(GPS::getPosition(&point))) {
///     // Operation `GPS::getPosition(&point)` war erfolgreich:
///     // was mit `point` machen:
///     global.updateGPSPosition(point);
///   }
/// }
/// \endcode
class CheapErrorLogger
{
public:
	///
	/// \brief Einfachen Meldungslogger erstellen
	///
	/// Es wird empfohlen, das Hilfsmakro \ref TURAG_CHEAP_ERROR_LOGGER zu benutzen.
	///
	/// \param log_level Loglevel (z.B. `TURAG_DEBUG_ERROR_PREFIX[0]`)
	/// \param log_source Logquelle: `TURAG_DEBUG_LOG_SOURCE[0]`
	/// \param message Fehlermeldung
	/// \param min_interval Minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen.
	/// Maximal 65.535 ms.
	///
	/// \bug Prozentzeichen in \p message müssen gedoppelt werden (wie in printf).
	///
	/// \sa TURAG_CHEAP_ERROR_LOGGER
	CheapErrorLogger(char log_level, char log_source, const char* message, SystemTime min_interval);

	///
	/// \brief Einfachen Meldungslogger für Fehlermeldungen erstellen
	///
	/// entspricht dem Aufruf mit dem ersten Parameter als `TURAG_DEBUG_ERROR_PREFIX[0]`.
	CheapErrorLogger(char log_source, const char* message, SystemTime min_interval) :
		CheapErrorLogger(TURAG_DEBUG_ERROR_PREFIX[0], log_source, message, min_interval)
	{ }

	/// \brief Ergebnis von letzter Operation loggen
	/// \param result Ergebnis von letzter Operation
	/// \return \p result
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

	///
	/// \brief Setzt den Meldungslogger zurück.
	///
	void reset()
	{
		last_error_message_ms_ = 0;
		failuresSinceLastOutput_ = 0;
	}


private:
	/// Fehlermeldung
	const char* message_;

	/// letzter Fehler
	uint32_t last_error_message_ms_;

	/// minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	uint16_t interval_ms_;

	/// Fehlerzähler
	uint16_t failuresSinceLastOutput_;

	/// Loglevel
	char log_level_;

	/// Logquelle
	char log_source_;
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
///     = TURAG_ERROR_LOGGER("meine schöne Fehlermeldung", SystemTime::fromSec(25), 5);
/// \endcode
///
/// \sa TURAG_DEBUG_LOG_SOURCE, TURAG::Debug::ErrorLogger::ErrorLogger
#define TURAG_ERROR_LOGGER(message, min_interval, min_errors) \
	TURAG::Debug::ErrorLogger(TURAG_DEBUG_LOG_SOURCE[0], message, min_interval, min_errors)


/// \brief Helfer zum Erstellen von \ref TURAG::Debug::CheapErrorLogger
///
/// \relates TURAG::Debug::CheapErrorLogger
///
/// Ruft Konstruktor von \ref TURAG::Debug::CheapErrorLogger auf mit `TURAG_DEBUG_LOG_SOURCE[0]` als ersten Parameter.
/// \param message Fehlermeldung
/// \param min_interval minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
///
/// Beispielcode:
/// \code
/// static CheapErrorLogger error_logger
///     = TURAG_CHEAP_ERROR_LOGGER("meine schöne Fehlermeldung", SystemTime::fromSec(25));
/// \endcode
///
/// \sa TURAG_DEBUG_LOG_SOURCE, TURAG::Debug::CheapErrorLogger::CheapErrorLogger
#define TURAG_CHEAP_ERROR_LOGGER(message, min_interval) \
	TURAG::Debug::CheapErrorLogger(TURAG_DEBUG_LOG_SOURCE[0], message, min_interval)


} // namespace Debug
} // namespace TURAG

#endif // TINAPP_DEBUG_ERRORLOGGER_H
