#ifndef TINA_DEBUG_BINARY_H
#define TINA_DEBUG_BINARY_H

#ifndef TURAG_DEBUG_LOG_SOURCE
#define TURAG_DEBUG_LOG_SOURCE "_"
#endif

#include <tina/debugprint.h>
#include "defines.h"
#include "log-source.h"

#ifdef __cplusplus
extern "C" {
#endif


/// \addtogroup Debug
/// \{
/// \defgroup Debugbinary Binärobjekte
///
/// Über die folgende Funktionen können Binärobjekte verschickt werden.
/// Diese können für eine erweiterte Darstellung (z.B. Roboterposition auf Spielfeld,
/// aktuelle Zielposition, ...) dienen.
///
/// Zu beachten ist, das dieses Protokoll sehr allgemein gehalten ist. D.h.
/// es können Binärobjekte jeden Typs übertragen werden. Es kann nicht garantiert
/// werden, dass die gesendeten Objekte von der Endstelle auch interpretiert werden können.
///
/// Benutzung
/// ---------
///
/// Objekte werden über das Makro \ref turag_binary_send() versendet. Die Objektstruktur
/// sollte in einem globalen Header definiert sein. Es ist empfehlenswert, dass der
/// erste Eintrag in dieser Struktur, die verwendete Objektstrukturversion beinhaltet:
/// \code
/// //
/// // Header
/// struct DebugPose {
///   uint8_t version;
///   int16_t x;
///   int16_t y;
/// } _packed;
///
/// //
/// // Code
///
/// struct DebugPose debug_pose; // Objektinstanz erstellen
/// debug_pose.version = 1; // Objektstrukturversion setzen
///
/// // Objekt füllen
/// debug_pose.x = // ...
/// debug_pose.y = // ...
///
/// // Objekt senden
/// turag_binary_send("P", debug_pose);
/// \endcode
/// Wird die Objektstruktur geändert, muss die Objektstrkturversion
/// entsprechend hochgesetzt werden. Damit wird verhindert, dass die
/// Objektdaten falsch interpretiert werden könnten.
///
/// Da es dem Compiler freisteht zwischen den Einträgen in der Objektstruktur
/// Leerräume zu erstellen (um z.B. die Performance zu steigern), sollte der
/// Compiler angewiesen werden, auf diese Leerräume zu verzichten (\ref _packed).
/// Wird dies vergessen, kann es vorkommen, dass unter unterschiedlichen Compilern
/// die Struktur eine andere Größe hat und damit die Binärdaten von der Endstelle nicht
/// interpretiert werden können.
///
/// Implementierung
/// ---------------
///
/// Der Binärobjektstrom implementiert das Tina-Debug-Protokoll:
/// \code
/// [SOF][Log source][Log level][Payload][EOL]
///
/// SOF - Start von Paketrahmen: TURAG_DEBUG_LINE_PREFIX
/// Log source - Senderid: TURAG_DEBUG_LOG_SOURCE
/// Log level - "x"
/// Payload - Binärobjectinformationen
/// EOL - Ende von Paketrahmen: TURAG_DEBUG_NEWLINE
/// \endcode
///
/// Die Binärobjectinformationen enthält eine Objekt-ID ähnlich der Logquelle (z.B. TURAG_DEBUG_LOG_SOURCE)
/// damit können unterschiedliche Objekte von der gleiche Quelle unterschieden werden. So muss
/// jedes Objekt, was einen anderen Typ hat, auch einen andere Objekt-ID aufweisen. Die Objekt-ID darf
/// nicht Binär 2 ('\\x02') entsprechen.
/// \code
/// [Objekt ID][Objekt Bytes (base64)]
///
/// Objekt ID - Zeichen für Objekt aus Logquelle
/// Objekt Daten - Binärdaten von Objekt (base64-encodiert)
/// \endcode
///
/// \{

/// \brief Prefix für Binärobjekte
#define TURAG_DEBUG_BINARY_PREFIX    		"x"

#ifdef TURAG_DEBUG_ENABLE_BINARY || defined(__DOXYGEN__)

#ifndef __DOXYGEN__

void _turag_binary_send(char source, char object_id, const void* object, size_t size);

#endif // __DOXYGEN__

/// \brief Debugobjektdaten senden
///
/// \param object_id ein Zeichen als zuordnenbare Objekt-ID von Objekt aus Logquelle
/// \param object Objektinstanz
#define turag_binary_send(object_id, object) \
	_turag_binary_send( \
	  TURAG_DEBUG_LOG_SOURCE[0], \
	  object_id, \
	  &object, sizeof(object))

#else // TURAG_DEBUG_ENABLE_BINARY

#define turag_binary_send(object_id, object) while(0)

#endif // TURAG_DEBUG_ENABLE_BINARY

/// \}
/// \}
/// \}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_DEBUG_BINARY_H
