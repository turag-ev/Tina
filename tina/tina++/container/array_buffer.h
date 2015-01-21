#ifndef TINA_ARRAY_H
#define TINA_ARRAY_H

#include <cstddef>
#include <iterator>
#include <cassert>
#include <algorithm>

#include "../tina.h"
#include "../debug.h"
#include "array_storage.h"

namespace TURAG {

#ifndef __DOXYGEN__
template<typename T>
struct _ArrayBufferHelper {
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

#ifdef NDEBUG
  static bool prepare_for_insert(iterator position, iterator end);

#else
  static bool prepare_for_insert(iterator position, iterator end, const_iterator max_end);
  static bool is_full(const_iterator end, const_iterator max_end);
#endif
};
#endif // __DOXYGEN__

/** \brief Array mit variabler Länge und festgelegter Kapazität.
 * \ingroup Container
 *
 * Es ist ein Äquilavent für <a href="http://en.cppreference.com/w/cpp/container/vector">std::vector</a>.
 * Der einzige Unterschied ist das die Kapazität nicht dynamisch wächst, sondern
 * sie wie bei <a href="http://en.cppreference.com/w/cpp/container/array">std::array</a> als Templateparameter \a N angegeben wird. Dadurch is
 * muss sie zur Kompilierzeit angegeben werden und kann sich nicht über die Laufzeit
 * verändern. Es werden alle Funktionen unterstützt die auch \a std::vector bietet.
 *
 * Durch die beschränkte Kapazität könnte es zu Pufferüberläufen kommen. Dies
 * wird abgefangen und eine Fehlermeldung ausgegeben. Das Element was hinzugefügt werden soll wird ignoiert.
 * Deshalb die Größe entsprechend groß wählen, lieber mehr als zu wenig.
 *
 * Das hinzufügen am Anfang mit \a push_front ist deutlich langsamer als das Hinzufügen
 * am Ende über \a push_back, da alle Elemente verschoben werden müssen. Gleiches gilt
 * für emplace_front, pop_front. Werden diese Funktionen oft aufgerufen, empfiehlt
 * es sich \a CircularBuffer zu verwenden.
 *
 * Ein \a ArrayBuffer kann kopiert und verschoben werden. Dabei werden alle Elemente
 * mit kopiert bzw. verschoben.
 *
 * \tparam T zu speicherender Typ der Elemente
 *           Die Elemente müssen kopier- oder verschiebbar sein.
 * \tparam N Angabe der Kapazität (also maximale Anzahl an Elementen von Typ \a T)
 */
template<typename T, size_t N>
struct ArrayBuffer {
  static_assert(N > 0, "ArrayBuffer size must be greater than null.");

public:
  typedef ArrayBuffer<T, N> self_type;

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

  // ctor
  /// ein ArrayBuffer erstellen
  constexpr explicit ArrayBuffer() :
	length_(0), bytes_()
  { }

  /// \brief ein ArrayBuffer zerstören
  /// \warning nie direkt aufrufen, für das Entfernen aller Elemente \a ArrayBuffer::clear benutzen.
  ~ArrayBuffer() {
	bytes_.erase(begin(), end());
  }

#ifndef __DOXYGEN__
  // FIXME: create own copy und move constructors for not trivial desctructable types
  COPYABLE(ArrayBuffer);
  MOVABLE(ArrayBuffer);
#endif

  // iterators
  /// gibt Zeiger auf erstes Element zurück
  iterator begin() {
	return iterator(&bytes_[0]);
  }
  /// gibt Zeiger auf erstes Element zurück
  constexpr
  const_iterator begin() const {
	return const_iterator(&bytes_[0]);
  }
  /// gibt Zeiger auf erstes Element zurück
  ///
  /// im Unterschied zu \a ArrayBuffer::begin wird immer ein \a ArrayBuffer::const_iterator zurückgegeben
  constexpr
  const_iterator cbegin() const {
	return const_iterator(&bytes_[0]);
  }

