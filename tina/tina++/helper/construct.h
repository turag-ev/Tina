#ifndef TINAPP_HELPER_CONSTRUCT_H
#define TINAPP_HELPER_CONSTRUCT_H

#include <type_traits>
#include <iterator>

#include "normalize.h"
#include "macros.h"

namespace TURAG {

/// \addtogroup Utils
/// \{

#ifndef DOXYGEN

template<typename T>
#if GCC_VERSION < 40800 && !defined(__clang__)
struct is_trivially_destructible : public std::has_trivial_destructor<T> { };
#else
struct is_trivially_destructible : public std::is_trivially_destructible<T> { };
#endif

#endif // DOXYGEN


/// Erstelle Instanz von Klasse in vorhandenen Speicher mit zugehörigen Argumenten
/// \param ptr Zeiger auf nicht initalisierten Speicher an dem Instanz erstellt wird
/// \param args Argumente für Konstuktor
/// \warning Nur verwenden, wenn C++ Instanz nicht selber verwaltet, weil
/// zum Beispiel ein Bytepuffer verwendet wird.
template<typename T, typename ...Args> _always_inline
void construct(T* ptr, Args&&... args) {
  ::new(ptr) T(std::forward<Args>(args)...);
}

#ifndef DOXYGEN

template<typename T, REQUIRES(is_trivially_destructible<T>)> _always_inline
void destruct(T*) { }

template<typename T, REQUIRES(!is_trivially_destructible<T>)> _always_inline
void destruct(T* ptr) {
  ptr->~T();
}

template<typename ForwardIterator, REQUIRES(is_trivially_destructible<typename std::iterator_traits<ForwardIterator>::value_type>)> _always_inline
void destruct(ForwardIterator, ForwardIterator) { }

template<typename ForwardIterator, REQUIRES(!is_trivially_destructible<typename std::iterator_traits<ForwardIterator>::value_type>)>
void destruct(ForwardIterator first, ForwardIterator last) {
  for (; first != last; ++first) {
    destruct(std::addressof(*first));
  }
}

#else // DOXYGEN

/// \brief Zerstöre Instanz von Typ
/// \param instance Zeiger auf Instanz von Typ
/// \warning Nur verwenden, wenn C++ Instanz nicht selber verwaltet, weil
/// zum Beispiel ein Bytepuffer verwendet wird.
template<typename T>
void destruct(T* instance);

/// \brief Zerstöre mehrere Instanzen von Typ
/// \param begin Interator für erstes zu zerstörende Instanz
/// \param end Interator für erstes nicht mehr zu zerstörende Instanz
/// \warning Nur verwenden, wenn C++ Instanzen nicht selber verwaltet, weil
/// zum Beispiel ein Bytepuffer verwendet wird.
template<typename ForwardIterator>
void destruct(ForwardIterator begin, ForwardIterator end);

#endif // DOXYGEN

/// \}

} // namespace TURAG

#endif // TINAPP_HELPER_CONSTRUCT_H
