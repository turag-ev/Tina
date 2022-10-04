#ifndef TINA_DEBUG_GAME_TIME_H
#define TINA_DEBUG_GAME_TIME_H

#include "defines.h"
#include "print.h"
#include <tina/time.h>

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

#define turag_print_gametime(gametime) \
	turag_debug_printf_impl(TURAG_DEBUG_LINE_PREFIX "GT%x" TURAG_DEBUG_NEWLINE, turag_ticks_to_ms(gametime))

#endif // __DOXYGEN__


#ifdef __DOXYGEN__

/// \brief Gibt die aktuelle Spielzeit aus.
///
/// Nur bei der Definition von \ref TURAG_DEBUG_LEVEL größer 0 wird die Spielzeit tatsächlich ausgegeben.
/// \param gametime Aktuelle Spielzeit in Millisekunden.
void turag_print_gametime_ms(unsigned gametime);

#else // __DOXYGEN__

#define turag_print_gametime_ms(gametime) \
	turag_debug_printf_impl(TURAG_DEBUG_LINE_PREFIX "GT%x" TURAG_DEBUG_NEWLINE, gametime)

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
# define turag_print_system_gametime turag_print_system_gametime
#else
# define turag_print_system_gametime() TURAG_MACRO_NOOP
#endif

#endif // __DOXYGEN__

/// \brief Setzt die von turag_print_system_gametime() verwendete Spielzeit auf 0 zurück.
void turag_print_system_gametime_reset(void);


/// \brief Aktiviert die automatische Ausagbe der System-Spielzeit mit jeder Debugausgabe.
///
/// Diese Funktion hat nur einen Effekt, wenn TURAG_PRINT_GAMETIME_AUTOMATIC auf 1 definiert wurde.
void turag_print_gametime_automatic_enable(void);


/// \brief Deaktiviert die automatische Ausagbe der System-Spielzeit mit jeder Debugausgabe.
///
/// Diese Funktion hat nur einen Effekt, wenn TURAG_PRINT_GAMETIME_AUTOMATIC auf 1 definiert wurde.
void turag_print_gametime_automatic_disable(void);

/**
 * @brief Gibt die System-Spielzeit zurück.
 * @return Zeit seit Systemstart oder dem letzten Aufruf von
 * turag_print_system_gametime_reset().
 */
TuragSystemTime turag_debug_system_gametime_get(void);


/// \}
/// \}

TURAG_C_API_END

#endif // TINA_DEBUG_GAME_TIME_H
