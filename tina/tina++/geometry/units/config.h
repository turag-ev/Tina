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
/// Die Standardkonfiguration ist für Embedded-Prozessoren mit FPU geeigent:
/// \code
/// typedef float Real;
/// typedef Real Value;
/// typedef Real UnitFactor;
/// constexpr UnitFactor FROM_SI_CONVERT_FACTOR_LENGTH = 1000.f;
/// constexpr UnitFactor FROM_SI_CONVERT_FACTOR_ANGLE  =    1.f;
/// constexpr UnitFactor FROM_SI_CONVERT_FACTOR_TIME   =    1.f;
/// \endcode
/// Dabei werden Werte in 32-Bit-Gleitkommavariablen gespeichert. Intern wird
/// in Millimetern, Radiand und Sekunden gespeichert.
///
/// Wird ein Prozessor ohne FPU verwendet, ist die Verwendung von Gleitkommazahlen
/// sehr prozessorlastig. Deshalb wird dafür folgenden Konfiguration empfohlen, die
/// Ganzzahlwerte, Milliradiand und Millisekunden zum Speichern verwendet:
/// \code
/// typedef float Real;
/// typedef int Value;
/// typedef Real UnitFactor;
/// constexpr UnitFactor FROM_SI_CONVERT_FACTOR_LENGTH = 1000.f;
/// constexpr UnitFactor FROM_SI_CONVERT_FACTOR_ANGLE  = 1000.f;
/// constexpr UnitFactor FROM_SI_CONVERT_FACTOR_TIME   = 1000.f;
/// \endcode
/// \{

/// Typ für Gleitkommazahlen.
///
/// \par Standardwert
///   float
typedef float Real;

/// Typ mit dem Variablen gespeichert werden sollen.
///
/// \par Standardwert
///   Real
typedef Real Value;

/// Typ mit dem Einheitenumwandlungsfaktoren gespeichert werden sollen.
///
/// Sollte ein Gleit- oder Festkommatyp sein.
/// \par Standardwert
///   Real
typedef Real UnitFactor;

/// Angabe der Relation zwischen gespeichertem Wert und SI-Einheiten.
///
/// Die Angabe fordert den Wert um die SI-Einheit in den internen Wert durch Multipizieren zu konvertieren.
/// Standard ist 1000, was bedeutet, dass der Wert intern in Millimetern gespeichert wird.
constexpr UnitFactor FROM_SI_CONVERT_FACTOR_LENGTH = 1000.f; // mm

/// Angabe der Relation zwischen gespeichertem Wert und SI-Einheiten.
///
/// Die Angabe fordert den Wert um die SI-Einheit in den internen Wert durch Multipizieren zu konvertieren.
constexpr UnitFactor FROM_SI_CONVERT_FACTOR_ANGLE  =    1.f; // rad

/// Angabe der Relation zwischen gespeichertem Wert und SI-Einheiten.
///
/// Die Angabe fordert den Wert um die SI-Einheit in den internen Wert durch Multipizieren zu konvertieren.
constexpr UnitFactor FROM_SI_CONVERT_FACTOR_TIME   =    1.f; // s

/// \} \}

} // namespace Units
} // namespace TURAG

#endif // TINAPP_UNITS_CONFIG_H

