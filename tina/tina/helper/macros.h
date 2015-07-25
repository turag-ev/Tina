#ifndef TINA_HELPER_MACROS_H
#define TINA_HELPER_MACROS_H

#include "normalize.h"

////////////////////////////////////////////////////////////////////////////////
// Preprocessor utils

/// \defgroup Macros Makrohelfer [C/C++]
/// \{

#ifndef __DOXYGEN__

// Interna

#define __NARG__(args...)  __NARG_I_(,##args,__RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N( \
      _0, _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define __RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#define _TURAG_VFUNC_(name, n) name##n
#define _TURAG_VFUNC(name, n) _TURAG_VFUNC_(name, n)

#define _TURAG_CONCAT1_(a) a
#define _TURAG_CONCAT1(a) _TURAG_CONCAT1_(a)

#define _TURAG_CONCAT2_(a,b) a##b
#define _TURAG_CONCAT2(a,b) _TURAG_CONCAT2_(a,b)

#define _TURAG_CONCAT3_(a,b,c) a##b##c
#define _TURAG_CONCAT3(a,b,c) _TURAG_CONCAT3_(a,b,c)

#define _TURAG_CONCAT4_(a,b,c,d) a##b##c##d
#define _TURAG_CONCAT4(a,b,c,d) _TURAG_CONCAT4_(a,b,c,d)

#define _TURAG_CONCAT5_(a,b,c,d,e) a##b##c##d##e
#define _TURAG_CONCAT5(a,b,c,d,e) _TURAG_CONCAT5_(a,b,c,d,e)

#define _TURAG_CONCAT6_(a,b,c,d,e,f) a##b##c##d##e##f
#define _TURAG_CONCAT6(a,b,c,d,e,f) _TURAG_CONCAT6_(a,b,c,d,e,f)

#define _TURAG_CONCAT_FUNC(n) _TURAG_CONCAT_FUNC_(n)
#define _TURAG_CONCAT_FUNC_(n) _TURAG_CONCAT ## n

#define _TURAG_STRINGIFY(x) #x
         
#else // __DOXYGEN__

///  \brief Nummer der Argumente zurückgeben
///
/// Von hier: https://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5?pli=1
///
/// \code
/// int x = __NARGS__(a,b,c,d,e,f); // x = 6
/// \endcode
///
/// Ist nützlich bei Makrofunktionen, da es dort nur eine Funktion mit dem
/// gleichen Namen geben darf. Will man aber mit einer unterschliedlichen Anzahl an Parametern
/// arbeiten, kann dieses Makro benutzt werden:
/// \code
/// #define SUM1(x) (x)
/// #define SUM2(x,y) ((x) + (y))
/// #define SUM3(x,y,z) ((x) + (y) + (z))
/// #define SUM(...) TURAG_CONCAT(SUM, __NARG__(__VA_ARGS__)) (__VA_ARGS__)
///
/// int sum = SUM(x, 5, 42); // benutzt SUM3
/// \endcode
///
/// \pre Maximal 64 Argumente können angegeben werden.
/// \bug Aufruf ohne Argumente compiliert nicht
#define __NARG__(...)

#endif // __DOXYGEN__

/// \brief Funktion abhängig von Anzahl von Argumenten aufrufen
///
/// Der `name` Parameter gibt den Grundnamen an, an diesen wird die Anzahl
/// der gegebenen Parameter angehängt. Der zusammengesetzte Bezeichner wird
/// als Funktion aufgerufen. Falls die Funktion eine Makrofunktion ist wird diese
/// aufgerufen.
///
/// Diese Funktionalität wird in C-Funktionen und in Makrofunktionen benötigt,
/// da hier nicht zwei Funktionen mit dem gleichen Namen aber unterschiedlicher
/// Anzahl an Parametern möglich ist.
///
/// Zuerst wurden die einzelnen Funktionen definiert mit den unterschiedlichen
/// Parametern und dann eine Makrofunktion definiert, welche selber TURAG_VFUNC
/// aufruft um den richtigen Namen der Funktion anhand der Parameteranzahl zu
/// bestimmen und auszuführen:
/// \code
/// #define SUM1(x) (x)
/// #define SUM2(x,y) ((x) + (y))
/// #define SUM3(x,y,z) ((x) + (y) + (z))
/// #define SUM(...) TURAG_VFUNC(SUM, __VA_ARGS__)
///
/// int S = SUM(x, 5, 42); // benutzt SUM3
///
/// int sum0() { return 0; }
/// int sum1(int x) { return x; }
/// int sum2(int x, int y) { return x + y; }
/// int sum3(int x, int y, int z) { return x + y + z; }
/// #define sum(...) TURAG_VFUNC(sum, __VA_ARGS__)
///
/// int s = sum(x, 5, 42); // benutzt sum3
/// \endcode
///
/// \pre Maximal 64 Argumente können angegeben werden.
/// \bug Aufruf ohne Argumente (nur mit Funktionsgrundname) compiliert nicht
#define TURAG_VFUNC(name, ...) _TURAG_VFUNC(name, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

