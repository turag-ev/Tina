#ifndef TINA_DEBUG_LOGSOURCE_H
#define TINA_DEBUG_LOGSOURCE_H

#include "defines.h"

#ifndef TURAG_DEBUG_LOG_SOURCE
/// \brief Debugquelle.
///
/// \ingroup Debug
///
/// Sollte in erster Zeile von Quelldatei vor den Includes definiert werden.
/// \warning Niemals in Headerdatei definieren.
///
/// \code
/// // source.cpp
/// // Logquelle auf A setzen
/// #define TURAG_DEBUG_LOG_SOURCE "A"
///
/// // Includes:
/// #include <tina++/debug.h>
///
/// // Quellcode
/// ...
/// \endcode
# define TURAG_DEBUG_LOG_SOURCE "_"
# warning Makro TURAG_DEBUG_LOG_SOURCE nicht definiert!
#endif

#endif // TINA_DEBUG_LOGSOURCE_H
