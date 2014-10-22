#ifndef TINAPP_RANGE_ALGORITHM_H
#define TINAPP_RANGE_ALGORITHM_H

#include <algorithm>

#include "../tina.h"

namespace TURAG {

/// \addtogroup Ranges
/// \{

////////////////////////////////////////////////////////////////////////////////
// predicates for std algorithm e.x. std::find, ...

#ifndef __DOXYGEN__
/// Prädikat zum Vergleichen auf Gleichheit
template<typename T>
struct is_equal_predicate {
  typedef T type;

  template<typename U>
  constexpr explicit
  is_equal_predicate(U&& t) :
    value(std::move(t))
  { }

  constexpr
  bool operator() (const T& t) {
    return t == value;
  }

  T value;
};
#endif // __DOXYGEN__

/// Prädikat, um zu prüfen ob Objekt einem anderem gleicht.
/// \param t zu vergleichendes Objekt
/// \return Prädikat zum Vergleichen auf Gleichheit
/// \code
/// std::find(container.begin(), container.end(), 42);
/// // entspricht:
/// std::find_if(container.begin(), container.end(), is_equal(42));
/// \endcode
template<typename T> constexpr
is_equal_predicate<typename std::remove_reference<T>::type> is_equal(T&& t) {
  return is_equal_predicate<typename std::remove_reference<T>::type>(std::forward<T>(t));
}

////////////////////////////////////////////////////////////////////////////////
// std extention

/// \brief Elemente mit Wert \p val von Container entfernen und entsprechend Größe anpassen.
/// \param container Container
/// \param val Wert von Element, dass entfernt werden soll
/// \return Referenz auf Container
/// Verwendet std::remove zum Entfernen des Containers und die Memberfunktion \e erase von Container
/// um Größe anzupassen.
template<typename Container, typename T>
Container& remove(Container& container, const T& val) {
  auto old_end = std::end(container);
  auto new_end = std::remove(std::begin(container), old_end, val);
  container.erase(new_end, old_end);
  return container;
}

/// \brief Elemente, bei dem Prädikat \e true zurückgibt, von Container entfernen und entsprechend Größe anpassen.
/// \param container Container
/// \param pred Prädikat, das angibt, ob Element entfernt werden soll.
/// \return Referenz auf Container
/// Verwendet std::remove_if zum Entfernen des Containers und die Memberfunktion \e erase von Container
/// um Größe anzupassen.
template<typename Container, typename Predicate>
Container& remove_if(Container& container, Predicate pred) {
  auto old_end = std::end(container);
  auto new_end = std::remove_if(std::begin(container), old_end, pred);
  container.erase(new_end, old_end);
  return container;
}

/// \brief Gibt an, ob Bereich mindestens ein Element mit dem Wert \p val besitzt.
/// \param range Bereich in dem gesucht werden soll
/// \param val Wert nach dem gesucht werden soll
/// \retval true mindestens ein Element mit dem Wert \p val existiert in Bereich
/// \retval false sonst
template <typename Range, typename T>
bool any_of_value(const Range& range, const T& val) {
  return std::find(std::begin(range), std::end(range), val) != std::end(range);
}

/// \brief Gibt an, ob Bereich mindestens ein Element besitzt, bei dem das Prädikat \a true zurückliefert.
/// \param range Bereich in dem gesucht werden soll
/// \param pred Prädikat
/// \retval true mindestens ein Element existiert, bei dem das Prädikat \a true zurückliefert.
/// \retval false sonst
template <typename Range, typename Pred>
bool any_of(const Range& range, Pred pred) {
  return std::any_of(std::begin(range), std::end(range), pred);
}

/// \brief Gibt an, ob Bereich kein Element mit dem Wert \p val besitzt.
/// \param range Bereich in dem gesucht werden soll
/// \param val Wert nach dem gesucht werden soll
/// \retval true kein Element mit dem Wert \p val existiert in Bereich
/// \retval false sonst
template <typename Range, typename T>
bool none_of_value(const Range& range, const T& val) {
  return std::find(std::begin(range), std::end(range), val) == std::end(range);
}

/// \brief Gibt an, ob Bereich kein Element besitzt, bei dem das Prädikat \a true zurückliefert.
/// \param range Bereich in dem gesucht werden soll
/// \param pred Prädikat
/// \retval true kein Element existiert, bei dem das Prädikat \a true zurückliefert.
/// \retval false sonst
template <typename Range, typename Pred>
bool none_of(const Range& range, Pred pred) {
  return std::none_of(std::begin(range), std::end(range), pred);
}


/// Länge von Container zurückgeben
/// \param container Container
/// \returns Länge von Container
template <typename Container>
std::size_t length(const Container& container) {
  return container.size();
}

/// Länge von Array zurückgeben
/// \param array Array
/// \returns Länge von Array
template <typename T, std::size_t N>
constexpr std::size_t length(const T (&)[N]) {
  return N;
}

////////////////////////////////////////////////////////////////////////////////
// iterator extentions

///
/// \brief einen rückwärts arbeitenden Iterator in den vorwärts arbeitenden umwandeln.
/// \param rit Rückwärtsiterator
/// \return Forwärtsiterator
///
/// \warning Rückgabewert ist undefiniert, wenn letzter Rückwärtsiterator von Container übergeben wird. Der erster Iterator, der außerhalb von Container ist, hat keinen zugeordneten Forwärtsiterator z.B. <code>container.rend()</code>.
template <class ReverseIterator>
typename ReverseIterator::iterator_type make_forward(ReverseIterator rit) {
  return rit.base() - 1;
}

/// \}

} // namespace TURAG

#endif // TINAPP_RANGE_ALGORITHM_H
