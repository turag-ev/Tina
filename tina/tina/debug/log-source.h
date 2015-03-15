#ifndef TINA_DEBUG_LOGSOURCE_H
#define TINA_DEBUG_LOGSOURCE_H

#include "defines.h"

// log source
#ifndef TURAG_DEBUG_LOG_SOURCE
/// \brief Debugquelle
///
/// Sollte über Makefile oder in Zeilen vor erstem Include umdefiniert werden.
# define TURAG_DEBUG_LOG_SOURCE "_"
# warning Makro TURAG_DEBUG_LOG_SOURCE nicht definiert!
#endif

#endif // TINA_DEBUG_LOGSOURCE_H
