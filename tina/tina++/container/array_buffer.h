#ifndef TINA_ARRAY_H
#define TINA_ARRAY_H

#include <iterator>
#include <cassert>
#include <algorithm>
#include <cstring>

#include "../tina.h"
#include "../algorithm.h"
#include <tina/debug/internal-debug.h>
#include "array_storage.h"

namespace TURAG {

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
  ~ArrayBuffer() {
	  // TODO: = default if std::is_trivally_destructable<T>::value
	bytes_.erase(begin(), end());
  }

  ArrayBuffer(const ArrayBuffer& other) :
	  length_(), bytes_()
  {
	  copy(other);
  }

  template<typename U, std::size_t M>
  ArrayBuffer(const ArrayBuffer<U, M>& other) :
	  length_(), bytes_()
  {
	  copy(other);
  }

  ArrayBuffer(ArrayBuffer&& other) :
	  length_(), bytes_()
  {
	  move(std::move(other));
  }

  template<typename U, std::size_t M>
  ArrayBuffer(ArrayBuffer<U, M>&& other) :
	  length_(), bytes_()
  {
	  move(std::move(other));
  }

  self_type& operator=(const ArrayBuffer& other)
  {
	  copy(other);
  }

  template<typename U, std::size_t M>
  self_type& operator=(const ArrayBuffer<U, M>& other)
  {
	  copy(other);
  }

  self_type& operator=(ArrayBuffer&& other)
  {
	  move(std::move(other));
  }

  template<typename U, std::size_t M>
  self_type& operator=(ArrayBuffer<U, M>&& other)
  {
	  move(std::move(other));
  }

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
	if (unlikely(empty())) return;

	length_--;
	bytes_.erase(length_);
  }

  /// Element anfügen
  /// \code{.cpp}
  /// ArrayBuffer<int, 4> array;
  /// array.push_back(1);
  /// array.push_back(2);
  /// // array = {1, 2}
  /// \endcode
  void push_back(const value_type& val) {
	if (unlikely(is_full())) return;

	bytes_.construct(length_, val);
	length_++;
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
	if (unlikely(is_full())) return;

	bytes_.construct(length_, std::forward<Args>(args)...);
	length_++;
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
	emplace(begin(), std::forward<Args>(args)...);
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
  void insert(iterator position, const value_type& val)
  {
	if (unlikely(is_full()))
	{
	  turag_internal_error("ArrayBuffer overflow!");
	  return;
	}

	const iterator last = end();

	length_++;

	if (std::is_trivially_copyable<T>::value)
	{
		value_type tmp(val);
		const ptrdiff_t n = last - position;
		if (n)
		  memmove(position + 1, position, sizeof(T) * n);
		bytes_.construct(position, tmp);
	}
	else
	{
		value_type tmp(val);
		if (position != last)
		{
			construct(last, std::move(*(last - 1)));
			TURAG::move_backward(position, last - 1, last);
			*position = tmp;
		}
		else
		{
			bytes_.construct(position, val);
		}
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
	  if (unlikely(is_full()))
	  {
		turag_internal_error("ArrayBuffer overflow!");
		return;
	  }

	  const iterator last = end();

	  length_++;

	  if (std::is_trivially_copyable<T>::value)
	  {
		  const ptrdiff_t n = last - position;
		  if (n)
			memmove(position + 1, position, sizeof(T) * n);
		  bytes_.construct(position, std::forward<Args>(args)...);
	  }
	  else
	  {
		  if (position != last)
		  {
			  construct(last, std::move(*(last - 1)));
			  TURAG::move_backward(position, last - 1, last);
			  *position = T(std::forward<Args>(args)...);
		  }
		  else
		  {
			  // es muss nichts verschoben werden, wir sind am Ende
			  bytes_.construct(position, std::forward<Args>(args)...);
		  }
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
	if (first == last) return;
	iterator new_end = std::move(last, end(), first);
	bytes_.erase(new_end, end());
	length_ = new_end - begin();
  }

  //remove all instances of a given value from the array
  void remove(const value_type& v) {
	  TURAG::remove(*this, v);
  }

private:
  // number of elements in array
  std::size_t length_;

  // bytes for array
  ArrayStorage<T, N> bytes_;

  bool is_full() {
	  if (unlikely(size() == capacity())) {
		turag_internal_error("ArrayBuffer overflow!");
		return true;
	  }
	  return false;
  }

  template<typename U, std::size_t M>
  void copy(const ArrayBuffer<U, M>& other) {
	  if (unlikely(this == &other)) return;

	  std::size_t new_size = other.size();
	  if (M > N) {
		  if (new_size > capacity()) {
			  turag_internal_error("ArrayBuffer overflow!");
			  new_size = capacity();
		  }
	  }

	  if (new_size <= size()) {
		  auto new_end = TURAG::copy(other.begin(), other.end(), begin());
		  destruct(new_end, end());
	  } else {
		  auto other_mid = other.begin() + size();
		  auto self_end = TURAG::copy(other.begin(), other_mid, begin());
		  TURAG::uninitialized_copy(other_mid, other.end(), self_end);
	  }

	  length_ = new_size;
  }

  template<typename U, std::size_t M>
  void move(ArrayBuffer<U, M>&& other) {
	  if (unlikely(this == &other)) return;

	  std::size_t new_size = other.size();
	  if (M > N) {
		  if (new_size > capacity()) {
			  turag_internal_error("ArrayBuffer overflow!");
			  new_size = capacity();
		  }
	  }

	  if (new_size <= size()) {
		  auto new_end = TURAG::move(other.begin(), other.end(), begin());
		  destruct(new_end, end());
	  } else {
		  auto other_mid = other.begin() + size();
		  auto self_end = TURAG::move(other.begin(), other_mid, begin());
		  TURAG::uninitialized_move(other_mid, other.end(), self_end);
	  }

	  length_ = new_size;
  }
};

#if 0
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
#endif

} // namespace TURAG

#endif // TINA_ARRAY_H
