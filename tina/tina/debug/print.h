#ifndef TINA_DEBUG_PRINT_H
#define TINA_DEBUG_PRINT_H

#include <tina/time.h>
#include <tina/debugprint.h>
#include "defines.h"
#include "log-source.h"

TURAG_C_API_BEGIN

/// \addtogroup Debug
/// \{
/// \defgroup Debugprint Meldungsausgabe
/// \brief Ausgabe formatierter Textmeldungen.
/// \{

#if TURAG_DEBUG_LEVEL > 0 || defined(__DOXYGEN__)

/**
 * @brief Enthält die Bezeichnungen der Logquellen.
 *
 * Dieses Array enthält 127 Elemente.
 */
extern const char* turag_log_source_table[127];

/**
 * @brief Macht die vorhandenen Logquellen bekannt.
 *
 * Es wird der Inhalt der turag_log_source_table ausgegeben.
 */
void turag_debug_print_logsources(void);

#else
# define turag_debug_print_logsources() TURAG_MACRO_NOOP
#endif

#ifndef __DOXYGEN__

// gibt im Unterschied zu turag_debug_puts in Regelmäßigen Abständen die Spielzeit aus
void turag_log_puts(const char* s);

// gibt im Unterschied zu turag_debug_printf in Regelmäßigen Abständen die Spielzeit aus
void turag_log_printf(const char* fmt, ...) __attribute__ ((format(printf, 1, 2)));

void turag_debug_printf(const char* fmt, ...) __attribute__ ((format(printf, 1, 2)));

// turag_debug_* Implementation wählen
#if TURAG_DEBUG_LEVEL > 0
# if TURAG_PRINT_GAMETIME_AUTOMATIC
#  define turag_debug_puts_impl turag_log_puts
#  define turag_debug_printf_impl turag_log_printf
# else // TURAG_PRINT_GAMETIME_AUTOMATIC
#  define turag_debug_puts_impl turag_debug_puts
#  define turag_debug_printf_impl turag_debug_printf
# endif // TURAG_PRINT_GAMETIME_AUTOMATIC
#else
# define turag_debug_puts_impl(...) TURAG_MACRO_NOOP
# define turag_debug_printf_impl(...) TURAG_MACRO_NOOP
#endif

#endif

// turag_system_print(f)

#ifdef __DOXYGEN__

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

#else // __DOXYGEN__

#define turag_system_printf(format, args...) \
	turag_debug_printf_impl(TURAG_DEBUG_LINE_PREFIX ";;" format TURAG_DEBUG_NEWLINE, ##args)
#define turag_system_print(msg)   \
	turag_debug_puts_impl(TURAG_DEBUG_LINE_PREFIX ";;" msg TURAG_DEBUG_NEWLINE)

#endif // __DOXYGEN__

// turag_error(f)

#ifdef __DOXYGEN__

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

#else // __DOXYGEN__

#define turag_errorf(format, args...) \
	turag_debug_printf_impl(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
#define turag_error(msg) \
	turag_debug_puts_impl(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX msg TURAG_DEBUG_NEWLINE)

#endif // __DOXYGEN__

// turag_critical(f)

#ifdef __DOXYGEN__

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

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 1
# define turag_criticalf(format, args...) \
	turag_debug_printf_impl(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define turag_critical(msg) \
	turag_debug_puts_impl(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_criticalf(format, args...) TURAG_MACRO_NOOP
# define turag_critical(msg) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

// turag_warning(f)

#ifdef __DOXYGEN__

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

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 2
# define turag_warningf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX format TURAG_DEBUG_NEWLINE,  ##args)
# define turag_warning(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_warningf(format, args...) TURAG_MACRO_NOOP
# define turag_warning(msg) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

// turag_info(f)

#ifdef __DOXYGEN__

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

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 3
# define turag_infof(format, args...) \
	turag_debug_printf_impl(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define turag_info(msg) \
	turag_debug_puts_impl(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_infof(format, args...) TURAG_MACRO_NOOP
# define turag_info(msg) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

// turag_debug(f)

#ifdef __DOXYGEN__

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

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 4
# define turag_debugf(format, args...) \
	turag_debug_printf_impl(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
# define turag_debug(msg) \
	turag_debug_puts_impl(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX msg TURAG_DEBUG_NEWLINE)
#else
# define turag_debugf(format, args...) TURAG_MACRO_NOOP
# define turag_debug(msg) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

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

TURAG_C_API_END

#endif // TINA_DEBUG_PRINT_H
