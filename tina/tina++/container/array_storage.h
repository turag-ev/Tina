#ifndef ARRAY_STORAGE_H
#define ARRAY_STORAGE_H

#include <type_traits>
#include <iterator>

#include "../tina.h"
#include "../helper/construct.h"

namespace TURAG {

/// \cond INCLUDE_HELPER

/// \brief Stellt Speicher für Klasse bereit, der entsprechend des Types ausgerichtet ist.
/// \ingroup Helferklassen
/// Wird verwendet in \a ArrayBuffer und \a CircularBuffer
template<typename T>
struct storage {
  typedef typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type type;
};

/// \brief Klasse die Speicher für Arrays bereitstellt
/// \ingroup Helferklassen Container
/// Es wird kein Konstruktor beim Erstellen von \a array_storage aufgerufen
///
/// Es wird auch keine Information gespeichert welche Elemente initalisiert sind.
/// Dafür ist der Benutzer der Klasse verantwortlich.
///
/// Wird verwendet in \a ArrayBuffer und \a CircularBuffer
template<typename T, size_t N>
struct array_storage {
public:
  COPYABLE(array_storage);
  MOVABLE(array_storage);

  // types
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  // iterator
  typedef pointer iterator;
  typedef const_pointer const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  typedef typename storage<T>::type element_type;

  constexpr _always_inline
  array_storage() :
    data_()
#ifdef TURAG_HEAVY_DEBUG
    , ref(*reinterpret_cast<T(*)[N]>(data_))
#endif
  {}

  // Create an object in aligned storage
  template<typename ...Args> _always_inline
  void emplace(std::size_t index, Args&&... args) {
    construct(reinterpret_cast<T*>(data_ + index), std::forward<Args>(args)...);
  }

  template<typename ...Args> _always_inline
  void emplace(iterator pos, Args&&... args) {
    construct(&(*pos), std::forward<Args>(args)...);
  }

  // Access an object in aligned storage
  constexpr _always_inline
  const T& operator[](std::size_t pos) const {
    return reinterpret_cast<const T&>(data_[pos]);
  }

  // Access an object in aligned storage
  _always_inline
  T& operator[](std::size_t pos) {
    return reinterpret_cast<T&>(data_[pos]);
  }

  // Delete objects from aligned storage
  void erase(std::size_t index) {
    destruct(reinterpret_cast<T*>(data_ + index));
  }

  void erase(iterator ptr) {
    destruct(&(*ptr));
  }

  void erase(iterator begin, iterator end) {
    destruct(begin, end);
  }

  constexpr std::size_t size() const {
    return N;
  }

  constexpr std::size_t length() const {
    return N;
  }

private:
  // propertly aligned uninitialized storage for N T's
  typename storage<T>::type data_[N];

#ifdef TURAG_HEAVY_DEBUG
  // just for better debugging
  // a reference to data_ only with type
  T (& ref)[N];
#endif
};

/// \endcond

} // namespace TURAG

#endif // ARRAY_STORAGE_H
