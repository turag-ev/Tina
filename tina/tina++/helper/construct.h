#ifndef TINAPP_HELPER_CONSTRUCT_H
#define TINAPP_HELPER_CONSTRUCT_H

#include <type_traits>
#include <iterator>
#include <cstring>

#include "normalize.h"

namespace TURAG {

/// \addtogroup Utils
/// \{

/// Erstelle Instanz von Klasse in vorhandenen Speicher mit zugehörigen Argumenten
/// \param ptr Zeiger auf nicht initalisierten Speicher an dem Instanz erstellt wird
/// \param args Argumente für Konstuktor
/// \warning Nur verwenden, wenn C++ Instanz nicht selber verwaltet, weil
/// zum Beispiel ein Bytepuffer verwendet wird.
template<typename T, typename ...Args> TURAG_ALWAYS_INLINE
void construct(T* ptr, Args&&... args) {
  ::new(ptr) T(std::forward<Args>(args)...);
}

#ifndef __DOXYGEN__

template<typename T, typename std::enable_if<std::is_trivially_destructible<T>::value, bool>::type = false> TURAG_ALWAYS_INLINE
void destruct(T*) { }

template<typename T, typename std::enable_if<!std::is_trivially_destructible<T>::value, bool>::type = false> TURAG_ALWAYS_INLINE
void destruct(T* ptr) {
  ptr->~T();
}

template<typename ForwardIterator, typename std::enable_if<std::is_trivially_destructible<typename std::iterator_traits<ForwardIterator>::value_type>::value, bool>::type = false> TURAG_ALWAYS_INLINE
void destruct(ForwardIterator, ForwardIterator) { }

template<typename ForwardIterator, typename std::enable_if<!std::is_trivially_destructible<typename std::iterator_traits<ForwardIterator>::value_type>::value, bool>::type = false>
void destruct(ForwardIterator first, ForwardIterator last) {
  for (; first != last; ++first) {
    destruct(std::addressof(*first));
  }
}

#else // __DOXYGEN__

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

#endif // __DOXYGEN__

// copy & move

namespace detail {

template<bool, bool, typename>
struct copy_move
{
	template<typename II, typename OI>
	static OI copy_m(II first, II last, OI result)
	{
		for (; first != last; ++result, ++first)
			*result = *first;
		return result;
	}
};

template<typename Category>
struct copy_move<true, false, Category>
{
	template<typename II, typename OI>
	static OI copy_m(II first, II last, OI result)
	{
		for (; first != last; ++result, ++first)
			*result = std::move(*first);
		return result;
	}
};

template<>
struct copy_move<false, false, std::random_access_iterator_tag>
{
	template<typename II, typename OI>
	static OI copy_m(II first, II last, OI result)
	{
		typedef typename std::iterator_traits<II>::difference_type Distance;
		for (Distance n = last - first; n > 0; --n)
		{
			*result = *first;
			++first;
			++result;
		}
		return result;
	}
};

template<>
struct copy_move<true, false, std::random_access_iterator_tag>
{
	template<typename II, typename OI>
	static OI copy_m(II first, II last, OI result)
	{
		typedef typename std::iterator_traits<II>::difference_type Distance;
		for(Distance n = last - first; n > 0; --n)
		{
			*result = std::move(*first);
			++first;
			++result;
		}
		return result;
	}
};

template<bool IsMove>
struct copy_move<IsMove, true, std::random_access_iterator_tag>
{
	template<typename T>
	static T* copy_m(const T* first, const T* last, T* result)
	{
		// trivial types can have deleted assignment
		static_assert(std::is_copy_assignable<T>::value,
					  "type is not assignable");

		const std::ptrdiff_t n = last - first;
		if (n)
			std::memmove(result, first, sizeof(T) * n);
		return result + n;
	}
};

template<bool IsMove, typename II, typename OI>
inline OI copy_move_a(II first, II last, OI result)
{
	typedef typename std::iterator_traits<II>::value_type ValueTypeI;
	typedef typename std::iterator_traits<OI>::value_type ValueTypeO;
	typedef typename std::iterator_traits<II>::iterator_category Category;
	const bool simple = (std::is_trivially_copyable<ValueTypeI>::value &&
						 std::is_pointer<II>::value &&
						 std::is_pointer<OI>::value &&
						 std::is_same<ValueTypeI, ValueTypeO>::value);

	return copy_move<IsMove, simple, Category>::copy_m(first, last, result);
}

} // namespace detail

/// \brief wie std::copy nur besser optimiert
template<typename II, typename OI>
inline OI copy(II first, II last, OI result)
{
	return detail::copy_move_a<false>(first, last, result);
}

/// \brief wie std::move nur besser optimiert
template<typename II, typename OI>
inline OI move(II first, II last, OI result)
{
	return detail::copy_move_a<true>(first, last, result);
}

// copy & move backward

namespace detail {

template<bool, bool, typename>
struct copy_move_backward
{
	template<typename BI1, typename BI2>
	static BI2 copy_move_b(BI1 first, BI1 last, BI2 result)
	{
		while (first != last)
			*--result = *--last;
		return result;
	}
};

template<typename Category>
struct copy_move_backward<true, false, Category>
{
	template<typename BI1, typename BI2>
	static BI2 copy_move_b(BI1 first, BI1 last, BI2 result)
	{
		while (first != last)
			*--result = std::move(*--last);
		return result;
	}
};

template<>
struct copy_move_backward<false, false, std::random_access_iterator_tag>
{
	template<typename BI1, typename BI2>
	static BI2 copy_move_b(BI1 first, BI1 last, BI2 result)
	{
		typename std::iterator_traits<BI1>::difference_type n;
		for (n = last - first; n > 0; --n)
			*--result = *--last;
		return result;
	}
};

template<>
struct copy_move_backward<true, false, std::random_access_iterator_tag>
{
	template<typename BI1, typename BI2>
	static BI2 copy_move_b(BI1 first, BI1 last, BI2 result)
	{
		typename std::iterator_traits<BI1>::difference_type n;
		for (n = last - first; n > 0; --n)
			*--result = std::move(*--last);
		return result;
	}
};

template<bool IsMove>
struct copy_move_backward<IsMove, true, std::random_access_iterator_tag>
{
	template<typename T>
	static T* copy_move_b(const T* first, const T* last, T* result)
	{
		// trivial types can have deleted assignment
		static_assert(std::is_copy_assignable<T>::value,
					  "type is not assignable");

		const std::ptrdiff_t n = last - first;
		result -= n;
		if (n)
			std::memmove(result, first, sizeof(T) * n);
		return result;
	}
};

template<bool IsMove, typename BI1, typename BI2>
inline BI2
copy_move_backward_a(BI1 first, BI1 last, BI2 result)
{
	typedef typename std::iterator_traits<BI1>::value_type ValueType1;
	typedef typename std::iterator_traits<BI2>::value_type ValueType2;
	typedef typename std::iterator_traits<BI1>::iterator_category Category;
	const bool simple = std::is_trivially_copy_assignable<ValueType1>::value &&
						std::is_pointer<BI1>::value &&
						std::is_pointer<BI2>::value &&
						std::is_same<typename std::remove_const<ValueType1>::type, ValueType2>::value;

	return copy_move_backward<IsMove, simple, Category>::copy_move_b(first, last, result);
}

} // namespace detail

/// \brief wie std::copy_backward nur besser optimiert
template<typename BI1, typename BI2>
inline BI2
copy_backward(BI1 first, BI1 last, BI2 result)
{
	return detail::copy_move_backward_a<false>(first, last, result);
}

/// \brief wie std::copy_backward nur besser optimiert
template<typename BI1, typename BI2>
inline BI2 move_backward(BI1 first, BI1 last, BI2 result)
{
	return detail::copy_move_backward_a<true>(first, last, result);
}

// uninitialized copy & move

namespace detail {

template<bool, bool, typename>
struct uninitialized_copy_move
{
	template<typename II, typename OI>
	static OI copy_m(II first, II last, OI result)
	{
		for (; first != last; ++result, ++first)
			TURAG::construct(std::addressof(*result), *first);
		return result;
	}
};

template<typename Category>
struct uninitialized_copy_move<true, false, Category>
{
	template<typename II, typename OI>
	static OI copy_m(II first, II last, OI result)
	{
		for (; first != last; ++result, ++first)
			TURAG::construct(std::addressof(*result), std::move(*first));
		return result;
	}
};

template<>
struct uninitialized_copy_move<false, false, std::random_access_iterator_tag>
{
	template<typename II, typename OI>
	static OI copy_m(II first, II last, OI result)
	{
		typedef typename std::iterator_traits<II>::difference_type Distance;
		for (Distance n = last - first; n > 0; --n)
		{
			TURAG::construct(std::addressof(*result), *first);
			++first;
			++result;
		}
		return result;
	}
};

template<>
struct uninitialized_copy_move<true, false, std::random_access_iterator_tag>
{
	template<typename II, typename OI>
	static OI copy_m(II first, II last, OI result)
	{
		typedef typename std::iterator_traits<II>::difference_type Distance;
		for(Distance n = last - first; n > 0; --n)
		{
			TURAG::construct(std::addressof(*result), std::move(*first));
			++first;
			++result;
		}
		return result;
	}
};

template<bool IsMove>
struct uninitialized_copy_move<IsMove, true, std::random_access_iterator_tag>
{
	template<typename T>
	static T* copy_m(const T* first, const T* last, T* result)
	{
		// trivial types can have deleted assignment
		static_assert(std::is_copy_assignable<T>::value,
					  "type is not assignable");

		const std::ptrdiff_t n = last - first;
		if (n)
			std::memmove(result, first, sizeof(T) * n);
		return result + n;
	}
};

template<bool IsMove, typename II, typename OI>
inline OI uninitialized_copy_move_a(II first, II last, OI result)
{
	typedef typename std::iterator_traits<II>::value_type ValueTypeI;
	typedef typename std::iterator_traits<OI>::value_type ValueTypeO;
	typedef typename std::iterator_traits<II>::iterator_category Category;
	const bool simple = (std::is_trivially_copy_assignable<ValueTypeI>::value &&
						 std::is_pointer<II>::value &&
						 std::is_pointer<OI>::value &&
						 std::is_same<typename std::remove_const<ValueTypeI>::type, ValueTypeO>::value);

	return uninitialized_copy_move<IsMove, simple, Category>::copy_m(first, last, result);
}

} // namespace detail

/// wie std::uninitialized_copy nur besser optimiert
template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
				   ForwardIterator result)
{
	return detail::uninitialized_copy_move_a<false>(first, last, result);
}

