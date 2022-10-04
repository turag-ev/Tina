#ifndef TINAPP_CONTAINER_ARRAY_STORAGE_H
#define TINAPP_CONTAINER_ARRAY_STORAGE_H

#include <type_traits>
#include <iterator>

#include "../tina.h"
#include "../helper/construct.h"
#include "../helper/traits_algorithms.h"

namespace TURAG {

/// \cond INCLUDE_HELPER

/// \brief Stellt Speicher für Klasse bereit, der entsprechend des Typs ausgerichtet ist.
/// \ingroup Utils
/// Wird verwendet in \ref TURAG::ArrayStorage
template<typename T>
using TypeStorage = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;

/// \brief tellt Speicher für Union bereit, der entsprechend der inhaltenen Typen ausgerichtet ist.
/// \ingroup Utils
/// Wird verwendet in \ref TURAG::VariantClass
template<typename... Types>
using UnionStorage = typename std::aligned_union<max_value(sizeof(Types)...), Types...>::type;

/// \brief Klasse die Speicher für Arrays bereitstellt
/// \ingroup Utils
/// Es wird kein Konstruktor beim Erstellen von \a array_storage aufgerufen
///
/// Es wird auch keine Information gespeichert welche Elemente initalisiert sind.
/// Dafür ist der Benutzer der Klasse verantwortlich.
///
/// Wird verwendet in \ref TURAG::ArrayBuffer und \ref TURAG::CircularBuffer
template<typename T, size_t N>
struct ArrayStorage {
public:
  ArrayStorage(const ArrayStorage&) = default;
  ArrayStorage& operator=(const ArrayStorage&) = default;
  ArrayStorage(ArrayStorage&&) = default;
  ArrayStorage& operator=(ArrayStorage&&) = default;

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

  typedef TypeStorage<T> element_type;

  constexpr TURAG_ALWAYS_INLINE
  ArrayStorage() :
    data_()
#ifdef TURAG_HEAVY_DEBUG
    , ref(reinterpret_cast<T(*)[N]>(data_))
#endif
  {}

  // Create an object in aligned storage
  template<typename ...Args> TURAG_ALWAYS_INLINE
  void construct(std::size_t index, Args&&... args) {
	TURAG::construct(reinterpret_cast<T*>(data_ + index), std::forward<Args>(args)...);
  }

  template<typename ...Args> TURAG_ALWAYS_INLINE
  static void construct(iterator pos, Args&&... args) {
	TURAG::construct(pos, std::forward<Args>(args)...);
  }

  template<typename ...Args> TURAG_ALWAYS_INLINE
  void assign(std::size_t index, T&& arg) {
	*reinterpret_cast<T*>(data_ + index) = std::move(arg);
  }

  template<typename ...Args> TURAG_ALWAYS_INLINE
  static void assign(iterator pos, T&& arg) {
	*(pos) = std::move(arg);
  }

  // Access an object in aligned storage
  constexpr TURAG_ALWAYS_INLINE
  const T& operator[](std::size_t pos) const {
    return reinterpret_cast<const T&>(data_[pos]);
  }

  // Access an object in aligned storage
  TURAG_ALWAYS_INLINE
  T& operator[](std::size_t pos) {
    return reinterpret_cast<T&>(data_[pos]);
  }

  // Delete objects from aligned storage
  void erase(std::size_t index) {
    destruct(reinterpret_cast<T*>(data_ + index));
  }

  static void erase(iterator ptr) {
    destruct(&(*ptr));
  }

  static void erase(iterator begin, iterator end) {
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
  TypeStorage<T> data_[N];

#ifdef TURAG_HEAVY_DEBUG
  // TODO: use union without TURAG_HEAVY_DEBUG; remove TURAG_HEAVY_DEBUG
  // just for better debugging
  // a reference to data_ only with type
  T (* ref)[N];
#endif
};

/// \endcond

} // namespace TURAG

#endif // TINAPP_CONTAINER_ARRAY_STORAGE_H
