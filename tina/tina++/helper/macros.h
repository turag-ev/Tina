#ifndef TINAPP_HELPER_MACROS_H
#define TINAPP_HELPER_MACROS_H

#include "normalize.h"
#include <tina/helper/macros.h>

/// \defgroup Utils Werkzeuge [C/C++]
/// \brief Makros, Funktionen und Klassen, die das Programmieren unterstützen
/// \{

/// \brief Verbiete kopieren von Klasse (Definition in Klassendefinition)
///
/// \code{.cpp}
///   class A {
///     NOT_COPYABLE(A);
///   };
///
///   A a;
///   A b = a; // Fehler: Variable a nicht kopierbar.
/// \endcode
#define NOT_COPYABLE(ClassName) \
  ClassName(const ClassName&) = delete; \
  ClassName& operator=(const ClassName&) = delete

/// \brief Standardkopierfunktionen definieren
#define COPYABLE(ClassName) \
  constexpr ClassName(const ClassName&) = default; \
  ClassName& operator=(const ClassName&) = default

/// \brief Verbiete verschieben (kopiert u.U. wenn Kopierfunktionen vorhanden)
#define NOT_MOVABLE(ClassName) \
  ClassName(ClassName&&) = delete; \
  ClassName& operator=(ClassName&&) = delete

/// \brief Standardverschiebfunktionen definieren
#define MOVABLE(ClassName) \
  constexpr ClassName(ClassName&&) = default; \
  ClassName& operator=(ClassName&&) = default

/// Helfermakro zur Benutzung mit std::enable_if
///
/// \code{.cpp}
///   template<T, REQUIRES(std::is_integral<T>) >
///   T add_int(T a, T b) {
///       return a + b;
///   }
/// \endcode
#define REQUIRES(...) typename std::enable_if<__VA_ARGS__::value, bool>::type = false
#define ENABLE_IF(...) typename std::enable_if<__VA_ARGS__::value>::type

/// Helfermakro zur Benutzung mit std::enable_if (ausführlichere Version)
/// Im Gegensatz zu REQUIRES kann diese Variante genutzt werden um auch
/// Booleasche Operatoren zu benutzen
/// \code{.cpp}
///   template<T, REQUIRES2(std::is_integral<T>::value) >
///   T add_int(T a, T b) {
///       return a + b;
///   }
/// \endcode
#define REQUIRES2(...) typename std::enable_if<(__VA_ARGS__), bool>::type = false

/// \}

#endif // TINAPP_HELPER_MACROS_H
