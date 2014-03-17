#ifndef TINAPP_RANGE_RANGE_H
#define TINAPP_RANGE_RANGE_H

#include "iterator_range.h"

namespace TURAG {

/// \addtogroup Container
/// \{
///
/// \defgroup Ranges Bereiche (Ranges) [C++]
///
/// Klasse und Funktionen zur Verarbeitung von Bereichen in Containern.
/// Ein Bereich ist definiert durch die beiden Funktionen \a begin und \a end,
/// die beide einen Iterator zurückgeben. Will man durch die Bereich durchiterieren
/// so sieht das folgendermaßen aus:
/// \code{.cpp}
/// for (auto iter = range.begin(); iter != range.end(); iter++) {
///     // mit *iter kann auf die Variable zugeriefen werden
/// }
/// \endcode
/// oder
/// \code{.cpp}
/// for (auto& value : range)
///   // in value ist Wert
/// }
/// \endcode
///
/// \}

} // namespace TURAG

#endif // TINAPP_RANGE_RANGE_H
