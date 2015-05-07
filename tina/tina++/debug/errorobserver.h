#ifndef ERROROBSERVER_H
#define ERROROBSERVER_H

#include <tina++/time.h>

namespace TURAG {
namespace Debug {

/// \brief Fehlerüberwacher
/// \ingroup Debug
///
/// Die Klasse dient dazu häufige Fehlermeldungen nicht sofort
/// auszugeben, sondern nur in bestimmten Abständen, damit
/// die Debugausgabe nicht "zugespämt" wird. Kann aber auch für andere
/// ähnliche Sachen benutzt werden.
///
/// In folgenden Beispiel liegen mindestens 25 Sekunden zwischen jeder Fehlermeldung:
/// \code
/// static ErrorObserver error_observer(SystemTime::fromSec(25)); // Definition
/// void getGpsPos() {
///   Point point;
///   if (error_observer.doErrorOutput(GPS::getPosition(&point))) {
///     // Zeit für neue Ausgabe ist gekommen, da letzter Fehler ausrechend entfernt oder
///     // noch nie ein Fehler aufgetretten
///     turag_errorf("Konnte schon zum %u. mal keine GPS-Position lesen.", error_observer.getErrorCount());
///   }
/// }
/// \endcode
class ErrorObserver
{
public:
	// ctors

	/// Fehlerüberwacher erstellen
	/// \param min_interval minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	/// \param min_errors Fehler die mindestens ausgegeben werden, bevor die Ausgabe reduziert wird.
	constexpr
	ErrorObserver(SystemTime min_interval, unsigned min_errors = 5) :
		interval_(min_interval), min_errors_(min_errors)
	{ }

	// verändern

	/// \brief Abfrage, ob Ausgabe erfolgen soll, bei neuer Operationsausführung
	///
	/// \param success Operation war erfolgreich
	/// \returns \a true, wenn Fehlerausgabe fällig ist
	bool doErrorOutput(bool success);

	/// \brief Fehlerüberwacher zurücksetzen
	///
	/// Fehler- und Erfolgszähler sowie letzten Fehlerzeitpunkt zurücksetzen
	void reset();

	// Parameter auslesen

	/// \brief minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	const SystemTime& getMinInterval() const { return interval_; }

	// Werte auslesen

	/// \brief Anzahl der Fehler, die aufgetreten sind
	unsigned getErrorCount() const { return failure_counter_; }

	/// \brief Anzahl der erfolgreichen Durchführungen
	unsigned getSuccessCount() const { return success_counter_; }

	/// \brief Erfolgsquote in Prozent
	unsigned getSuccessRatio() { return 100 * success_counter_ / (failure_counter_ + success_counter_); }

	/// \brief Fehlerquote in Prozent
	unsigned getErrorRatio() { return 100 * failure_counter_ / (failure_counter_ + success_counter_); }
	/// \brief Anzahl alle Durchführungen
	unsigned getTotalCount() { return failure_counter_ + success_counter_; }

	/// \brief Anzahl der Fehler, die seit letzter Ausgabe aufgetreten sind
	unsigned getErrorCountSinceLastOutput() { return failuresSinceLastOutput_; }

	/// \brief Anzahl der erfolgreichen Durchführungen seit letzter Ausgabe
	unsigned getSuccessCountSinceLastOutput() { return successesSinceLastOutput_; }

	/// \brief Erfolgsquote in Prozent seit letzter Ausgabe
	unsigned getSuccessRatioSinceLastOutput() { return 100 * successesSinceLastOutput_ / (failuresSinceLastOutput_ + successesSinceLastOutput_); }

	/// \brief Fehlerquote in Prozent seit letzter Ausgabe
	unsigned getErrorRatioSinceLastOutput() { return 100 * failuresSinceLastOutput_ / (failuresSinceLastOutput_ + successesSinceLastOutput_); }

	/// \brief Anzahl alle Durchführungen seit der letzten Ausgabe
	unsigned getTotalCountSinceLastOutput() { return failuresSinceLastOutput_ + successesSinceLastOutput_; }

	/// \brief Zeitpunkt, an dem letzte Fehlermeldung ausgegeben wurde
	///
	/// Wenn noch kein Fehler aufgetretten ist (getErrorCount() == 0), dann
	/// wird `SystemTime(0)` zurück gegeben.
	const SystemTime& getLastErrorMessageTime() const { return last_error_message_;}

private:
	/// Fehlerzähler
	unsigned failure_counter_ = 0;
	unsigned failuresOnLastOutput_ = 0;
	unsigned failuresSinceLastOutput_ = 0;

	/// Erfolgszähler
	unsigned success_counter_ = 0;
	unsigned successesOnLastOutput_ = 0;
	unsigned successesSinceLastOutput_ = 0;

	/// letzter Fehler
	SystemTime last_error_message_ = SystemTime(0);

	/// minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	SystemTime interval_;

	unsigned min_errors_;
};

} // namespace Debug
} // namespace TURAG

#endif // ERROROBSERVER_H
