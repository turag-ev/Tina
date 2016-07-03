#ifndef ERROROBSERVER_H
#define ERROROBSERVER_H

#include <tina++/time.h>
#include <tina/debug/print-impl.h>
#include <algorithm>

namespace TURAG {
namespace Debug {

/// \ingroup Debug
/// \brief Universeller Fehlerüberwacher.
///
/// Die Klasse dient dazu häufige Fehlermeldungen nicht sofort
/// auszugeben, sondern nur in bestimmten Abständen, damit
/// die Debugausgabe nicht zugespamt wird. Kann aber auch für andere
/// ähnliche Sachen benutzt werden. Dazu wird im Konstruktor ein
/// Pointer zu einer Funktion übergeben, die aufgerufen wird sobald
/// eine Ausgabe nötig ist. Es werden zahlreiche Parameter wie
/// Erfolgsquote, Gesamtanzahl der bislang aufgetretenen Fehler usw.
/// bereitgestellt.
///
/// In folgenden Beispiel liegen mindestens 25 Sekunden zwischen jeder Fehlermeldung:
/// \code
///		// Definition, direkte Logausgabe mittels Lambda-Funktion
///		static ErrorObserver logger(SystemTime::fromSec(25), [](const ErrorObserver& l) {
///			turag_errorf("Konnte zum %u. mal keine GPS-Position lesen (%u %% Erfolgsquote).", l.getErrorCount(), l.getSuccessRatio());
///		});
///
///     void getGpsPos() {
///       Point point;
///       if (logger.log(GPS::getPosition(&point))) {
///         // do something with GPS data
///       }
///     }
/// \endcode
template <class T>
class ErrorObserver
{
public:
	/// \brief Funktion zum Ausgeben einer Fehlermeldung.
	/// \param observer Referenz auf die aufrufende Instanz.
	using LogFunction = void(T::*)(void) const;

	// ctors

	/// Fehlerüberwacher erstellen
	/// \param min_interval Minimales Intervall innerhalb dessen Fehlermeldungen ausgegeben werden dürfen.
	/// \param logFunction Pointer zu einer Funktion, die die Logmeldung ausgibt.
	/// Dies kann eine Lambda-Funktion (ohne Capturing) sein.
	/// \param min_errors Fehler die mindestens ausgegeben werden, bevor die Ausgabe reduziert wird.
	constexpr
	ErrorObserver(const T* parent, SystemTime min_interval, LogFunction logFunction, unsigned min_errors = 5) :
		parent_(parent), logFunction_(logFunction), interval_ms_(min_interval.toMsec()), min_errors_(min_errors)
	{ }

	// verändern

	/// \brief Ergebnis von letzter Operation loggen
	/// \param result Ergebnis von letzter Operation
	/// \return \p result
	///
	/// Ruft die im Konstruktor übergebene Funktion auf, wenn eine
	/// Ausgabe nötig ist. Das Ergebnis der Funktion wird unverändert
	/// zurückgegeben, sodass diese Funktion ohne Anpassung in
	/// bestehenden Code eingefügt werden kann, um eine Fehlerausgabe
	/// zu erhalten.
	bool log(bool result)
	{
		if (result)
		{
			// Erfolg
			++success_counter_;
		}
		else
		{
			++failure_counter_;

			// Fehler
			uint32_t now = SystemTime::now().toMsec();
			if (now - last_error_message_ms_ > interval_ms_ || failure_counter_ <= min_errors_)
			{
				last_error_message_ms_ = now;
				failuresSinceLastOutput_ = failure_counter_ - failuresOnLastOutput_;
				successesSinceLastOutput_ = success_counter_ - successesOnLastOutput_;
				failuresOnLastOutput_ = failure_counter_;
				successesOnLastOutput_ = success_counter_;

				// Fehlermeldung ausgeben
				if (logFunction_ && parent_) (parent_->*logFunction_)();
			}
		}

		return result;
	}

	/// \brief Fehlerüberwacher zurücksetzen
	///
	/// Fehler- und Erfolgszähler sowie letzten Fehlerzeitpunkt zurücksetzen
	void reset()
	{
		failure_counter_ = 0;
		success_counter_ = 0;
		failuresOnLastOutput_ = 0;
		successesOnLastOutput_ = 0;
		failuresSinceLastOutput_ = 0;
		successesSinceLastOutput_ = 0;
		last_error_message_ms_ = 0;
	}


	// Parameter auslesen