  /// gibt Zeiger auf letzes Element zurück, fürs Iterieren in umgekehrter Reihenfolge
  reverse_iterator rbegin() {
	return reverse_iterator(end());
  }
  /// gibt Zeiger auf letzes Element zurück, fürs Iterieren in umgekehrter Reihenfolge
  constexpr
  const_reverse_iterator rbegin() const {
	return reverse_iterator(end());
  }
  /// gibt Zeiger auf letzes Element zurück, fürs Iterieren in umgekehrter Reihenfolge
  ///
  /// im Unterschied zu \a ArrayBuffer::rbegin wird immer ein \a ArrayBuffer::const_reverse_iterator zurückgegeben
  constexpr
  const_reverse_iterator crbegin() const {
	return reverse_iterator(end());
  }

  /// gibt Zeiger auf letzes Element zurück
  iterator end() {
	return iterator(&bytes_[size()]);
  }
  /// gibt Zeiger auf letzes Element zurück
  constexpr
  const_iterator end() const {
	return const_iterator(&bytes_[size()]);
  }
  /// gibt Zeiger auf letzes Element zurück
  ///
  /// im Unterschied zu \a ArrayBuffer::end wird immer ein \a ArrayBuffer::const_iterator zurückgegeben
  constexpr
  const_iterator cend() const {
	return const_iterator(&bytes_[size()]);
  }

  /// gibt Zeiger auf erstes Element zurück, fürs Iterieren in umgekehrter Reihenfolge
  reverse_iterator rend() {
	return reverse_iterator(begin());
  }
  /// gibt Zeiger auf erstes Element zurück, fürs Iterieren in umgekehrter Reihenfolge
  constexpr
  const_reverse_iterator rend() const {
	return const_reverse_iterator(begin());
  }
  /// gibt Zeiger auf erstes Element zurück, fürs Iterieren in umgekehrter Reihenfolge
  ///
  /// im Unterschied zu \a ArrayBuffer::rend wird immer ein \a ArrayBuffer::const_reverse_iterator zurückgegeben
  constexpr
  const_reverse_iterator crend() const {
	return const_reverse_iterator(begin());
  }

  // capacity
  /// Gibt zurück, ob sich keine Elemente in Array befinden
  constexpr
  bool empty() const {
	return length_ == 0;
  }

  /// Gibt Anzahl der im Moment im Array befindlichen Elemente zurück
  constexpr
  size_type size() const {
	return length_;
  }
  /// Gibt Anzahl der im Moment im Array befindlichen Elemente zurück
  constexpr
  size_type length() const {
	return length_;
  }

  /// Gibt \a N zurück
  constexpr
  size_type capacity() const {
	return N;
  }
  /// Gibt \a N zurück
  constexpr
  size_type max_size() const {
	return capacity();
  }

  // element access
  /// Gibt Referenz auf erstes Element zurück
  reference front() {
	return *begin();
  }
  /// Gibt Referenz auf erstes Element zurück
  constexpr
  const_reference front() const {
	return *begin();
  }

  /// Gibt Referenz auf letztes Element zurück
  constexpr
  const_reference back() const {
	return *(end()-1);
  }
  /// Gibt Referenz auf letztes Element zurück
  reference back() {
	return *(end()-1);
  }

  ///@{
  /// Zugriff als n-tes Element
  /// \warning Es erfolgt keine Prüfung ob Element vorhanden ist.
  /// \pre n < ArrayBuffer::length()
  /// \param n Index von Element
  reference operator[](size_type n) {
	return bytes_[n];
  }

  /// Zugriff als n-tes Element
  /// \warning Es erfolgt keine Prüfung ob Element vorhanden ist.
  /// \pre n < ArrayBuffer::length()
  /// \param n Index von Element
  constexpr
  const_reference operator[](size_type n) const {
	return bytes_[n];
  }
  ///@}

  // modifiers

  /// erstes Element entfernen
  ///
  /// äquivalent zu array.erase(array.begin());
  ///
  /// alle nachfolgenden Elemente werden verschoben
  /// \code{.cpp}
  /// ArrayBuffer<int, 4> array;
  /// array.push_back(1);
  /// array.push_back(2);
  /// array.pop_front();
  /// // array = { 2 }
  /// \endcode
  void pop_front() {
	erase(begin());
  }

