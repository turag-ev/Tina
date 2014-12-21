#ifndef TINAPP_UNITS_CONFIG_H
#define TINAPP_UNITS_CONFIG_H

namespace TURAG {
namespace Units {

/// \addtogroup Units
/// \{
///
/// \defgroup UnitsConfig Konfiguration für Einheitensystem
/// \brief Konfiguration des Einheitensystems.
///
/// Die Konfiguration kann in der benutzererstellten tina/config.h eingetragen
/// werden.
///
/// Die Standardkonfiguration ist für Embedded-Prozessoren mit FPU geeigent:
/// \code
/// #define TURAG_UNITS_REAL_TYPE float
/// #define TURAG_UNITS_VALUE_TYPE float
///
/// #define TURAG_UNITS_FACTOR_TO_METER  1000.f;
/// #define TURAG_UNITS_FACTOR_TO_RADIAN    1.f;
/// #define TURAG_UNITS_FACTOR_TO_SECOND    1.f;
/// \endcode
/// Dabei werden Werte in 32-Bit-Gleitkommavariablen gespeichert. Intern wird
/// in Millimetern, Radiand und Sekunden gespeichert.
///
/// Wird ein Prozessor ohne FPU verwendet, ist die Verwendung von Gleitkommazahlen
/// sehr prozessorlastig. Deshalb wird dafür folgenden Konfiguration empfohlen, die
/// Ganzzahlwerte, Milliradiand und Millisekunden zum Speichern verwendet:
/// \code
/// #define TURAG_UNITS_REAL_TYPE float
/// #define TURAG_UNITS_VALUE_TYPE int
///
/// #define TURAG_UNITS_FACTOR_TO_METER  1000.f;
/// #define TURAG_UNITS_FACTOR_TO_RADIAN 1000.f;
/// #define TURAG_UNITS_FACTOR_TO_SECOND 1000.f;
/// \endcode
/// \{

#ifndef TURAG_UNITS_REAL_TYPE
/// \brief konfigurierbarer Typ für Gleitkommazahlen.
///
/// \par Standardwert
///   float
#define TURAG_UNITS_REAL_TYPE float
#endif

#ifndef TURAG_UNITS_VALUE_TYPE
/// \brief konfigurierbarer Typ mit dem Variablen gespeichert werden sollen.
///
/// \par Standardwert
///   float
#define TURAG_UNITS_VALUE_TYPE float
#endif

#ifndef TURAG_UNITS_FACTOR_TO_METER
/// Faktor zwischen internem zu speichernden Wert und Meter
///
/// \par Standardwert
///   1000.f
#define TURAG_UNITS_FACTOR_TO_METER 1000.f
#endif

#ifndef TURAG_UNITS_FACTOR_TO_RADIAN
/// Faktor zwischen internem zu speichernden Wert und Radian
///
/// \par Standardwert
///   1.f
#define TURAG_UNITS_FACTOR_TO_RADIAN 1.f
#endif

#ifndef TURAG_UNITS_FACTOR_TO_SECOND
/// Faktor zwischen internem zu speichernden Wert und Sekunden
///
/// \par Standardwert
///   1.f
#define TURAG_UNITS_FACTOR_TO_SECOND 1.f
#endif

/// Typ für Gleitkommazahlen
///
/// Konfigurierbar über Definition von \ref TURAG_UNITS_REAL_TYPE
///
/// \par Standardwert
///   float
typedef TURAG_UNITS_REAL_TYPE Real;

/// Typ mit dem Variablen gespeichert werden sollen.
///
/// Konfigurierbar über Definition von \ref TURAG_UNITS_VALUE_TYPE
///
/// \par Standardwert
///   float
typedef TURAG_UNITS_VALUE_TYPE Value;

/// Typ mit dem Einheitenumwandlungsfaktoren gespeichert werden sollen.
typedef Real UnitFactor;

#ifndef __DOXYGEN__
constexpr UnitFactor FROM_SI_CONVERT_FACTOR_LENGTH = TURAG_UNITS_FACTOR_TO_METER;
constexpr UnitFactor FROM_SI_CONVERT_FACTOR_ANGLE  = TURAG_UNITS_FACTOR_TO_RADIAN;
constexpr UnitFactor FROM_SI_CONVERT_FACTOR_TIME   = TURAG_UNITS_FACTOR_TO_SECOND;
#endif

/// \} \}

} // namespace Units
} // namespace TURAG

#endif // TINAPP_UNITS_CONFIG_H

