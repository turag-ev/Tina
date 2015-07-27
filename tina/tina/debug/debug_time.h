#ifndef DEBUG_TIME_H
#define DEBUG_TIME_H

#include "defines.h"
#include <tina/time.h>
#include <tina/debugprint.h>

TURAG_C_API_BEGIN

/// \addtogroup Debug
/// \{
/// \defgroup Debugprint Meldungsausgabe
/// \brief Ausgabe formatierter Textmeldungen.
/// \{

extern TuragSystemTime turag_debug_print_gametime_gamestarttime;
extern int turag_debug_print_gametime_auto_output_enabled;


#ifdef __DOXYGEN__

/// \brief Gibt die aktuelle Spielzeit aus.
///
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 0 wird die Spielzeit tatsächlich ausgegeben.
/// \param gametime Aktuelle Spielzeit
void turag_print_gametime(TuragSystemTime gametime);

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 0
# define turag_print_gametime(gametime) \
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX "GT%x" TURAG_DEBUG_NEWLINE, turag_ticks_to_ms(gametime))
#else
# define turag_print_gametime(gametime) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__


#ifdef __DOXYGEN__

/// \brief Gibt die aktuelle Spielzeit aus.
///
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 0 wird die Spielzeit tatsächlich ausgegeben.
/// \param gametime Aktuelle Spielzeit in Millisekunden.
void turag_print_gametime_ms(unsigned gametime);

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 0
# define turag_print_gametime_ms(gametime) \
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX "GT%x" TURAG_DEBUG_NEWLINE, gametime)
#else
# define turag_print_gametime_ms(gametime) TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__


#ifdef __DOXYGEN__

/// \brief Gibt die aktuelle Spielzeit aus.
///
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 0 wird die Spielzeit tatsächlich ausgegeben.
/// Die übermittelte Zeit entspricht der Systemzeit, sofern sie nicht durch Aufruf von
/// turag_print_system_gametime_reset() zurückgesetzt wurde.
void turag_print_system_gametime();

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 0
# define turag_print_system_gametime() \
	turag_print_gametime(_turag_ticks_to_time(turag_get_current_tick().value - turag_debug_print_gametime_gamestarttime.value))
#else
# define turag_print_system_gametime() TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__


#ifdef __DOXYGEN__

/// \brief Setzt die von turag_print_system_gametime() verwendete Spielzeit auf 0 zurück.
void turag_print_system_gametime_reset(void);

#else // __DOXYGEN__

# if TURAG_DEBUG_LEVEL > 0
#  define turag_print_system_gametime_reset() \
	turag_debug_print_gametime_gamestarttime = turag_get_current_tick()
# else
#  define turag_print_system_gametime_reset() TURAG_MACRO_NOOP
# endif

#endif // __DOXYGEN__


#ifdef __DOXYGEN__

/// \brief Aktiviert die automatische Ausagbe der System-Spielzeit mit jeder Debugausgabe.
///
/// Diese Funktion hat nur einen Effekt, wenn TURAG_PRINT_GAMETIME_AUTOMATIC auf 1 definiert wurde.
void turag_print_gametime_automatic_enable();

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 0 && TURAG_PRINT_GAMETIME_AUTOMATIC == 1
# define turag_print_gametime_automatic_enable() \
	turag_debug_print_gametime_auto_output_enabled = 1
#else
# define turag_print_gametime_automatic_enable() TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__


#ifdef __DOXYGEN__

/// \brief Deaktiviert die automatische Ausagbe der System-Spielzeit mit jeder Debugausgabe.
///
/// Diese Funktion hat nur einen Effekt, wenn TURAG_PRINT_GAMETIME_AUTOMATIC auf 1 definiert wurde.
void turag_print_gametime_automatic_disable();

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 0 && TURAG_PRINT_GAMETIME_AUTOMATIC == 1
# define turag_print_gametime_automatic_disable() \
	turag_debug_print_gametime_auto_output_enabled = 0
#else
# define turag_print_gametime_automatic_disable() TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

#ifdef __DOXYGEN__

/**
 * @brief Gibt die System-Spielzeit zurück.
 * @return Zeit seit Systemstart oder dem letzten Aufruf von
 * turag_print_system_gametime_reset().
 */
TuragSystemTime turag_debug_system_gametime_get(void);

#else // __DOXYGEN__

#if TURAG_DEBUG_LEVEL > 0
# define turag_debug_system_gametime_get() \
	_turag_ticks_to_time(turag_get_current_tick().value - turag_debug_print_gametime_gamestarttime.value)
#else
# define turag_debug_system_gametime_get() \
	_turag_ticks_to_time(turag_get_current_tick().value)
#endif

#endif // __DOXYGEN__


/// \}
/// \}

TURAG_C_API_END

#endif // DEBUG_TIME_H