	/// \brief minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	const SystemTime getMinInterval() const { return SystemTime::fromMsec(interval_ms_); }

	unsigned getMinErrors() const { return min_errors_; }

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
	constexpr const SystemTime getLastErrorMessageTime() const { return SystemTime::fromMsec(last_error_message_ms_);}

private:
	/// Funktion, die zur Ausgabe aufgerufen wird
	LogFunction logFunction_;

	const T* parent_;

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
/// ähnliche Sachen benutzt werden. Dazu wird im Konstruktor ein
/// Pointer zu einer Funktion übergeben, die aufgerufen wird sobald
/// eine Ausgabe nötig ist.
///
/// Diese Klasse ist gegenüber ErrorObserver deutlich leichtgewichtiger
/// im Speicherverbrauch (üblicherweise 16 Byte). Dafür stellt sie
/// als einzige Information die Anzahl der seit der letzten Ausgabe
/// aufgetretenen Fehler zur Verfügung.
///
/// In folgenden Beispiel liegen mindestens 25 Sekunden zwischen jeder Fehlermeldung:
/// \code
///		// Definition, direkte Logausgabe mittels Lambda-Funktion
///		static CheapErrorObserver logger(SystemTime::fromSec(25), [](uint16_t failures) {
///			turag_errorf("Konnte %u mal keine GPS-Position lesen.", failures);
///		});
///
///     void getGpsPos() {
///       Point point;
///       if (logger.log(GPS::getPosition(&point))) {
///         // do something with GPS data
///       }
///     }
/// \endcode
template <class T>
class CheapErrorObserver
{
public:
	/// \brief Funktion zum Ausgeben einer Fehlermeldung.
	/// \param failureSinceLastOutput Enthält die Anzahl der aufgetretenen
	/// Fehler seit der letzten Ausgabe.
	using LogFunction = void(T::*)(uint16_t failureSinceLastOutput) const;

	// ctors

	/// Fehlerüberwacher erstellen
	/// \param min_interval minimales Intervall in dem Fehlermeldungen
	/// ausgegeben werden dürfen. Der maximal erlaubte Wert ist 65536 ms.
	/// \param logFunction Pointer zu einer Funktion, die die Logmeldung ausgibt.
	/// Dies kann eine Lambda-Funktion (ohne Capturing) sein.
	CheapErrorObserver(const T* parent, SystemTime min_interval, LogFunction logFunction) :
		logFunction_(logFunction), parent_(parent)
	{
		interval_ms_ = static_cast<uint16_t>(std::min(min_interval.toMsec(), 0xffffU));
	}

	// verändern

	/// \brief Ergebnis von letzter Operation loggen
	/// \param result Ergebnis von letzter Operation
	/// \return \p result
	///
	/// Ruft die im Konstruktor übergebene Funktion auf, wenn eine
	/// Ausgabe nötig ist. Das Ergebnis der Funktion wird unverändert
	/// zurückgegeben, sodass diese Funktion ohne Anpassung in
	/// bestehenden Code eingefügt werden kann, um eine Fehlerausgabe
	/// zu erhalten.
	bool log(bool result)
	{
		if (!result)
		{
			uint32_t now_ms = static_cast<uint32_t>(SystemTime::now().toMsec());
			++failuresSinceLastOutput_;

			if ((last_error_message_ms_ == 0) || (failuresSinceLastOutput_ == 0xffff) || (now_ms - last_error_message_ms_ > interval_ms_))
			{
				if (logFunction_ && parent_) (parent_->*logFunction_)(failuresSinceLastOutput_);

				last_error_message_ms_ = now_ms;
				failuresSinceLastOutput_ = 0;
			}
		}

		return result;
	}


	/// \brief Fehlerüberwacher zurücksetzen
	///
	/// Fehlerzähler sowie letzten Fehlerzeitpunkt zurücksetzen.
	void reset()
	{
		last_error_message_ms_ = 0;
		failuresSinceLastOutput_ = 0;
	}



private:
	/// Funktion, die zur Ausgabe aufgerufen wird
	LogFunction logFunction_;

	const T* parent_;

	/// letzter Fehler
	uint32_t last_error_message_ms_ = 0;

	/// Fehlerzähler
	uint16_t failuresSinceLastOutput_ = 0;

	/// minimales Intervall in dem Fehlermeldungen ausgegeben werden dürfen
	uint16_t interval_ms_;
};



} // namespace Debug
} // namespace TURAG

#endif // ERROROBSERVER_H
