/**
 * \file tina/helper/config_tina_default.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \author Martin Oemus <martin@oemus.net>
 * \date 2011,2012, 2014
 *
 * \brief %TinA default configuration file.
 * 
 * Diese Datei enthält eine Standardkonfiguration für TinA.
 * 
 */



#ifndef TINA_CONFIG_TINA_DEFAULT_H
#define TINA_CONFIG_TINA_DEFAULT_H


/// ASCII-Zeichen für neue Zeile für Debugausgaben
#if !defined(TURAG_DEBUG_NEWLINE) || defined(__DOXYGEN__)
# define TURAG_DEBUG_NEWLINE "\n"
#endif


/// enables some features in feldbus classes that 
/// require dynamic memory
#if !defined(TURAG_FELDBUS_AKTOR_STRUCTURED_OUTPUT_AVAILABLE) || defined(__DOXYGEN__)
# define TURAG_FELDBUS_AKTOR_STRUCTURED_OUTPUT_AVAILABLE 0
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

#endif // TINA_CONFIG_TINA_DEFAULT_H
