#ifndef TINAPP_RANGE_SUB_RANGE_H
#define TINAPP_RANGE_SUB_RANGE_H

#include "iterator_range.h"
#include "traits.h"

namespace TURAG {

/// \addtogroup Ranges
/// \{

template< class ForwardRange >
class sub_range : public iterator_range< typename range_traits<ForwardRange>::iterator >
{
    typedef typename range_traits<ForwardRange>::iterator iterator_t;
    typedef iterator_range< iterator_t  > base;

public:
    typedef typename range_traits<ForwardRange>::value_type      value_type;
    typedef typename range_traits<ForwardRange>::iterator        iterator;
    typedef typename range_traits<const ForwardRange>::iterator  const_iterator;
    typedef typename range_traits<ForwardRange>::difference_type difference_type;
    typedef typename range_traits<ForwardRange>::size_type       size_type;
    typedef typename base::reference                             reference;

public: // for return value of front/back
    typedef const typename std::remove_reference<reference>::type& const_reference;

public:
    sub_range() : base()
    { }

    template< class ForwardRange2 >
    sub_range( ForwardRange2& r ) :
        base( r )
    { }

    template< class ForwardRange2 >
    sub_range( const ForwardRange2& r ) :
        base( r )
    { }

    template< class Iter >
    sub_range( Iter first, Iter last ) :
        base( first, last )
    { }

    template< class ForwardRange2 >
    sub_range& operator=( ForwardRange2& r )
    {
        base::operator=( r );
        return *this;
    }

    template< class ForwardRange2 >
    sub_range& operator=( const ForwardRange2& r )
    {
        base::operator=( r );
        return *this;
    }

    sub_range& operator=( const sub_range& r )
    {
        base::operator=( static_cast<const base&>(r) );
        return *this;
    }

public:

    iterator        begin()          { return base::begin(); }
    const_iterator  begin() const    { return base::begin(); }
    iterator        end()            { return base::end();   }
    const_iterator  end() const      { return base::end();   }
    difference_type size() const     { return base::size();  }


public: // convenience
    reference front()
    {
        return base::front();
    }

    const_reference front() const
    {
        return base::front();
    }

    reference back()
    {
        return base::back();
    }

    const_reference back() const
    {
        return base::back();
    }

    reference operator[]( difference_type sz )
    {
        return base::operator[](sz);
    }

    const_reference operator[]( difference_type sz ) const
    {
        return base::operator[](sz);
    }

};

template< class ForwardRange, class ForwardRange2 >
inline bool operator==( const sub_range<ForwardRange>& l,
                        const sub_range<ForwardRange2>& r )
{
    return r.equal(l);
}

template< class ForwardRange, class ForwardRange2 >
inline bool operator!=( const sub_range<ForwardRange>& l,
                        const sub_range<ForwardRange2>& r )
{
    return !r.equal(l);
}

template< class ForwardRange, class ForwardRange2 >
inline bool operator<( const sub_range<ForwardRange>& l,
                       const sub_range<ForwardRange2>& r )
{
    return iterator_range_detail::less_than( l, r );
}

/// \}

} // namespace TURAG

#endif // TINAPP_RANGE_SUB_RANGE_H
