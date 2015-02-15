#ifndef TINAPP_RANGE_ITERATOR_RANGE_H
#define TINAPP_RANGE_ITERATOR_RANGE_H

#include <algorithm>
#include <iterator>

#include "traits.h"

namespace TURAG {

/// \addtogroup Ranges
/// \{

// from boost 1.54.0

namespace iterator_range_detail {

template< class Left, class Right >
inline bool less_than( const Left& l, const Right& r )
{
    return std::lexicographical_compare(std::begin(l),
                                        std::end(l),
                                        std::begin(r),
                                        std::end(r));
}

template< class Left, class Right >
inline bool greater_than( const Left& l, const Right& r )
{
    return less_than(r,l);
}

template< class Left, class Right >
inline bool less_or_equal_than( const Left& l, const Right& r )
{
    return !iterator_range_detail::less_than(r,l);
}

template< class Left, class Right >
inline bool greater_or_equal_than( const Left& l, const Right& r )
{
    return !iterator_range_detail::less_than(l,r);
}

struct range_tag { };
struct const_range_tag { };

}

//  iterator range template class -----------------------------------------//

//! iterator_range class
/*!
    An \c iterator_range delimits a range in a sequence by beginning and ending iterators.

    Many algorithms working with sequences take a pair of iterators,
    delimiting a working range, as an arguments. The \c iterator_range class is an
    encapsulation of a range identified by a pair of iterators.
    It provides a collection interface,
    so it is possible to pass an instance to an algorithm requiring a collection as an input.
*/
template<class IteratorT>
class iterator_range {
public:
    //! this type
    typedef iterator_range<IteratorT> type;

    //! Encapsulated value type
    typedef typename std::iterator_traits<IteratorT>::value_type value_type;

    //! Difference type
    typedef typename std::iterator_traits<IteratorT>::difference_type difference_type;

    //! Size type
    typedef std::size_t size_type; // note: must be unsigned

    //! This type
    typedef iterator_range<IteratorT> this_type;

    //! Reference type
    //
    // Needed because value-type is the same for
    // const and non-const iterators
    //
    typedef typename std::iterator_traits<IteratorT>::reference reference;

    //! const_iterator type
    /*!
        There is no distinction between const_iterator and iterator.
        These typedefs are provides to fulfill container interface
    */
    typedef IteratorT const_iterator;
    //! iterator type
    typedef IteratorT iterator;

public:
    iterator_range() : m_Begin( iterator() ), m_End( iterator() )
    { }

    //! Constructor from a pair of iterators
    template< class Iterator >
    iterator_range( Iterator Begin, Iterator End ) :
        m_Begin(Begin), m_End(End)
    {}

    //! Constructor from a Range
    template< class Range >
    iterator_range( const Range& r ) :
        m_Begin( std::begin( r ) ), m_End( std::end( r ) )
    {}

    //! Constructor from a Range
    template< class Range >
    iterator_range( Range& r ) :
        m_Begin( std::begin( r ) ), m_End( std::end( r ) )
    {}

    //! Constructor from a Range
    template< class Range >
    iterator_range( const Range& r, iterator_range_detail::const_range_tag ) :
        m_Begin( std::begin( r ) ), m_End( std::end( r ) )
    {}

    //! Constructor from a Range
    template< class Range >
    iterator_range( Range& r, iterator_range_detail::range_tag ) :
        m_Begin( std::begin( r ) ), m_End( std::end( r ) )
    {}

    template< class Iterator >
    iterator_range& operator=( const iterator_range<Iterator>& r )
    {
        m_Begin  = r.begin();
        m_End    = r.end();
        return *this;
    }

    template< class ForwardRange >
    iterator_range& operator=( ForwardRange& r )
    {
      m_Begin  = static_cast<IteratorT>(std::begin( r ));
      m_End    = static_cast<IteratorT>(std::end( r ));
        return *this;
    }

    template< class ForwardRange >
    iterator_range& operator=( const ForwardRange& r )
    {
      m_Begin  = static_cast<IteratorT>(std::begin( r ));
      m_End    = static_cast<IteratorT>(std::end( r ));
        return *this;
    }

    IteratorT begin() const
    {
        return m_Begin;
    }

    IteratorT end() const
    {
        return m_End;
    }

    difference_type size() const
    {
        return m_End - m_Begin;
    }

    bool empty() const
    {
        return m_Begin == m_End;
    }

    explicit
    operator bool() const
    {
        return m_Begin != m_End;
    }

    bool operator!() const
    {
        return empty();
    }

    bool equal( const iterator_range& r ) const
    {
        return m_Begin == r.m_Begin && m_End == r.m_End;
    }


public: // convenience
   reference front() const
   {
       return *m_Begin;
   }

   reference back() const
   {
       IteratorT last( m_End );
       return *--last;
   }

   reference operator[]( difference_type at ) const
   {
       return m_Begin[at];
   }

   iterator_range& advance_begin( difference_type n )
   {
       std::advance( m_Begin, n );
       return *this;
   }

