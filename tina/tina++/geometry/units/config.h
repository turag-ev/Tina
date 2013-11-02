#ifndef TINAPP_UNITS_CONFIG_H
#define TINAPP_UNITS_CONFIG_H

namespace TURAG {
namespace Units {

/// Typ für Gleitkommazahlen
typedef float Real;

/// Typ mit dem Variablen gespeichert werden sollen
typedef Real Value;

/// Typ mit dem Einheitenumwandlungsfaktoren gespeichert werden sollen
typedef Real UnitFactor;

/// Angabe der Relation zwischen gespeichertem Wert und SI-Einheiten
/// Die Angabe fordert den Wert um die SI-Einheit in den internen Wert durch Multipizieren zu konvertieren
constexpr UnitFactor FROM_SI_CONVERT_FACTOR_LENGTH = 1000.f; // mm
constexpr UnitFactor FROM_SI_CONVERT_FACTOR_ANGLE  =    1.f; // rad
constexpr UnitFactor FROM_SI_CONVERT_FACTOR_TIME   =    1.f; // s

} // namespace Units
} // namespace TURAG

#endif // TINAPP_UNITS_CONFIG_H