/// wie std::uninitialized_copy mit std::move_iterator's nur besser optimiert
template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_move(InputIterator first, InputIterator last,
				   ForwardIterator result)
{
	return detail::uninitialized_copy_move_a<true>(first, last, result);
}

// uninitialized copy & move backward

namespace detail {

template<bool, bool, typename>
struct uninitialized_copy_move_backward
{
	template<typename BI1, typename BI2>
	static BI2 copy_move_b(BI1 first, BI1 last, BI2 result)
	{
		while (first != last)
			TURAG::construct(std::addressof(*result), *first);
		return result;
	}
};

template<typename Category>
struct uninitialized_copy_move_backward<true, false, Category>
{
	template<typename BI1, typename BI2>
	static BI2 copy_move_b(BI1 first, BI1 last, BI2 result)
	{
		while (first != last)
			TURAG::construct(std::addressof(*--result), std::move(*--last));
		return result;
	}
};

template<>
struct uninitialized_copy_move_backward<false, false, std::random_access_iterator_tag>
{
	template<typename BI1, typename BI2>
	static BI2 copy_move_b(BI1 first, BI1 last, BI2 result)
	{
		typename std::iterator_traits<BI1>::difference_type n;
		for (n = last - first; n > 0; --n)
			TURAG::construct(std::addressof(*--result), *--last);
		return result;
	}
};

template<>
struct uninitialized_copy_move_backward<true, false, std::random_access_iterator_tag>
{
	template<typename BI1, typename BI2>
	static BI2 copy_move_b(BI1 first, BI1 last, BI2 result)
	{
		typename std::iterator_traits<BI1>::difference_type n;
		for (n = last - first; n > 0; --n)
			TURAG::construct(std::addressof(*--result), std::move(*--last));
		return result;
	}
};

template<bool IsMove>
struct uninitialized_copy_move_backward<IsMove, true, std::random_access_iterator_tag>
{
	template<typename T>
	static T* copy_move_b(const T* first, const T* last, T* result)
	{
		// trivial types can have deleted assignment
		static_assert(std::is_copy_assignable<T>::value,
					  "type is not assignable");

		const ptrdiff_t n = last - first;
		if (n)
			std::memmove(result - n, first, sizeof(T) * n);
		return result - n;
	}
};

template<bool IsMove, typename BI1, typename BI2>
inline BI2
uninitialized_copy_move_backward_a(BI1 first, BI1 last, BI2 result)
{
	typedef typename std::iterator_traits<BI1>::value_type ValueType1;
	typedef typename std::iterator_traits<BI2>::value_type ValueType2;
	typedef typename std::iterator_traits<BI1>::iterator_category Category;
	const bool simple = std::is_trivially_copy_constructible<ValueType1>::value &&
						std::is_pointer<BI1>::value &&
						std::is_pointer<BI2>::value &&
						std::is_same<typename std::remove_const<ValueType1>::type, ValueType2>::value;

	return uninitialized_copy_move_backward<IsMove, simple, Category>::copy_move_b(first, last, result);
}

} // namespace detail

/// \brief wie std::copy_backward nur besser optimiert
template<typename BI1, typename BI2>
inline BI2 uninitialized_copy_backward(BI1 first, BI1 last, BI2 result)
{
	return detail::copy_move_backward_a<false>(first, last, result);
}

/// \brief wie std::copy_backward nur besser optimiert
template<typename BI1, typename BI2>
inline BI2 uninitialized_move_backward(BI1 first, BI1 last, BI2 result)
{
	return detail::copy_move_backward_a<true>(first, last, result);
}

/// \}

} // namespace TURAG

#endif // TINAPP_HELPER_CONSTRUCT_H
