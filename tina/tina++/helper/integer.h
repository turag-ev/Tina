#ifndef TINAPP_HELPER_INTEGER_H
#define TINAPP_HELPER_INTEGER_H

#include <tina/tina.h>

namespace TURAG {

/// \addtogroup Utils
/// \{

/// \brief Gibt zurück ob Zahl eine Potenz von zwei ist.
///
/// \param n Vorzeichenlose Zahl
/// \return \a true, wenn \a n Potenz von 2, sonst \a false
constexpr bool power_of_two(unsigned n) {
    return (n & (n - 1)) == 0;
}

/// \brief Gibt nächst größere Potenz von zwei aus.
///
/// \param n Vorzeichenlose Zahl
/// \return nächst größere Potenz von zwei
constexpr unsigned next_power_of_two(unsigned n) {
    return power_of_two(n) ? n : next_power_of_two(n + 1);
}

/// \brief Gibt die mindestens notwendigen Bytes zum Speichern von \a n Bits zurück.
/// \param n Anzahl der Bits
/// \return Anzahl der Bytes, die für das Speichern von \a n Bits nötig sind
constexpr unsigned least_bytes_needed(unsigned n) {
    return (n + 7) / 8;
}

/// \brief Typ für vorzeichenlose Ganzzahlen
/// \tparam Bytes Anzahl der Bytes des Ganzzahltypes (nur 0, 1, 2, 4 und 8 erlaubt)
template<std::size_t Bytes>
struct unsigned_integer_bytes { };

#ifndef __DOXYGEN__
template<> struct unsigned_integer_bytes<0> { typedef  void type; };
template<> struct unsigned_integer_bytes<1> { typedef  uint8_t type; };
template<> struct unsigned_integer_bytes<2> { typedef uint16_t type; };
template<> struct unsigned_integer_bytes<4> { typedef uint32_t type; };
template<> struct unsigned_integer_bytes<8> { typedef uint64_t type; };
#endif // __DOXYGEN__

/// \brief Typ für vorzeichenlose Ganzzahlen
/// \tparam Bits Anzahl der Mindestanzahl an Bits des Ganzzahltypes (nur 0-64 erlaubt)
template<std::size_t Bits>
struct unsigned_integer_least_bits {
  typedef typename unsigned_integer_bytes<least_bytes_needed(Bits)>::type type;
};

/// \}

} // namespace TURAG

#endif // TINAPP_HELPER_INTEGER_H
