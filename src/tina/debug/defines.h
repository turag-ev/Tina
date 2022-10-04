/**
 * \file tina/debug.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2012
 *
 * \brief Utils for printing debug information
 *
 */

#ifndef TINA_DEBUG_DEFINES_H
#define TINA_DEBUG_DEFINES_H

#include <tina/tina.h>

/// \defgroup Debug Debugging [C/C++]
/// \brief Debugging [C/C++]
///
/// Textausgaben
/// ------------
///
/// Zum Abschicken von Debugausgaben stehen Funktionen zur Verfügung, die sich ähnlich wie ''printf'' verhalten:
/// \code{.cpp}
/// // freigeschaltet bei TURAG_DEBUG_LEVEL >= 5
/// turag_debugf("formatierte Ausgabe %s", "von internen Debuginformationen");
///
/// // freigeschaltet bei TURAG_DEBUG_LEVEL >= 4
/// turag_infof("formatierte Ausgabe %s", "von einfachen Informationen");
///
/// // freigeschaltet bei TURAG_DEBUG_LEVEL >= 3
/// turag_warningf("formatierte Ausgabe %s", "von Warnungen");
///
/// // freigeschaltet bei TURAG_DEBUG_LEVEL >= 2
/// turag_criticalf("formatierte Ausgabe %s", "von kritischen Fehlern");
///
/// // freigeschaltet bei TURAG_DEBUG_LEVEL >= 1
/// turag_errorf("formatierte Ausgabe %s", "von fatalen Fehlern");
/// \endcode
/// Ein Zeilenumbruch am Ende der Zeile ist nicht zulässig!. Die TURAG-Console
/// kommt mit Zeilenumbrüchen in der Mitte von einer Ausgabe nicht klar,
/// deswegen dann eher zwei Funktionsaufrufe tätigen.
///
/// Zu jeder dieser Funktion existiert auch eine Funktion ohne ''f'' am Ende,
/// damit können nur Zeichenketten ausgegeben werden (ohne Formatierung).
/// Sie sind dadurch aber schneller.
///
/// Codierung
/// ---------
///
/// Es wird das <a href="https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;tina-debugprotokoll">
/// Tina-Debugprotokoll</a> verwendet.
/// Die Ausgabe in der seriellen Konsole sieht dann so aus:
/// \code
///   -Tformatierte Ausgabe von einfachen Informationen
/// \endcode
/// Das erste Zeichen ist ein Startzeichen mit dem Wert 2 (wird in der Konsole meist nicht angezeigt).
/// Das nächste Zeichen danach steht für das Debuglevel der Ausgabe (hier: ''-'' entspricht
/// einfachen Informationen und ist definiert in ''INFO_PREFIX'').
/// Für die verschiedenen Debuglevel (''info'' bis ''error'') gibt es ein
/// entsprechendes Prefix, was in dem Makro ''*_PREFIX'' definiert ist.
///
/// Das zweite Zeichen stellt die Herkunft der Ausgabe dar. Im Beispiel ist es
/// ''T'' was für Testausgaben stehen soll. Die Herkunft muss im Makro
/// ''TURAG_DEBUG_LOG_SOURCE'' definiert werden bevor ''utils/debug.h'' eingebunden wird.
/// Es sollte somit definiert werden bevor überhaupt etwas eingebunden wird.
/// Die Definition sollte auch nur ein Quelldateien geschehen, weil sie in
/// Headerdateien sinnlos ist. Es sind nur ASCII-Zeichen erlaubt. Ist das Makro
/// beim einbinden nicht definiert wird ''_'' gewählt.
/// \code{.cpp}
/// // Quelldatei beispiel.cpp:
/// #define TURAG_DEBUG_LOG_SOURCE "T"
/// #include <tina/debug.h>
///
/// // weitere includes
///
/// // Quelltext
/// \endcode
///
/// Um zum Beispiel für die Simulation einen Zusammenhang zwischen
/// Debugquellenzeichen und Herkunftsname herzustellen, sollte ein Array für
/// alle möglichen ASCII-Zeichen angelegt werden. Im EB13-Code wurde das in der
/// ''systemcontrol.cpp'' so gelöst:
/// \code{.cpp}
/// const char* log_source_table[127] = {0};
///
/// TURAG_INITIALIZATION {
///   log_source_table['*'] = "AStar";
///   // ...
///
///   log_source_table['_'] = "k.A."; // Zeichen bei keiner Angabe der Herkunft
/// }
///
/// // Zugriff aus Simulations- und Testmenücode:
/// extern const char* log_source_table[127];
/// log_source_table[zeichen]
///  \endcode
///
/// Abschalten von Debugmeldungen beim Kompilieren
/// ----------------------------------------------
///
/// Zum Abschalten über alle Quelldateien muss im Makefile nur ein gewünschter
/// Wert für ''TURAG_DEBUG_LEVEL'' angegeben werden.
/// \code{Makefile}
/// # um nur fatale Fehler anzuzeigen:
/// DDEF += -DTURAG_DEBUG_LEVEL=1
/// \endcode
///
/// Will man nur für eine Quelldatei die Debugmeldungen abschalten, so kann, wie
/// bei ''TURAG_DEBUG_LOG_SOURCE'', vor dem Einbinden der gewünschte Wert gesetzt werden:
/// \code{.cpp}
/// // Quelldatei beispiel.cpp:
/// #define TURAG_DEBUG_LOG_SOURCE "T"
/// #define TURAG_DEBUG_LEVEL 3 // Nur warning, critical und error anzeigen
/// #include <tina/debug.h>
///
/// // weitere includes
///
/// // Quelltext
/// \endcode
///
/// Ausgabe der Spielzeit mit jeder Debug-Ausgabe
/// ----------------------------------------------
/// Zur Ausgabe der Spielzeit mit jeder Text-Ausgabe muss dieses Symbol
/// definiert sein:
///
/// \code{Makefile}
/// # um die Spielzeit auszugeben:
/// DDEF += -DTURAG_ALWAYS_PRINT_GAMETIME=1
/// \endcode
///
///
/// \{

