#ifndef TINAPP_RANGE_TRAITS_H
#define TINAPP_RANGE_TRAITS_H

#include <iterator>

namespace TURAG {

#ifndef __DOXYGEN__
namespace range_traits_detail {

////////////////////////////////////////////////////////////////////////////////

template< typename C >
struct range_mutable_iterator
{
  typedef typename C::iterator type;
};

template< typename Iterator >
struct range_mutable_iterator< std::pair<Iterator,Iterator> >
{
    typedef Iterator type;
};

template< typename T, std::size_t sz >
struct range_mutable_iterator< T[sz] >
{
    typedef T* type;
};

////////////////////////////////////////////////////////////////////////////////

template< typename C >
struct range_const_iterator
{
  typedef typename C::const_iterator type;
};

template< typename Iterator >
struct range_const_iterator< std::pair<Iterator,Iterator> >
{
    typedef Iterator type;
};

template< typename T, std::size_t sz >
struct range_const_iterator< T[sz] >
{
    typedef const T* type;
};

////////////////////////////////////////////////////////////////////////////////

template<class C, class Enable = void>
struct range_iterator {
  // std::enable_if<!std::is_const<C>::value == true
  typedef typename range_mutable_iterator<C>::type type;
};

template< typename C >
struct range_iterator<C, typename std::enable_if<std::is_const<C>::value >::type>
{
    typedef typename range_const_iterator< typename std::remove_const<C>::type >::type type;
};

////////////////////////////////////////////////////////////////////////////////

template< typename C >
struct range_size
{
  typedef typename C::size_type type;
};

template< typename Iterator >
struct range_size< std::pair<Iterator,Iterator> >
{
    typedef std::size_t type;
};

template< typename T, std::size_t sz >
struct range_size< T[sz] >
{
    typedef std::size_t type;
};

} // namespace range_traits_detail
#endif __DOXYGEN__

////////////////////////////////////////////////////////////////////////////////
// range_traits

/// \addtogroup Ranges
/// \{

template< typename C >
struct range_traits {
  typedef typename range_traits_detail::range_iterator<C>::type iterator;
  typedef typename range_traits_detail::range_size<C>::type size_type;

  // from std::iterator_traits
  typedef typename std::iterator_traits<iterator>::difference_type difference_type;
  typedef typename std::iterator_traits<iterator>::value_type value_type;
  typedef typename std::iterator_traits<iterator>::pointer pointer;
  typedef typename std::iterator_traits<iterator>::reference reference;
  typedef typename std::iterator_traits<iterator>::iterator_category iterator_category;
};

/// \}

} // namespace TURAG

#endif // TINAPP_RANGE_TRAITS_H
