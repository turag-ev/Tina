#ifndef ERRORLOGGER_H
#define ERRORLOGGER_H

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
/// Das \ref ErrorLogger Objekt kann über das Makro \ref ERROR_LOGGER erstellt
/// werden. Damit entfällt die Angabe der Logquelle (\ref TURAG_DEBUG_LOG_SOURCE),
/// die über das Makro eingelesen und dem Konstruktur übergeben wird.
///
/// Mit der Funktion \ref log wird das Ergebnis der letzten Operation übergeben.
/// Diese Funktion gibt wenn nötig eine Fehlermeldung mit Statistik aus.
/// log() gibt den ihr übergebenen Parameter zurück, sodass der log-Aufruf
/// direkt in eine if-Bedingung eingefügt werden kann, wo sonst nur der Parameter selbst
/// stehen würde.
///
/// Beispielcode:
/// \code
/// // Definition
/// static ErrorLogger error_logger
///     = ERROR_LOGGER("Konnte keine GPS-Position lesen.", SystemTime::fromSec(25));
/// void getGpsPos() {
///   Point point;
///   if (error_logger.log(GPS::getPosition(&point))) {
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
	/**
	 * @brief Verschiedene Ausgabevarianten.
	 */
	enum class MessageType : uint8_t {
		/// Ausgabe der Gesamtfehleranzahl.
		Simpel,
		/// Ausgabe der Fehleranzahl seit der letzten Ausgabe und einer
		/// Gesamtstatistik.
		Statistics
	};

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
	/// \param messageType Gibt an, was für eine Art von Fehlermeldung ausgegeben werden soll.
	/// \param min_errors Fehler die mindestens ausgegeben werden, bevor die Ausgabe reduziert wird.
	///
	/// \sa ERROR_LOGGER, ErrorObserver::ErrorObserver
	ErrorLogger(char log_level, char log_source, const char* message, SystemTime min_interval,
				MessageType messageType = MessageType::Simpel, unsigned min_errors = 5) :
		observer(this, min_interval, &ErrorLogger::printMessage, min_errors),
		message_(message),
		log_level_(log_level),
		log_source_(log_source),
		messageType_(messageType)
	{ }

	///
	/// \brief	Fehlerzähler für Fehlermeldungen erstellen
	///
	/// entspricht dem Aufruf mit dem ersten Parameter als `TURAG_DEBUG_ERROR_PREFIX[0]`.
	ErrorLogger(char log_source, const char* message, SystemTime min_interval,
				MessageType messageType = MessageType::Simpel, unsigned min_errors = 5) :
		ErrorLogger(TURAG_DEBUG_ERROR_PREFIX[0], log_source, message, min_interval, messageType, min_errors)
	{ }

	/// \brief Ergebnis von letzter Operation loggen
	/// \param result Ergebnis von letzter Operation
	/// \return \p result
	///
	/// \sa ErrorObserver::doErrorOutput
	bool log(bool result);

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
	void printMessage(void) const;

	ErrorObserver<ErrorLogger> observer;

	/// Fehlermeldung
	const char* message_;

	/// Loglevel
	char log_level_;

	/// Logquelle
	char log_source_;

	MessageType messageType_;
};


/// \ingroup Debug
/// \brief Einfacher Fehlerzähler mit geringem Speicherbedarf.
///
/// Ist dem \ref CheapErrorObserver ähnlich, gibt allerdings selbsttätig eine
/// konfigurierbare Meldung, erweitert um die Anzahl der seit der letzten
/// Ausgabe aufgetretenen Fehler aus.
///
/// Das \ref ErrorLogger Objekt kann über das Makro \ref CHEAP_ERROR_LOGGER erstellt
/// werden. Damit entfällt die Angabe der Logquelle (\ref TURAG_DEBUG_LOG_SOURCE),
/// die über das Makro eingelesen und dem Konstruktur übergeben wird.
///
/// Mit der Funktion \ref log wird das Ergebnis der letzten Operation übergeben.
/// Diese Funktion gibt wenn nötig eine Fehlermeldung mit Statistik aus.
/// log() gibt den ihr übergebenen Parameter zurück, sodass der log-Aufruf
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
///     = CHEAP_ERROR_LOGGER("Konnte keine GPS-Position lesen.", SystemTime::fromSec(25));
/// void getGpsPos() {
///   Point point;
///   if (error_logger.log(GPS::getPosition(&point))) {
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
	/// Es wird empfohlen, das Hilfsmakro \ref CHEAP_ERROR_LOGGER zu benutzen.
	///
	/// \param log_level Loglevel (z.B. `TURAG_DEBUG_ERROR_PREFIX[0]`)
	/// \param log_source Logquelle: `TURAG_DEBUG_LOG_SOURCE[0]`
	/// \param message Fehlermeldung
	/// \param min_interval minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	///
	/// \sa CHEAP_ERROR_LOGGER
	CheapErrorLogger(char log_level, char log_source, const char* message, SystemTime min_interval) :
		message_(message), last_error_message_ms_(0), interval_ms_(min_interval.toMsec()),
		failuresSinceLastOutput_(0), log_level_(log_level), log_source_(log_source)
	{ }

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
	uint32_t interval_ms_;

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
///     = ERROR_LOGGER("meine schöne Fehlermeldung", SystemTime::fromSec(25), 5);
/// \endcode
///
/// \sa TURAG_DEBUG_LOG_SOURCE, TURAG::Debug::ErrorLogger::ErrorLogger
#define ERROR_LOGGER(message, min_interval, min_errors) \
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
///     = CHEAP_ERROR_LOGGER("meine schöne Fehlermeldung", SystemTime::fromSec(25));
/// \endcode
///
/// \sa TURAG_DEBUG_LOG_SOURCE, TURAG::Debug::CheapErrorLogger::CheapErrorLogger
#define CHEAP_ERROR_LOGGER(message, min_interval) \
	TURAG::Debug::CheapErrorLogger(TURAG_DEBUG_LOG_SOURCE[0], message, min_interval)

} // namespace Debug
} // namespace TURAG

#endif // ERRORLOGGER_H
