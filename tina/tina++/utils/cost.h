#ifndef COST_H
#define COST_H

#include <limits>
#include <tina++/tina.h>

namespace TURAG {

/// \ingroup Misc
/// \brief Darstellung für Kosten.
///
/// Klasse zum einfachen rechnen mit Kosten. Kosten können nur positiv sein, dabei
/// gibt es zwei spezielle Kosten:
///  * \ref COST_INFINITY, zur Darstellung von unendlichen hohen Kosten.
///    Wird benutzt, wenn Aktion nicht ausführbar ist.
///  * \ref COST_MAX, zur Darstellung von sehr hohen endlichen Kosten.
///    Wird benutzt wenn Aktion zwar benutzt werden kann, aber nur wenn nichts
///    anderes geht. Aber auch wenn Wert von Kosten nicht mehr von Ganzzahl dargestellt werden kann (Integer-Overflow).
/// \code{.cpp}
/// Cost x = Cost(42); // Aktion mit Kosten 42
/// x += 5; // Kosten addieren
/// x -= 100; // Kosten abziehen (x hat Kosten Null)
/// x += COST_MAX; // Kosten maximal
/// x += 300; // Kosten immer noch maximal
/// x += COST_INFINITY; // unendliche Kosten
/// x -= 100; // immer noch unendliche Kosten
/// \endcode
///
/// Die Kosten dürfen nicht den Wert von std::numeric_limits<unsigned>::max() - 2
/// überschreiten. Wird bei einer Operation (Addieren, ...) der Wert überschritten,
/// werden die Kosten auf \ref COST_MAX gesetzt.
class Cost {
public:
#ifndef __DOXYGEN__
    static constexpr unsigned INFINITY_ = std::numeric_limits<unsigned>::max();
    static constexpr unsigned MAX_ = INFINITY_ - 1;
#endif

    /// \brief unendliche Kosten erstellen
    constexpr
    Cost() :
        cost_(INFINITY_)
    { }

    /// \brief Kosten mit bestimmten Wert erstellen
    constexpr explicit
    Cost(unsigned arg) :
        cost_(arg)
    { }

    /// \{
    /// \brief Kosten addieren
    Cost operator+=(int arg) {
        if (arg >= 0) {
            return *this += unsigned(arg);
        } else {
            return *this -= unsigned(-arg);
        }
    }

    Cost operator+=(unsigned arg);

    Cost operator+=(Cost other) {
        return *this += other.cost_;
    }
		/// \}

    /// \{
    /// \brief Kosten subtrahieren
    Cost operator-=(int arg) {
        if (arg >= 0) {
            return *this -= unsigned(arg);
        } else {
            return *this += unsigned(-arg);
        }
    }
    Cost operator-=(Cost arg) {
        return *this -= arg.toUnsigned();
    }
    Cost operator-=(unsigned arg);
    /// \}

    /// \{
    /// \brief Wert in unsigned Variable ausgeben
    constexpr explicit
    operator unsigned() const {
        return cost_;
    }

    constexpr
    unsigned toUnsigned() const {
        return cost_;
    }
    /// \}

private:
    unsigned cost_;
};


/// \relates Cost
/// \brief Kosten vergleichen
constexpr bool operator< (Cost lhs, Cost rhs)     { return lhs.toUnsigned() <  rhs.toUnsigned(); }
/// \relates Cost
/// \brief Kosten vergleichen
constexpr bool operator> (Cost lhs, Cost rhs)     { return lhs.toUnsigned() >  rhs.toUnsigned(); }
/// \relates Cost
/// \brief Kosten vergleichen
constexpr bool operator<=(Cost lhs, Cost rhs)     { return lhs.toUnsigned() <= rhs.toUnsigned(); }
/// \relates Cost
/// \brief Kosten vergleichen
constexpr bool operator>=(Cost lhs, Cost rhs)     { return lhs.toUnsigned() >= rhs.toUnsigned(); }
/// \relates Cost
/// \brief Kosten vergleichen
constexpr bool operator==(Cost lhs, Cost rhs)     { return lhs.toUnsigned() == rhs.toUnsigned(); }
/// \relates Cost
/// \brief Kosten vergleichen
constexpr bool operator!=(Cost lhs, Cost rhs)     { return lhs.toUnsigned() != rhs.toUnsigned(); }

/// \{
/// \relates Cost
/// \brief Kosten vergleichen
/// \bug unsigned-Wert darf nicht die Größe von <code>std::numeric_limits<unsigned>::max() - 2</code> überschreiten
constexpr bool operator< (Cost lhs, unsigned rhs) { return lhs.toUnsigned() <  rhs;              }
constexpr bool operator< (unsigned lhs, Cost rhs) { return              lhs <  rhs.toUnsigned(); }

constexpr bool operator> (Cost lhs, unsigned rhs) { return lhs.toUnsigned() >  rhs;              }
constexpr bool operator> (unsigned lhs, Cost rhs) { return              lhs >  rhs.toUnsigned(); }

constexpr bool operator<=(Cost lhs, unsigned rhs) { return lhs.toUnsigned() <= rhs;              }
constexpr bool operator<=(unsigned lhs, Cost rhs) { return              lhs <= rhs.toUnsigned(); }

constexpr bool operator>=(Cost lhs, unsigned rhs) { return lhs.toUnsigned() >= rhs;              }
constexpr bool operator>=(unsigned lhs, Cost rhs) { return              lhs >= rhs.toUnsigned(); }

constexpr bool operator==(Cost lhs, unsigned rhs) { return lhs.toUnsigned() == rhs;              }
constexpr bool operator==(unsigned lhs, Cost rhs) { return              lhs == rhs.toUnsigned(); }

constexpr bool operator!=(Cost lhs, unsigned rhs) { return lhs.toUnsigned() != rhs;              }
constexpr bool operator!=(unsigned lhs, Cost rhs) { return              lhs != rhs.toUnsigned(); }
/// \}

/// \relates Cost
/// \{
/// \brief Kosten addieren
inline Cost operator+ (Cost lhs, Cost rhs)     { return Cost(lhs) += rhs.toUnsigned(); }
inline Cost operator+ (Cost lhs, unsigned rhs) { return Cost(lhs) += rhs;              }
inline Cost operator+ (unsigned lhs, Cost rhs) { return Cost(rhs) += lhs;              }
/// \}

/// \relates Cost
/// unendlich große Kosten (Weg nicht benutzbar)
constexpr Cost COST_INFINITY = Cost(Cost::INFINITY_);

/// \relates Cost
/// größte Kosten ohne ausgeschlossen zu werden
constexpr Cost COST_MAX = Cost(Cost::MAX_);

} // namespace TURAG

#endif // COST_H
