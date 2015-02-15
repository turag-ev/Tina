#ifndef TINAPP_RANGE_ALGORITHM_H
#define TINAPP_RANGE_ALGORITHM_H

#include <algorithm>

#include "../tina.h"
#include "../helper/static_const.h"

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

// TURAG::length mit Erweiterungspunkt:
// http://ericniebler.com/2014/10/21/customization-point-design-in-c11-and-beyond/
namespace _detail
{
  // Länge von Array
  template<class T, size_t N>
  constexpr std::size_t length(T (&a)[N] _unused) noexcept
  {
	return N;
  }

  // Länge von Container
  template<class Container>
  constexpr std::size_t length(Container&& container)
  {
	return std::forward<Container>(container).size();
  }

  // Erweiterungspunkt
  struct _length_fn
  {
	template<class Container>
	constexpr std::size_t operator()(Container&& container) const
	{
	  return length(std::forward<Container>(container));
	}
  };
}

#ifndef __DOXYGEN__
namespace
{
  // globales Funktionsobjekt
  constexpr auto const & length = _detail::static_const<_detail::_length_fn>::value;
}
#else

/// \brief Länge von Container zurückgeben
///
/// Gibt standardmäßig die Länge von Arrays und Containern (benutzt \c size Methode) zurück:
/// \code
/// int array[200];
/// std::size_t size = length(array); // size = 200
///
/// // for-Schleife:
/// for (std::size_t i = 0; i < length(array); i++) {
///   // mach was mit array[i]
/// }
///
/// // für Container
/// ArrayBuffer<100> array_buffer;
/// size = length(array_buffer); // benutzt array_buffer.size()
/// \endcode
///
/// Diese Funktion kann für andere Typen erweitert werden:
/// \code
/// namespace MyNamespace {
///
/// // nicht veränderbarer Container
/// struct MyContainer {
///   ArrayBuffer<100> buffer;
/// }
///
/// // Erweiterungsmethode
/// std::size_t length(MyContainer&& my_container) {
///   return std::forward<MyContainer>(my_container).buffer.size();
/// }
///
/// } // namespace MyNamespace
///
/// // Benutzung:
/// MyNamespace::MyContainer container;
/// auto size = TURAG::length(container);
///
/// \endcode
///
/// Als Inspiration diente dieser Artikel: http://ericniebler.com/2014/10/21/customization-point-design-in-c11-and-beyond/
///
/// \param container Container
/// \returns Länge von Container
template<class Container>
constexpr std::size_t length(Container&& container);

#endif

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
