#ifndef TINAPP_UTILS_TIMER_H
#define TINAPP_UTILS_TIMER_H

#include <tina++/time.h>

namespace TURAG {

/// \ingroup Misc
/// \brief Stopuhr
///
/// Werkzeug zum Messen von Aufführzeiten von Funktionen.
/// \code{.cpp}
/// Stopwatch stopwatch;
/// func(...);
/// stopwatch.elaped(); // gibt verstrichene Zeit zurück
/// \endcode
class Stopwatch {
public:
    /// \brief Stopuhr erstellen und starten
    Stopwatch() :
        start_time_(SystemTime::now())
    { }

    /// \brief Stopuhr zurücksetzen und starten
    void reset() {
        start_time_ = SystemTime::now();
    }

    /// \brief verstrichene Zeit
    SystemTime elapsed() const {
        return SystemTime::now() - start_time_;
    }

private:
    SystemTime start_time_;
};

} // namespace TURAG

#endif // TINAPP_UTILS_TIMER_H
