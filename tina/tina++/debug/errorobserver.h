#ifndef ERROROBSERVER_H
#define ERROROBSERVER_H

#include <tina++/time.h>

namespace TURAG {
namespace Debug {

/// \ingroup Debug
/// \brief Universeller Fehlerüberwacher.
///
/// Die Klasse dient dazu häufige Fehlermeldungen nicht sofort
/// auszugeben, sondern nur in bestimmten Abständen, damit
/// die Debugausgabe nicht zugespamt wird. Kann aber auch für andere
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
		interval_ms_(min_interval.toMsec()), min_errors_(min_errors)
	{ }

	// verändern

	/// \brief Abfrage ob Ausgabe erfolgen soll.
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
	const SystemTime getMinInterval() const { return SystemTime::fromMsec(interval_ms_); }

	// Werte auslesen

	/// \brief Anzahl der Fehler, die aufgetreten sind
	constexpr unsigned getErrorCount() const { return failure_counter_; }

	/// \brief Anzahl der erfolgreichen Durchführungen
	constexpr unsigned getSuccessCount() const { return success_counter_; }

	/// \brief Erfolgsquote in Prozent
	constexpr unsigned getSuccessRatio() const
	{ return 100 * success_counter_ / (failure_counter_ + success_counter_); }

	/// \brief Fehlerquote in Prozent
	constexpr unsigned getErrorRatio() const
	{ return 100 * failure_counter_ / (failure_counter_ + success_counter_); }
	/// \brief Anzahl alle Durchführungen
	constexpr unsigned getTotalCount() const
	{ return failure_counter_ + success_counter_; }

	/// \brief Anzahl der Fehler, die seit letzter Ausgabe aufgetreten sind
	constexpr unsigned getErrorCountSinceLastOutput() const
	{ return failuresSinceLastOutput_; }

	/// \brief Anzahl der erfolgreichen Durchführungen seit letzter Ausgabe
	constexpr unsigned getSuccessCountSinceLastOutput() const
	{ return successesSinceLastOutput_; }

	/// \brief Erfolgsquote in Prozent seit letzter Ausgabe
	constexpr unsigned getSuccessRatioSinceLastOutput() const
	{ return 100 * successesSinceLastOutput_ / (failuresSinceLastOutput_ + successesSinceLastOutput_); }

	/// \brief Fehlerquote in Prozent seit letzter Ausgabe
	constexpr unsigned getErrorRatioSinceLastOutput() const
	{ return 100 * failuresSinceLastOutput_ / (failuresSinceLastOutput_ + successesSinceLastOutput_); }

	/// \brief Anzahl alle Durchführungen seit der letzten Ausgabe
	constexpr unsigned getTotalCountSinceLastOutput() const
	{ return failuresSinceLastOutput_ + successesSinceLastOutput_; }

	/// \brief Zeitpunkt, an dem letzte Fehlermeldung ausgegeben wurde
	///
	/// Wenn noch kein Fehler aufgetretten ist (getErrorCount() == 0), dann
	/// wird `SystemTime(0)` zurück gegeben.
	constexpr const SystemTime getLastErrorMessageTime() const { return SystemTime::fromMsec(last_error_message_ms_); }

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
	unsigned last_error_message_ms_ = 0;

	/// minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	unsigned interval_ms_;

	unsigned min_errors_;
};


/// \ingroup Debug
/// \brief Einfacher Fehlerüberwacher mit geringem Speicherbedarf.
///
/// Die Klasse dient dazu häufige Fehlermeldungen nicht sofort
/// auszugeben, sondern nur in bestimmten Abständen, damit
/// die Debugausgabe nicht zugespamt wird. Kann aber auch für andere
/// ähnliche Sachen benutzt werden.
///
/// Diese Klasse ist gegenüber ErrorObserver deutlich leichtgewichtiger
/// im Speicherverbrauch (üblicherweise 12 Byte). Dafür stellt sie
/// als einzige Information die Gesamtfehleranzahl zur Verfügung.
/// Wird eine Information über die seit der letzten Ausgabe aufgetretenen
/// Fehleranzahl benötigt, so kann der Fehlerzähler mit resetErrorCount()
/// manuell zurückgesetzt werden.
///
/// In folgenden Beispiel liegen mindestens 25 Sekunden zwischen jeder Fehlermeldung:
/// \code
/// static CheapErrorObserver error_observer(SystemTime::fromSec(25)); // Definition
/// void getGpsPos() {
///   Point point;
///   if (error_observer.doErrorOutput(GPS::getPosition(&point))) {
///     // Zeit für neue Ausgabe ist gekommen, da letzter Fehler ausrechend entfernt oder
///     // noch nie ein Fehler aufgetretten
///     turag_errorf("Konnte schon zum %u. mal keine GPS-Position lesen.", error_observer.getErrorCount());
///   }
/// }
///
/// \endcode
class CheapErrorObserver
{
public:
	// ctors

	/// Fehlerüberwacher erstellen
	/// \param min_interval minimales Intervall in dem Fehlermeldungen
	/// ausgegeben werden dürfen.
	CheapErrorObserver(SystemTime min_interval) :
		interval_ms_(min_interval.toMsec())
	{ }


	// Parameter auslesen

	/**
	 * @brief Anzahl der aufgetretenen Fehler.
	 *
	 * Kann mit resetErrorCount() zurückgesetzt werden.
	 */
	unsigned getErrorCount(void)
	{
		return failures_;
	}


	// verändern

	/// \brief Abfrage ob Ausgabe erfolgen soll.
	///
	/// \param success Operation war erfolgreich
	/// \returns \a true, wenn Fehlerausgabe fällig ist
	bool doErrorOutput(bool success);

	/**
	 * @brief Fehlerüberwacher zurücksetzen
	 *
	 * Fehlerzähler sowie letzten Fehlerzeitpunkt zurücksetzen.
	 */
	void resetAll()
	{
		last_error_message_ms_ = 0;
		failures_ = 0;
	}

	/**
	 * @brief Fehlerzähler zurücksetzen.
	 */
	void resetErrorCount(void)
	{
		failures_ = 0;
	}


private:
	/// minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	unsigned interval_ms_;

	/// letzter Fehler
	unsigned last_error_message_ms_ = 0;

	/// Fehlerzähler
	unsigned failures_ = 0;
};



} // namespace Debug
} // namespace TURAG

#endif // ERROROBSERVER_H
