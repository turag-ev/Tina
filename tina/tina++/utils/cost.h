#ifndef COST_H
#define COST_H

#include <limits>
#include <tina++/tina.h>
#include <cstdio>

namespace TURAG {

/// \ingroup Misc
/// \brief Darstellung für Kosten.
///
/// Klasse zum einfachen rechnen mit Kosten. Kosten könne nur positiv sein, dabei
/// gibt es zwei spezielle Kosten:
///  * \ref COST_INFINITY, zur Darstellung von unendlichen hohen Kosten.
///    Wird benutzt, wenn Aktion nicht ausführbar ist.
///  * \ref COST_MAX, zur Darstellung von sehr hohen endlichen Kosten.
///    Wird benutzt wenn Aktion zwar benutzt werden kann, aber nur wenn nichts
///    anderes geht. Sowohl wenn Wert von Kosten nicht mehr von Ganzzahl dargestellt werden kann (Overflow).
/// \code{.cpp}
/// Cost x = Cost(42); // Aktion mit Kosten 42
/// x += 5; // Kosten addieren
/// x -= 100; // Kosten abziehen (x hat Kosten Null)
/// x += COST_MAX; // Kosten maximal
/// x += 300; // Kosten immer noch maximal
/// x += COST_INIFINTY; // unendliche Kosten
/// x -= 100; // immer noch unendliche Kosten
/// \endcode
class Cost {
public:
#ifndef DOXYGEN
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

    Cost operator+(Cost other) {
        return Cost(*this) += other.cost_;
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
    /// \brief Kosten vergleichen
    constexpr bool operator< (Cost other)     const { return cost_ <  other.cost_; }
    constexpr bool operator< (unsigned other) const { return cost_ <  other;       }
    constexpr bool operator> (Cost other)     const { return cost_ >  other.cost_; }
    constexpr bool operator> (unsigned other) const { return cost_ >  other;       }
    constexpr bool operator<=(Cost other)     const { return cost_ <= other.cost_; }
    constexpr bool operator<=(unsigned other) const { return cost_ <= other;       }
    constexpr bool operator>=(Cost other)     const { return cost_ >= other.cost_; }
    constexpr bool operator>=(unsigned other) const { return cost_ >= other;       }
    constexpr bool operator==(Cost other)     const { return cost_ == other.cost_; }
    constexpr bool operator==(unsigned other) const { return cost_ == other;       }
    constexpr bool operator!=(Cost other)     const { return cost_ != other.cost_; }
    constexpr bool operator!=(unsigned other) const { return cost_ != other;       }
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

/// unendlich große Kosten (Weg nicht benutzbar)
static constexpr Cost COST_INFINITY = Cost(Cost::INFINITY_);

/// größte Kosten ohne ausgeschlossen zu werden
static constexpr Cost COST_MAX = Cost(Cost::MAX_);

} // namespace TURAG

#endif // COST_H
