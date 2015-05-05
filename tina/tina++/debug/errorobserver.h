#ifndef ERROROBSERVER_H
#define ERROROBSERVER_H

#include <tina++/time.h>

namespace TURAG {

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
///   if (!GPS::getPosition(&point)) {
///     // Fehler
///     if (error_observer.failure()) {
///       // Zeit für neue Ausgabe ist gekommen, da letzter Fehler ausrechend entfernt oder
///       // noch nie ein Fehler aufgetretten
///       turag_errorf("Konnte schon zum %u. mal keine GPS-Position lesen.", error_observer.getErrorCount());
///     }
///   }
/// }
/// \endcode
class ErrorObserver
{
public:
	// ctors

	/// Fehlerüberwacher erstellen
	/// \param min_interval minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	constexpr
	ErrorObserver(SystemTime min_interval) :
		interval_(min_interval)
	{ }

	// verändern

	/// \brief Fehler signalisieren
	///
	/// wenn letzter Fehler mehr als getMinInterval() entfernt ist, dann wird Fehlermeldung ausgegeben
	bool failure();

	// Parameter auslesen

	/// \brief minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	const SystemTime& getMinInterval() const { return interval_; }

	// Werte auslesen

	/// \brief Anzahl der Fehler, die aufgetretten sind
	unsigned getErrorCount() const { return counter_; }

	/// \brief Zeitpunkt, an dem letzte Fehlermeldung ausgegeben wurde
	///
	/// Wenn noch kein Fehler aufgetretten ist (getErrorCount() == 0), dann
	/// wird `SystemTime(0)` zurück gegeben.
	const SystemTime& getLastErrorMessageTime() const { return last_error_message_;}

private:
	/// Fehlerzähler
	unsigned counter_ = 0;

	/// letzter Fehler
	SystemTime last_error_message_ = SystemTime(0);

	/// minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	SystemTime interval_;
};

} // namespace TURAG

#endif // ERROROBSERVER_H
