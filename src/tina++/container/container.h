#ifndef TINAPP_CONTAINER_CONTAINER_H
#define TINAPP_CONTAINER_CONTAINER_H

/**
 * \defgroup Container Container [C++]
 *
 * \brief Stellt Datenstrukturen für C++ bereit, die kein Heap (new) benötigen
 *
 * Alle Datenstrukturen besitzen eine vorher definierte Kapazität, damit sie
 * nur Stackspeicher benutzen. Sie können ihre Kapazität nicht während der
 * Laufzeit ändern, wie Container aus der STL (<a href="http://de.wikipedia.org/wiki/C%2B%2B-Standardbibliothek#Container">Standard-Template-Libary</a>) wie std::vector, ...
 *
 * Die Kapazität eines Containers muss beim Compilieren bekannt sein.
 */

#include "array_buffer.h"
#include "circular_buffer.h"
#include "thread_fifo.h"

#endif // TINAPP_CONTAINER_CONTAINER_H
