#ifndef TINA_TINA_H
#define TINA_TINA_H

/**
 * \mainpage ReadMe
 * 
 * \section Allgemein
 * TinA ist eine Sammlung wiederverwendbarer und modularisierter C- und C++-
 * Quellcodes, die vor allem zur Roboterentwicklung in der %TURAG
 * entstanden sind und gepflegt werden. 
 * 
 * Die vorliegende Dokumentation wird server-seitig automatisch aus den Quellen
 * generiert und spiegelt stets den neuesten Stand des master-Branches
 * des [programmierung-git-Repositories](https://intern.turag.de/joomla/gitweb?fetch=gitweb%2F%3Fp%3Dprogrammierung.git%3Ba%3Dsummary) 
 * wider.
 * 
 * Diese Dokumentation ist in Modulen strukturiert, die Modulübersicht ist daher ein guter
 * Startpunkt. Jedes Modul enthält in eckigen Klammern Angaben über das Interface. Möglich sind:
 * - [C] - nur mit C-Interface
 * - [C++] - nur mit C++-Interface
 * - [C/C++] - Interface für C und C++ vorhanden. Das ist allerdings keine 
 * Garantie für identische Funktionalität beider Interfaces.
 *
 * \note Das C-Interface soll mittelfristig entfernt werden. Neue Features sollten
 * daher ausschließlich in C++ implementiert werden.
 * 
 * Die primäre Dokumentationssprache für die Doxygen-formatierten Kommentare ist deutsch. 
 * Quelltext und sonstige Kommentare, die die Implementierung erklären, sollten
 * englisch sein.
 * 
 * Weiterführende Informationen zu doxygen finden sich auf der enstprechenden
 * [Wiki-Seite](https://intern.turag.de/wiki/doku.php?id=04_software;doxygen).
 * 
 * \section Einschränkungen
 * Bei der Entwicklung und Benutzung von TinA gelten einige Vereinbarungen:
 * - Der verwendete Compiler ist der gcc in einer aktuellen Version (>= 4.7)
 * - je nach Plattform ist die C++-Laufzeitunterstützung eingeschränkt --> der Grad an 
 * erforderlicher Laufzeitunterstützung ist noch zu definieren!
 * - es ist empfohlen mit -Wundef zu compilieren!
 * 
 * \section Konfigurierbarkeit
 * TinA verfügt über ein 3-stufiges Konfigurationssystem.
 * Der Reihe nach werden diese Header eingebunden:
 * 1. <config_tina.h>
 * 2. <tina/config_tina_platform.h>
 * 3. <tina/helper/config_tina_default.h>
 *
 * - Der erste Header soll benutzt werden, um im Projekt bestimmte Features zu
 * aktivieren oder deaktivieren.
 * - Der zweite Header ersetzt den bisherigen Header <tina/config.h> und ist nach wie vor
 * gedacht, um TinA der Platform/OS gemäß anzupassen.
 * - Der dritte Header enthält sinnvolle Standardwerte und listet alle verfügbaren
 * Optionen auf. Hat man sich einmal für einen Standardwert entschieden, so sollte
 * dieser keinesfalls mehr geändert werden! Außerdem kann in diesem Header, wenn notwendig,
 * ein rudimentäres Konfliktmanagement implementiert werden.
 *
 * Die Änderungen sollen mehr Kontrolle darüber geben, welche Features von TinA ins
 * Programm gelinkt werden und welche nicht, um, falls nötig, Flash und RAM zu sparen.
 * Damit das 3-stufige System funktioniert, müssen die Header 2 und 3 bei jeder Option,
 * die sie konfigurieren, prüfen, ob diese nicht bereits von einem der übergeordneten
 * Header gesetzt wurden.
 *
 * @namespace TURAG
 * @brief Standard namespace for TinA.
 * 
 */

// include custom project configuration unless
// this is deactivated
#ifndef TURAG_NO_PROJECT_CONFIG
# include <config_tina.h>
#endif


// include platform configuration unless
// this is deactivated
#ifndef TURAG_NO_PLATFORM_CONFIG
# include <tina/config_tina_platform.h>
#endif

// always include default configuration
#include "helper/config_tina_default.h"


#include "helper/normalize.h"
#include "helper/types.h"
#include "helper/macros.h"

#endif // TINA_TINA_H
