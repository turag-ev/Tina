#ifndef TINA_DEBUG_PRINT_H
#define TINA_DEBUG_PRINT_H

#include <tina/debugprint.h>
#include "defines.h"

/// \addtogroup Debug
/// \{
/// \defgroup Debugprint Meldungsausgabe
/// \{

/// Prefix für Fehlermeldung
#define TURAG_DEBUG_ERROR_PREFIX    "!"

/// Prefix für kritische Debugmeldungen
#define TURAG_DEBUG_CRITICAL_PREFIX "!"

/// Prefix für Warnungen
#define TURAG_DEBUG_WARN_PREFIX     "?"

/// Prefix für informative Meldungen
#define TURAG_DEBUG_INFO_PREFIX     "-"

/// Prefix für Debugmeldungen
#define TURAG_DEBUG_DEBUG_PREFIX    "_"

#ifdef DOXYGEN

/// \brief Gibt Systemmeldungen formatiert aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 0 wird tatsächlich Meldung ausgegeben.
/// \param format Formatstring entsprechend printf aus stdio.h
/// \param ... Formatargumente entsprechend Formatstring
void turag_system_printf(const char* format, ...);

/// \brief Gibt Systemmeldungen aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 0 wird tatsächlich Meldung ausgegeben.
/// \param msg Zeichenkette, die ausgeben werden soll
void turag_system_print(const char* msg);

#else // DOXYGEN

#if TURAG_DEBUG_LEVEL > 0
# define turag_system_printf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX ";;" format TURAG_DEBUG_NEWLINE, ##args)
# define turag_system_print(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX ";;" msg TURAG_DEBUG_NEWLINE)
#else
# define turag_system_printf(format, args...) while(0)
# define turag_system_print(msg) while(0)
#endif

#endif // DOXYGEN

#ifdef DOXYGEN

/// \brief Gibt Fehlermeldungen formatiert aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 0 wird tatsächlich Meldung ausgegeben.
/// \param format Formatstring entsprechend printf aus stdio.h
/// \param ... Formatargumente entsprechend Formatstring
void turag_errorf(const char* format, ...);

/// \brief Gibt Fehlermeldungen aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 0 wird tatsächlich eine Meldung ausgegeben.
/// \param msg Zeichenkette, die ausgeben werden soll
void turag_error(const char* msg);

#else // DOXYGEN

#if TURAG_DEBUG_LEVEL > 0
# define turag_errorf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define turag_error(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_errorf(format, args...) while(0)
# define turag_error(msg) while(0)
#endif

#endif // DOXYGEN

#ifdef DOXYGEN

/// \brief Gibt Meldungen über kritischen Zustand formatiert aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 1 wird tatsächlich Meldung ausgegeben.
/// \param format Formatstring entsprechend printf aus stdio.h
/// \param ... Formatargumente entsprechend Formatstring
void turag_criticalf(const char* format, ...);

/// \brief Gibt Meldungen über kritischen Zustand aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 1 wird tatsächlich Meldung ausgegeben.
/// \param msg Zeichenkette, die ausgeben werden soll
void turag_critical(const char* msg);

#else // DOXYGEN

#if TURAG_DEBUG_LEVEL > 1
# define turag_criticalf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define turag_critical(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_criticalf(format, args...) while(0)
# define turag_critical(msg) while(0)
#endif

#endif // DOXYGEN

#ifdef DOXYGEN

/// \brief Gibt Warningsmeldungen formatiert aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 2 wird tatsächlich Meldung ausgegeben.
/// \param format Formatstring entsprechend printf aus stdio.h
/// \param ... Formatargumente entsprechend Formatstring
void turag_warningf(const char* format, ...);

/// \brief Gibt Warningsmeldungen aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 2 wird tatsächlich Meldung ausgegeben.
/// \param msg Zeichenkette, die ausgeben werden soll
void turag_warning(const char* msg);

#else // DOXYGEN

#if TURAG_DEBUG_LEVEL > 2
# define turag_warningf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX format TURAG_DEBUG_NEWLINE,  ##args)
# define turag_warning(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_warningf(format, args...) while(0)
# define turag_warning(msg) while(0)
#endif

#endif // DOXYGEN

#ifdef DOXYGEN

/// \brief Gibt informative Meldungen formatiert aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 3 wird tatsächlich Meldung ausgegeben.
/// \param format Formatstring entsprechend printf aus stdio.h
/// \param ... Formatargumente entsprechend Formatstring
void turag_infof(const char* format, ...);

/// \brief Gibt informative Meldungen aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 3 wird tatsächlich Meldung ausgegeben.
/// \param msg Zeichenkette, die ausgeben werden soll
void turag_info(const char* msg);

#else // DOXYGEN

#if TURAG_DEBUG_LEVEL > 3
# define turag_infof(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define turag_info(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_infof(format, args...) while(0)
# define turag_info(msg) while(0)
#endif

#endif // DOXYGEN

#ifdef DOXYGEN

/// \brief Gibt Warningsmeldungen formatiert aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 4 wird tatsächlich Meldung ausgegeben.
/// \warning Standardmäßig ist TURAG_DEBUG_LEVEL gleich 4, womit Meldungen von
///         \ref turag_debugf ignoriert werden. Wenn turag_debugf für eine Datei
///         aktiviert werden sollen, sollte folgende Zeile vorallen Headerincludes stehen
///         \code #define TURAG_DEBUG_LEVEL 5\endcode
/// \param format Formatstring entsprechend printf aus stdio.h
/// \param ... Formatargumente entsprechend Formatstring
void turag_debugf(const char* format, ...);

/// \brief Gibt Warningsmeldungen aus.
///
/// Zur Formatierungen weitere Informationen unter <a href="http://en.cppreference.com/w/c/io/fprintf">printf</a>.
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 4 wird tatsächlich Meldung ausgegeben.
/// \warning Standardmäßig ist TURAG_DEBUG_LEVEL gleich 4, womit Meldungen von
///         \ref turag_debug ignoriert werden. Wenn \ref turag_debug für eine Datei
///         aktiviert werden sollen, sollte folgende Zeile vorallen Headerincludes stehen:
///         \code #define TURAG_DEBUG_LEVEL 5\endcode
/// \param msg Zeichenkette, die ausgeben werden soll
void turag_debug(const char* msg);

#else // DOXYGEN

#if TURAG_DEBUG_LEVEL > 4
# define turag_debugf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define turag_debug(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_debugf(format, args...) while(0)
# define turag_debug(msg) while(0)
#endif

#endif // DOXYGEN

/// Gibt Zeichenketten entsprechend in englischer Sprache für Boolean zurück.
/// \param boolean Boolean-Wert
/// \retval "yes", wenn \p b wahr
/// \retval "no", sonst
#define turag_bool_string(boolean) ((boolean)?"yes":"no")

/// Gibt Zeichenketten entsprechend in deutscher Sprache für Boolean zurück.
/// \param boolean Boolean-Wert
/// \retval "ja", wenn \p b wahr
/// \retval "nein", sonst
#define turag_bool_string_de(boolean) ((boolean)?"ja":"nein")

/// \}
/// \}

#endif // TINA_DEBUG_PRINT_H
