/**
 * \file tina/helper/config_tina_default.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \author Martin Oemus <martin@oemus.net>
 * \date 2011, 2012, 2014, 2015
 *
 * \brief %TinA default configuration file.
 * 
 * Diese Datei enthält eine Standardkonfiguration für TinA.
 * 
 * \see \ref Konfigurierbarkeit
 */


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!  NEVER NEVER EVER CHANGE ANY OF THESE DEFAULT VALUES  !!!
 * !!!   UNLESS YOU REALLY REALLY KNOW WHAT YOU ARE DOING!   !!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#ifndef TINA_CONFIG_TINA_DEFAULT_H
#define TINA_CONFIG_TINA_DEFAULT_H

/** @name Allgemein
 *  
 * @{
 */

/// ASCII-Zeichen für neue Zeile für Debugausgaben
#if !defined(TURAG_DEBUG_NEWLINE) || defined(__DOXYGEN__)
# define TURAG_DEBUG_NEWLINE "\n"
#endif

/// enable this if you link with default c++ runtime support.
#if !defined(TURAG_USE_LIBSUPCPP_RUNTIME_SUPPORT) || defined(__DOXYGEN__)
# define TURAG_USE_LIBSUPCPP_RUNTIME_SUPPORT 0
#endif

/// enable this if you link without libstc++/libsupc++
/// but require C++-runtime support.
#if !defined(TURAG_USE_TINA_CPP_RUNTIME_SUPPORT) || defined(__DOXYGEN__)
# define TURAG_USE_TINA_CPP_RUNTIME_SUPPORT		1
#endif

/// Aktiviert TinA HighResDelay-Timer.
/// \see \ref TURAG::HighResDelayTimer
#if !defined(TURAG_USE_TINA_HIGH_RES_DELAY_TIMER) || defined(__DOXYGEN__)
# define TURAG_USE_TINA_HIGH_RES_DELAY_TIMER		0
#endif


/**
 * @}
 */

/** @name CRC-Config
 *  
 * @{
 */
/// Stellt ein, welcher Algorithmus zur CRC8-Berechnung verwendet werden
/// soll. 
///
/// Mögliche Werte:
/// - 0: Feature deaktivieren
/// - 1: tabellenbasiert
/// - 2: bit-by-bit
/// - 3: Kompromiss aus 1 und 2
#if !defined(TURAG_CRC_CRC8_ALGORITHM) || defined(__DOXYGEN__)
# define TURAG_CRC_CRC8_ALGORITHM 0
#endif

/// Stellt ein, welcher Algorithmus zur CRC16-Berechnung verwendet werden
/// soll. 
///
/// Mögliche Werte:
/// - 0: Feature deaktivieren
/// - 1: tabellenbasiert
/// - 2: byte-wise direkte Berechnung
#if !defined(TURAG_CRC_CRC16_ALGORITHM) || defined(__DOXYGEN__)
# define TURAG_CRC_CRC16_ALGORITHM 		0
#endif

/// Wenn auf ungleich 0 definiert, so wird für die Berechnungsfunktionen
/// inlinebarer Code im Header ausgegeben, ansonsten einfache Funktionen.
#if !defined(TURAG_CRC_INLINED_CALCULATION) || defined(__DOXYGEN__)
# define TURAG_CRC_INLINED_CALCULATION 		0
#endif
/**
 * @}
 */


/** @name Feldbus Hostklassen-Konfiguration
 * @{
 */

/// Legt fest, ob Host-Support für den TURAG-Feldbus verfügbar ist.
/// Wenn auf 0 definiert, werden die zum Feldbus gehörenden Quelldateien
/// nicht compiliert. Daran sollten sich auch alle neuen Klassen halten.
/// \see \ref feldbus-host
#if !defined(TURAG_USE_TURAG_FELDBUS_HOST) || defined(__DOXYGEN__)
# define TURAG_USE_TURAG_FELDBUS_HOST		0
#endif

/// enables some features in feldbus classes that 
/// require dynamic memory
/// \see \ref feldbus-host
#if !defined(TURAG_FELDBUS_AKTOR_STRUCTURED_OUTPUT_AVAILABLE) || defined(__DOXYGEN__)
# define TURAG_FELDBUS_AKTOR_STRUCTURED_OUTPUT_AVAILABLE 0
#endif

/**
 * @}
 */



/*
 * plausability checks
 */
#if TURAG_CRC_CRC8_ALGORITHM < 0 || TURAG_CRC_CRC8_ALGORITHM > 3
# error TURAG_CRC_CRC8_ALGORITHM must be between 0 and 3
#endif
#if TURAG_CRC_CRC16_ALGORITHM < 0 || TURAG_CRC_CRC16_ALGORITHM > 2
# error TURAG_CRC_CRC16_ALGORITHM must be between 0 and 2
#endif

#endif // TINA_CONFIG_TINA_DEFAULT_H