   iterator_range& advance_end( difference_type n )
   {
       std::advance( m_End, n );
       return *this;
   }

private:
    // begin and end iterators
    IteratorT m_Begin;
    IteratorT m_End;

protected:
    //
    // Allow subclasses an easy way to access the
    // base type
    //
    typedef iterator_range iterator_range_;
};

//  iterator range free-standing operators ---------------------------//

/////////////////////////////////////////////////////////////////////
// comparison operators
/////////////////////////////////////////////////////////////////////

template< class IteratorT, class ForwardRange >
inline bool operator==( const ForwardRange& l,
                        const iterator_range<IteratorT>& r )
{
    return r.equal(l);
}

template< class IteratorT, class ForwardRange >
inline bool operator!=( const ForwardRange& l,
                        const iterator_range<IteratorT>& r )
{
    return !r.equal(l);
}

template< class IteratorT, class ForwardRange >
inline bool operator<( const ForwardRange& l,
                       const iterator_range<IteratorT>& r )
{
    return iterator_range_detail::less_than( l, r );
}

template< class IteratorT, class ForwardRange >
inline bool operator<=( const ForwardRange& l,
                        const iterator_range<IteratorT>& r )
{
    return iterator_range_detail::less_or_equal_than( l, r );
}

template< class IteratorT, class ForwardRange >
inline bool operator>( const ForwardRange& l,
                       const iterator_range<IteratorT>& r )
{
    return iterator_range_detail::greater_than( l, r );
}

template< class IteratorT, class ForwardRange >
inline bool operator>=( const ForwardRange& l,
                        const iterator_range<IteratorT>& r )
{
    return iterator_range_detail::greater_or_equal_than( l, r );
}

template< class Iterator1T, class Iterator2T >
inline bool operator<( const iterator_range<Iterator1T>& l,
                       const iterator_range<Iterator2T>& r )
{
    return iterator_range_detail::less_than( l, r );
}

template< class IteratorT, class ForwardRange >
inline bool operator<( const iterator_range<IteratorT>& l,
                       const ForwardRange& r )
{
    return iterator_range_detail::less_than( l, r );
}

template< class Iterator1T, class Iterator2T >
inline bool operator<=( const iterator_range<Iterator1T>& l,
                        const iterator_range<Iterator2T>& r )
{
    return iterator_range_detail::less_or_equal_than( l, r );
}

template< class IteratorT, class ForwardRange >
inline bool operator<=( const iterator_range<IteratorT>& l,
                        const ForwardRange& r )
{
    return iterator_range_detail::less_or_equal_than( l, r );
}

template< class Iterator1T, class Iterator2T >
inline bool operator>( const iterator_range<Iterator1T>& l,
                       const iterator_range<Iterator2T>& r )
{
    return iterator_range_detail::greater_than( l, r );
}

template< class IteratorT, class ForwardRange >
inline bool operator>( const iterator_range<IteratorT>& l,
                       const ForwardRange& r )
{
    return iterator_range_detail::greater_than( l, r );
}

template< class Iterator1T, class Iterator2T >
inline bool operator>=( const iterator_range<Iterator1T>& l,
                        const iterator_range<Iterator2T>& r )
{
    return iterator_range_detail::greater_or_equal_than( l, r );
}

template< class IteratorT, class ForwardRange >
inline bool operator>=( const iterator_range<IteratorT>& l,
                        const ForwardRange& r )
{
    return iterator_range_detail::greater_or_equal_than( l, r );
}

//! iterator_range construct helper
/*!
    Construct an \c iterator_range from a pair of iterators

    \param Begin A begin iterator
    \param End An end iterator
    \return iterator_range object
*/
template< typename IteratorT >
inline iterator_range< IteratorT >
make_iterator_range( IteratorT Begin, IteratorT End )
{
    return iterator_range<IteratorT>( Begin, End );
}

//! iterator_range construct helper
/*!
    Construct an \c iterator_range from a \c Range containing the begin
    and end iterators.
*/
template< class ForwardRange >
inline iterator_range< typename range_traits<ForwardRange>::iterator >
make_iterator_range( ForwardRange& r )
{
   return iterator_range< typename range_traits<ForwardRange>::iterator >
        ( r, iterator_range_detail::range_tag() );
}

template< class ForwardRange >
inline iterator_range< typename range_traits<const ForwardRange>::iterator >
make_iterator_range( const ForwardRange& r )
{
   return iterator_range< typename range_traits<const ForwardRange>::iterator >
        ( r, iterator_range_detail::const_range_tag() );
}

namespace iterator_range_detail
{
    template< class Range >
    inline iterator_range< typename range_traits<Range>::iterator >
    make_range_impl( Range& r,
                     typename range_traits<Range>::difference_type advance_begin,
                     typename range_traits<Range>::difference_type advance_end )
    {
        typename range_traits<Range>::iterator
            new_begin = std::begin( r ),
            new_end   = std::end( r );
        std::advance( new_begin, advance_begin );
        std::advance( new_end, advance_end );
        return make_iterator_range( new_begin, new_end );
    }
}

template< class Range >
inline iterator_range< typename range_traits<Range>::iterator >
make_iterator_range( Range& r,
            typename range_traits<Range>::difference_type advance_begin,
            typename range_traits<Range>::difference_type advance_end )
{
    return iterator_range_detail::make_range_impl( r, advance_begin, advance_end );
}

template< class Range >
inline iterator_range< typename range_traits<Range>::iterator >
make_iterator_range( const Range& r,
            typename range_traits<Range>::difference_type advance_begin,
            typename range_traits<Range>::difference_type advance_end )
{
    return iterator_range_detail::make_range_impl( r, advance_begin, advance_end );
}

/// \}

} // namespace TURAG

#endif // TINAPP_RANGE_ITERATOR_RANGE_H
