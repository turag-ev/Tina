#ifndef TINA_DEBUG_PRINT_H
#define TINA_DEBUG_PRINT_H

#include <tina/debugprint.h>
#include <tina/time.h>
#include "defines.h"
#include "log-source.h"
#include "debug_time.h"

#ifdef __cplusplus
extern "C" {
#endif


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
# define turag_debug_print_logsources(x,y) TURAG_MACRO_NOOP
#endif

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

#if TURAG_DEBUG_LEVEL > 0
# if TURAG_PRINT_GAMETIME_AUTOMATIC == 1
#  define turag_system_printf(format, args...)  \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }             \
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX ";;" format TURAG_DEBUG_NEWLINE, ##args);				\
	TURAG_MACRO_END
#  define turag_system_print(msg)   \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }    \
		turag_debug_puts(TURAG_DEBUG_LINE_PREFIX ";;" msg TURAG_DEBUG_NEWLINE);					\
	TURAG_MACRO_END
# else
#  define turag_system_printf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX ";;" format TURAG_DEBUG_NEWLINE, ##args)
#  define turag_system_print(msg)   \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX ";;" msg TURAG_DEBUG_NEWLINE)
# endif
#else
# define turag_system_printf(format, args...) TURAG_MACRO_NOOP
# define turag_system_print(msg) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

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

#if TURAG_DEBUG_LEVEL > 0
# if TURAG_PRINT_GAMETIME_AUTOMATIC == 1
#  define turag_errorf(format, args...) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }              \
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args); \
	TURAG_MACRO_END
#  define turag_error(msg) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
		turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX msg TURAG_DEBUG_NEWLINE); \
	TURAG_MACRO_END
# else
#  define turag_errorf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
#  define turag_error(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_ERROR_PREFIX msg TURAG_DEBUG_NEWLINE)
# endif
#else
# define turag_errorf(format, args...) TURAG_MACRO_NOOP
# define turag_error(msg) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

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
# if TURAG_PRINT_GAMETIME_AUTOMATIC == 1
#  define turag_criticalf(format, args...) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }              \
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX format TURAG_DEBUG_NEWLINE, ##args); \
	TURAG_MACRO_END
#  define turag_critical(msg) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
		turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX msg TURAG_DEBUG_NEWLINE); \
	TURAG_MACRO_END
# else
#  define turag_criticalf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
#  define turag_critical(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_CRITICAL_PREFIX msg TURAG_DEBUG_NEWLINE)
# endif
#else
# define turag_criticalf(format, args...) TURAG_MACRO_NOOP
# define turag_critical(msg) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

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
# if TURAG_PRINT_GAMETIME_AUTOMATIC == 1
#  define turag_warningf(format, args...) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX format TURAG_DEBUG_NEWLINE,  ##args); \
	TURAG_MACRO_END
#  define turag_warning(msg) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
		turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX msg TURAG_DEBUG_NEWLINE); \
	TURAG_MACRO_END
# else
#  define turag_warningf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX format TURAG_DEBUG_NEWLINE,  ##args)
#  define turag_warning(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_WARN_PREFIX msg TURAG_DEBUG_NEWLINE)
# endif
#else
# define turag_warningf(format, args...) TURAG_MACRO_NOOP
# define turag_warning(msg) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

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
# if TURAG_PRINT_GAMETIME_AUTOMATIC == 1
#  define turag_infof(format, args...) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX format TURAG_DEBUG_NEWLINE, ##args); \
	TURAG_MACRO_END
#  define turag_info(msg) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
		turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX msg TURAG_DEBUG_NEWLINE); \
	TURAG_MACRO_END
# else
#  define turag_infof(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
#  define turag_info(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_INFO_PREFIX msg TURAG_DEBUG_NEWLINE)
# endif
#else
# define turag_infof(format, args...) TURAG_MACRO_NOOP
# define turag_info(msg) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

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
# if TURAG_PRINT_GAMETIME_AUTOMATIC == 1
#  define turag_debugf(format, args...) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX format TURAG_DEBUG_NEWLINE, ##args); \
	TURAG_MACRO_END
#  define turag_debug(msg) \
	TURAG_MACRO_BEGIN \
		if (turag_debug_print_gametime_auto_output_enabled) {turag_print_system_gametime(); }               \
		turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX msg TURAG_DEBUG_NEWLINE); \
	TURAG_MACRO_END
# else
#  define turag_debugf(format, args...) \
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX format TURAG_DEBUG_NEWLINE, ##args)
#  define turag_debug(msg) \
    turag_debug_puts(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_LOG_SOURCE TURAG_DEBUG_DEBUG_PREFIX msg TURAG_DEBUG_NEWLINE)
# endif
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


#ifdef __cplusplus
}
#endif


#endif // TINA_DEBUG_PRINT_H
