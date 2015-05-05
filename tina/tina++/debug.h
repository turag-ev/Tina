/**
 * \file tina++/debug.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief Utils for printing debug information
 *
 */

#ifndef TINAPP_DEBUG_H
#define TINAPP_DEBUG_H

#include <tina/debug/print.h>
#include <tina/debug/image.h>
#include <tina++/debug/binary.h>
#include <tina++/debug/graph.h>

namespace TURAG {

/// \addtogroup Debug
/// \{
/// \defgroup Debugprint Meldungsausgabe
/// \{

/// \copydoc turag_bool_string
inline const char* bool_str(bool boolean) {
    return boolean ? "yes" : "no";
}

/// \copydoc turag_bool_string_de
inline const char* bool_str_de(bool boolean) {
    return boolean ? "ja" : "nein";
}

/// \}
/// \}

} // namespace TURAG

#endif // UTILS_DEBUG_H

