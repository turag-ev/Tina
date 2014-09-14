#ifndef TINA_H
#define TINA_H

/**
 * \mainpage Introduction
 * 
 * TinA ist eine Sammlung wiederverwenbare und modularisierter C- und C++-
 * Quellcodes, die vor allem zur Roboterentwicklung in der %TURAG
 * entstanden sind und gepflegt werden. 
 * 
 * Die vorliegende Dokumentation wird server-seitig automatisch aus den Quellen
 * generiert und spiegelt stets den neuesten Stand des master-Branches
 * des [programmierung-git-Repositories](https://www.turag.de/gitweb?fetch=gitweb%2F%3Fp%3Dprogrammierung.git%3Ba%3Dsummary) 
 * wider.
 * 
 * Diese Dokumentation ist in Modulen strukturiert, die Modulübersicht ist daher ein guter
 * Startpunkt. Jedes Modul enthält in eckigen Klammern Angaben über das Interface. Möglich sind:
 * - [C] - nur mit C-Interface
 * - [C++] - nur mit C++-Interface
 * - [C/C++] - Interface für C und C++ vorhanden. Das ist allerdings keine 
 * Garantie für identische Funktionalität beider Interfaces.
 * 
 * 
 * 
 * 
 * 
 * 
 * @namespace TURAG
 * @brief Standard namespace for TinA.
 * 
 */


#ifndef TURAG_STD_CONFIG
# include <tina/config.h>
#else
# include "helper/std_config.h"
#endif

#include "helper/normalize.h"
#include "helper/types.h"
#include "helper/macros.h"

#endif // TINA_H