/// \brief die zwei Argumente zusammenführen
///
/// `TURAG_CONCAT(A, B, C)` wird zu `ABC`
/// \code
/// #define GET_PIN_ON_PORT(port, bit) (TURAG_CONCAT(PORT, port)&(TURAG_CONCAT(P, port, bit)))
/// if (GET_PIN_ON_PORT(B, 1)) { // wird zu: PORTB & PB1
///   // ...
/// }
/// \endcode
///
/// \pre Maximal 6 Argumente können angegeben werden.
#define TURAG_CONCAT(...) TURAG_VFUNC(_TURAG_CONCAT, __VA_ARGS__)

/// \brief die zwei Argumente zusammenführen
///
/// `TURAG_CONCAT2(A, BC)` wird zu `ABC`
///
/// \deprecated TURAG_CONCAT benutzen
#define TURAG_CONCAT2(a,b) _TURAG_CONCAT2_(a,b)

/// \brief die drei Argumente zusammenführen
///
/// `TURAG_CONCAT3(A, B, C)` wird zu `ABC`
///
/// \deprecated TURAG_CONCAT benutzen
#define TURAG_CONCAT3(a,b,c) _TURAG_CONCAT3_(a,b,c)

/// \brief die vier Argumente zusammenführen
///
/// `TURAG_CONCAT4(A, B, C, D)` wird zu `ABCD`
///
/// \deprecated TURAG_CONCAT benutzen
#define TURAG_CONCAT4(a,b,c,d) _TURAG_CONCAT4_(a,b,c,d)

/// \brief Argument in Zeichenkette umwandeln
///
/// `TURAG_STRINGIFY(123)` wird zu `"123"`
#define TURAG_STRINGIFY(x) _TURAG_STRINGIFY(x)

////////////////////////////////////////////////////////////////////////////////
// Macro utils

/// \brief leere Anweisung in Makro
///
/// Beispiel:
/// \code
/// #if OPTION == 42
/// # define my_function() printf("%d", 42)
/// #else
/// # define my_function() TURAG_MACRO_NOOP
/// #endif
/// \endcode
#define TURAG_MACRO_NOOP while (0)

/// \brief Anfang von Makro mit mehreren Anweisungen
///
/// Beispiel:
/// \code
/// #define PRINT_INT_ARRAY(c) TURAG_MACRO_BEGIN \​
///                      for (size_t i = 0; i < sizeof(c)/sizeof(c[0]); i++) { \​
///                         printf("%d ", c[i]); \​
///                      } TURAG_MACRO_END
/// \endcode
/// Wird dieses Marko bei Makros mit mehreren Anweisungen nicht verwenden, kann
/// es z.B. bei `if`-Anweisungen ohne Block, zu nicht gewollten Verhalten kommen.
/// \sa TURAG_MACRO_END
#define TURAG_MACRO_BEGIN do {

/// \brief Ende von Makro mit mehreren Anweisungen
/// \sa TURAG_MACRO_BEGIN
#define TURAG_MACRO_END } while (0)

////////////////////////////////////////////////////////////////////////////////
// Compiler utils

/// use it to shut the compiler up talking about unused variables
#ifndef UNUSED
#  define UNUSED(x) (void)x
#endif

////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION and FINALIZATION

/// \brief Definiert Funktion, welche bei Initialisierung des Programms aufgerufen wird.
///
/// Die Funktion wird noch vor `main` aufgerufen zu dem Zeitpunkt, wo auch
/// globale C++-Klassen erstellt werden. Die Reihenfolge in welcher die Funktionen
/// aufgerufen werden ist undefiniert. Deswegen darf auf keine globale Klasseninstanz
/// zu gegriffen werden, außer die Klasse ist trival konstruierbar (z.B. POD's).
///
/// Der Sinn dieser Funktion ist es globale Variablen zu initialisieren:
/// \code
/// static int global_vector[32];
/// static int global_int;
///
/// INITIALIZATION {
///   // Quadratzahlen speichern
///   for (size_t i = 0; i < 32; i++) {
///     global_vector[i] = i*i;
///   }
///
///   // Summe aller Quatratzahlen bis 31 speichern
///   global_int = global_vector[0];
///   for (size_t i = 1; i < 31; i++) {
///     global_int += global_vector[i];
///   }
/// }
/// \endcode
///
/// \todo In `TURAG_INITIALIZATION` umbenennen
#define INITIALIZATION \
  static _constructor void TURAG_CONCAT(_turag_initialization_code_, __COUNTER__)()


/// \brief Definiert Funktion, welche bei Terminierung des Programms aufgerufen wird.
///
/// Die Funktion wird nach `main` aufgerufen zu dem Zeitpunkt, wo auch
/// globale C++-Klassen zerstört werden. Die Reihenfolge in welcher die Funktionen
/// aufgerufen werden ist undefiniert. Deswegen darf auf keine globale Klasseninstanz
/// zu gegriffen werden, außer die Klasse ist trival zerstörbar (z.B. POD's).
///
/// Der Sinn dieser Funktion ist es abschließende Aktionen in einem <em>Desktop-Programm</em>
/// zu machen. Im Roboter wird dieser Code nie aufgerufen.
/// \code
/// FINALIZATION {
///   // abschließende Aktion durchführen
///   saveSettings();
/// }
/// \endcode
///
/// \todo In `TURAG_FINALIZATION` umbenennen
#define FINALIZATION \
  static _destructor void TURAG_CONCAT(_finalization_code, __COUNTER__)()
  
/// \}

#endif // TINA_HELPER_MACROS_H