  /// letztes Element entfernen
  ///
  /// äquivalent zu array.erase(array.end() - 1);
  ///
  /// alle nachfolgenden Elemente werden verschoben
  /// \code{.cpp}
  /// ArrayBuffer<int, 4> array;
  /// array.push_back(1);
  /// array.push_back(2);
  /// array.pop_back();
  /// // array = { 1 }
  /// \endcode
  void pop_back() {
	if (!empty()) {
	  length_--;
	  bytes_.erase(length_);
	}
  }

  /// Element anfügen
  /// \code{.cpp}
  /// ArrayBuffer<int, 4> array;
  /// array.push_back(1);
  /// array.push_back(2);
  /// // array = {1, 2}
  /// \endcode
  void push_back(const value_type& val) {
	if (!is_full()) {
	  bytes_.emplace(length_, val);
	  length_++;
	}
  }

  /// Element für Konstructor anfügen.
  ///
  /// Ähnlich \a ArrayBuffer::push_back, nur werden Argumente direkt an Konstrutor von Typ \a T übergeben.
  /// Die Elemente werden direkt an ihrem neuen Speicherort erstellt, wodurch bei komplexen Typen ein Kopiervorgang entfällt.
  /// \code{.cpp}
  /// ArrayBuffer<int, 4> array;
  /// array.emplace_back(1);
  /// array.emplace_back(2);
  /// // array = {1, 2}
  ///
  /// struct A {
  ///     int x;
  ///     int y;
  ///
  ///     A(int x, int y) :
  ///         x(x), y(y)
  ///     { }
  /// };
  ///
  /// ArrayBuffer<A, 4> array;
  /// array.emplace_back(1, 2);
  /// array.emplace_back(3, 4);
  /// // array = { {x: 1, y: 2}, {x: 3, y: 4} }
  /// \endcode
  /// \param args Argumente für einen Konstruktor von Typ \a T um Element zu erstellen
  template<class... Args> _always_inline
  void emplace_back(Args&&... args) {
	if (!is_full()) {
	  bytes_.emplace(length_, std::forward<Args>(args)...);
	  length_++;
	}
  }

  /// Fügt Element \a val vorne an
  ///
  /// Wie ArrayBuffer::push_back wird Element vorne angefügt. Dadurch müssen
  /// alle folgenden Elemente verschoben werden, was entsprechend Zeit kostet.
  /// \param val Element was am Anfang des Arrays hinzugefügt werden soll.
  void push_front(const value_type& val) {
	insert(begin(), val);
  }

  /// Fügt Element vorne an
  ///
  /// Wie ArrayBuffer::emplace_back wird Element vorne angefügt. Dadurch müssen
  /// alle folgenden Elemente verschoben werden, was entsprechend Zeit kostet.
  /// \param args Argumente für einen Konstruktor von Typ \a T um Element zu erstellen
  template<class... Args> _always_inline
  void emplace_front(Args&&... args) {
	if (prepare_for_insert(begin())) {
	  length_++;
	  bytes_.emplace(begin(), std::forward<Args>(args)...);
	}
  }

  /// Alle Elemente entfernen
  /// \code{.cpp}
  /// ArrayBuffer<int, 1> array;
  /// array.push_back(1);
  /// array.push_back(2);
  /// array.push_back(3);
  ///
  /// array.clear(); // array = { }
  /// // äquivalent zu:
  /// array.erase(array.begin(), array.end())
  /// \endcode
  void clear() {
	bytes_.erase(begin(), end());
	length_ = 0;
  }

  /// Element an beliebiger Position einfügen
  ///
  /// ähnlich \a ArrayBuffer::push_back, nur an beliebiger Position in Array
  /// \code{.cpp}
  /// ArrayBuffer<int, 4> array;
  /// array.insert(array.begin(), 1); // äquivalent zu array.push_front(1);
  /// // array = {1}
  /// array.insert(array.end(), 2); // äquivalent zu array.push_back(1);
  /// // array = {1, 2}
  /// array.insert(array.begin() + 1, 3);
  /// // array = {1, 3, 2}
  /// \endcode
  /// \param position Iterator auf Element in Array an das Element eingefügt werden soll
  /// \param val einzufügendes Element
  void insert(iterator position, value_type val) {
	if (prepare_for_insert(position)) {
	  length_++;
	  bytes_.emplace(position, std::move(val));
	}
  }