#ifndef TURAG_DEBUG_LEVEL
/// \brief Debuglevel
///
/// Kann über Makefile oder in Zeilen vor erstem Include umdefiniert werden.
///
/// \note Dieser Wert hat einen direkten Einfluss auf das Verhalten aller Debug-Module.
#define TURAG_DEBUG_LEVEL 4
#endif

#if !defined(TURAG_DEBUG_ENABLE_BINARY)
# if TURAG_DEBUG_LEVEL > 3
/// \brief Wenn definiert werden Debugbinärobjekte gesendet.
///
/// Kann über Makefile oder in Zeilen vor erstem Include definiert werden.
/// Wird standardmäßig nicht definiert.
/// \todo Sollte standardmäßig aktiviert werden.
#  define TURAG_DEBUG_ENABLE_BINARY 0
# else
#  define TURAG_DEBUG_ENABLE_BINARY 0
# endif
#endif

#if !defined(NDEBUG) && TURAG_DEBUG_LEVEL < 1
# define NDEBUG
#endif

// automatic game time printing
#ifndef TURAG_PRINT_GAMETIME_AUTOMATIC
/// \brief Automatische Spielzeitausgabe
///
/// Sollte über Makefile oder in Zeilen vor erstem Include umdefiniert werden.
/// Standardmäßig deaktiviert.
/// Die Makrodefinition schaltet das Feature lediglich frei - die tatsächliche
/// Ausgabe muss explizit durch einen Aufruf von turag_print_gametime_automatic_enable()
/// aktiviert werden.
//# define TURAG_PRINT_GAMETIME_AUTOMATIC 0
#endif



/// Startzeichen für Debugmeldungen
#define TURAG_DEBUG_LINE_PREFIX "\x02"

/// Speziallevel: der Text wird ab sofort als Label der entsprechenden Logquelle benutzt.
#define TURAG_DEBUG_REPORT_LOG_SOURCE_PREFIX  ">"

/// Prefix für Fehlermeldung
#define TURAG_DEBUG_ERROR_PREFIX    "!"

/// Prefix für kritische Debugmeldungen
#define TURAG_DEBUG_CRITICAL_PREFIX "?"

/// Prefix für Warnungen
#define TURAG_DEBUG_WARN_PREFIX     "#"

/// Prefix für informative Meldungen
#define TURAG_DEBUG_INFO_PREFIX     "-"

/// Prefix für Debugmeldungen
#define TURAG_DEBUG_DEBUG_PREFIX    "_"

/// Prefix für Spielzeitausgaben
#define TURAG_DEBUG_GAMETIME_PREFIX "T"


#ifndef __DOXYGEN__

// graph output prefixes
#define TURAG_DEBUG_GRAPH_PREFIX    		"D"
#define TURAG_DEBUG_GRAPH_CREATE    		"n"
#define TURAG_DEBUG_GRAPH_COPY				"r"
#define TURAG_DEBUG_GRAPH_CHANNEL   		"b"
#define TURAG_DEBUG_GRAPH_CHANNEL_FIXED		"B"
#define TURAG_DEBUG_GRAPH_DATA      		"d"
#define TURAG_DEBUG_GRAPH_DATA2D      		"D"
#define TURAG_DEBUG_GRAPH_VERTICAL_MARKER  	"m"
#define TURAG_DEBUG_GRAPH_CHANNELGROUP  	"g"

#endif // __DOXYGEN__

#ifndef TURAG_DEBUG_GRAPH_MAX_CHANNELS
/// Maximale Anzahl an Graph-Kanälen
#define TURAG_DEBUG_GRAPH_MAX_CHANNELS  (64)
#endif

# if TURAG_WORDSIZE == 64
#  define TURAG_32_PREFIX
#  define TURAG_64_PREFIX	"l"
#  define TURAG_PTR_PREFIX	"l"
# else
#  define TURAG_32_PREFIX
#  define TURAG_64_PREFIX	"ll"
#  define TURAG_PTR_PREFIX
# endif

# define TURAG_dPTR	TURAG_PTR_PREFIX "d"
# define TURAG_iPTR	TURAG_PTR_PREFIX "i"
# define TURAG_oPTR	TURAG_PTR_PREFIX "o"
# define TURAG_uPTR	TURAG_PTR_PREFIX "u"
# define TURAG_xPTR	TURAG_PTR_PREFIX "x"
# define TURAG_XPTR	TURAG_PTR_PREFIX "X"

# define TURAG_d32	TURAG_32_PREFIX "d"
# define TURAG_i32	TURAG_32_PREFIX "i"
# define TURAG_o32	TURAG_32_PREFIX "o"
# define TURAG_u32	TURAG_32_PREFIX "u"
# define TURAG_x32	TURAG_32_PREFIX "x"
# define TURAG_X32	TURAG_32_PREFIX "X"

/// \}

#endif // TINA_DEBUG_DEFINES_H