  /// Element an beliebiger Position einfügen
  ///
  /// ähnlich \a sArrayBuffer::emplace_back, nur an beliebiger Position in Array
  /// \code{.cpp}
  /// struct A {
  ///     int x;
  ///     int y;
  ///
  ///     A(int x, int y) :
  ///         x(x), y(y)
  ///     { }
  /// };
  ///
  /// ArrayBuffer<A, 4> array;
  /// array.emplace(array.begin(), 1, 1); // äquivalent zu array.emplace_front(1);
  /// // array = { {x: 1, y: 1} }
  /// array.emplace(array.end(), 2, 5); // äquivalent zu array.emplace_back(1);
  /// // array = { {x: 1, y: 1}, {x: 2, y: 5} }
  /// array.emplace(array.begin() + 1, 3, 7);
  /// // array = { {x: 1, y: 1}, {x: 3, y: 7}, {x: 2, y: 5} }
  /// \endcode
  /// \param position Iterator auf Element in Array an das Element eingefügt werden soll
  /// \param args Argumente für einen Konstruktor von Typ \a T um Element zu erstellen
  template<class... Args> _always_inline
  void emplace(iterator position, Args&&... args) {
	if (prepare_for_insert(position)) {
	  length_++;
	  bytes_.emplace(position, std::forward<Args>(args)...);
	}
  }

  /// Element entfernen
  /// \param position Iterator auf Element, was entfernt werden soll
  void erase(iterator position) {
	std::move(position + 1, end(), position);
	length_--;
	bytes_.erase(end());
  }

  /// Elemente die entfernt werden sollen
  /// \code{.cpp}
  /// ArrayBuffer<int, 8> array;
  /// array.push_back(1);
  /// array.push_back(2);
  /// array.push_back(3);
  /// array.push_back(4);
  ///
  /// array.erase(array.begin() + 1, array.end() - 1);
  /// // bzw. array.erase(array.begin() + 1, array.begin() + 3);
  ///
  /// // array = {1, 4}
  /// \endcode
  /// \param first Iterator auf erstes zu entfernendes Element
  /// \param last Iterator erstes nicht zu entfernendes Element
  void erase(iterator first, iterator last) {
	iterator new_end = std::move(last, end(), first);
	bytes_.erase(new_end, end());
	length_ = new_end - begin();
  }

private:
  // number of elements in array
  std::size_t length_;

  // bytes for array
  ArrayStorage<T, N> bytes_;

  bool prepare_for_insert(iterator position) {
	return _ArrayBufferHelper<T>::prepare_for_insert(position, end(), const_iterator(&bytes_[N]));
  }

  bool is_full() {
	return _ArrayBufferHelper<T>::is_full(cend(), const_iterator(&bytes_[N]));
  }

#ifndef __DOXYGEN__
  friend struct _ArrayBufferHelper<T>;
#endif // __DOXYGEN__
};

#ifndef __DOXYGEN__

template<typename T>
bool _ArrayBufferHelper<T>::prepare_for_insert(iterator position, iterator end, const_iterator max_end) {
  if (end == max_end) {
	turag_error("ArrayBuffer overflow!");
	return false;
  }

  std::move_backward(position, end, end + 1);
  return true;
}

template<typename T>
bool _ArrayBufferHelper<T>::is_full(const_iterator end, const_iterator max_end) {
  if (end == max_end) {
	turag_error("ArrayBuffer overflow!");
	return true;
  }
  return false;
}

/*
template<typename T>
void ArrayBufferBase<T>::copy(const ArrayBufferBase<T>& other) {
	if (this == other) return;

	std::size_t new_size = other.size();
	if (new_size > max_size()) {
		turag_error("ArrayBuffer overflow!");
		new_size = max_size();
	}

	length_ = new_size;
	std::copy(other.begin(), other.begin() + new_size, begin());
}*/


template <typename T, size_t N>
static ArrayBuffer<size_t, N> sort_indexes(const ArrayBuffer<T, N> &v) {

  // initialize original index locations
  ArrayBuffer<size_t, N> idx;
  for (size_t i = 0; i != v.size(); ++i) idx[i] = i;

  // sort indexes based on comparing values in v
  std::sort(idx.begin(), idx.end(),
	   [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}


#endif // __DOXYGEN__

} // namespace TURAG

#endif // TINA_ARRAY_H
