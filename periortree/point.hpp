#ifndef PERIOR_TREE_POINT
#define PERIOR_TREE_POINT
#include <periortree/point_traits.hpp>
#include <periortree/point_ops.hpp>

#if __cplusplus >= 201103L
#include <initializer_list>
#include <algorithm>
#endif

namespace perior
{

template<typename T, std::size_t N>
struct point
{
    typedef T scalar_type;
    BOOST_STATIC_CONSTEXPR std::size_t dimension = N;
    typedef T value_type;
    typedef boost::array<T, N> container_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;

    point(){values_.fill(0);}
    BOOST_DEFAULTED_FUNCTION(~point(), {})

#if __cplusplus >= 201103L
    template<typename ...Ts,
        typename boost::enable_if_c<sizeof...(Ts) == dimension, std::nullptr_t
            >::type = nullptr>
    point(Ts&& ... xs) noexcept : values_{{std::forward<scalar_type>(xs)...}}{}

    point(std::initializer_list<scalar_type> il)
    {
        if(il.size() != N)
        {
            throw std::invalid_argument("initializer-list has invalid size");
        }
        std::copy(il.begin(), il.end(), this->values_.begin());
    }
#endif

    point(const point& rhs): values_(rhs.values_){}
    point& operator=(const point& rhs){values_ = rhs.values_; return *this;}

    BOOST_FORCEINLINE scalar_type&       operator[](const std::size_t i)
        BOOST_NOEXCEPT_OR_NOTHROW {return values_[i];}
    BOOST_FORCEINLINE scalar_type const& operator[](const std::size_t i) const
        BOOST_NOEXCEPT_OR_NOTHROW {return values_[i];}

    BOOST_FORCEINLINE scalar_type&       at(const std::size_t i)
    {return values_.at(i);}
    BOOST_FORCEINLINE scalar_type const& at(const std::size_t i) const
    {return values_.at(i);}

    BOOST_CONSTEXPR std::size_t size() const BOOST_NOEXCEPT_OR_NOTHROW
    {return dimension;}

    BOOST_FORCEINLINE iterator       begin()
        BOOST_NOEXCEPT_OR_NOTHROW {return values_.begin();}
    BOOST_FORCEINLINE iterator       end()
        BOOST_NOEXCEPT_OR_NOTHROW {return values_.end();}
    BOOST_FORCEINLINE const_iterator begin()  const
        BOOST_NOEXCEPT_OR_NOTHROW {return values_.begin();}
    BOOST_FORCEINLINE const_iterator end()    const
        BOOST_NOEXCEPT_OR_NOTHROW {return values_.end();}
    BOOST_FORCEINLINE const_iterator cbegin() const
        BOOST_NOEXCEPT_OR_NOTHROW {return values_.begin();}
    BOOST_FORCEINLINE const_iterator cend()   const
        BOOST_NOEXCEPT_OR_NOTHROW {return values_.end();}

  private:

    container_type values_;
};


template<typename T, std::size_t N, typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const point<T, N>& rhs)
{
    os << '{';
    for(std::size_t i=0; i<N; ++i)
    {
        os << rhs[i] << " ";
    }
    os << '}';
    return os;
}

using ops::operator+;
using ops::operator+=;
using ops::operator-;
using ops::operator-=;
using ops::operator*;
using ops::operator*=;
using ops::operator/;
using ops::operator/=;
using ops::operator==;
using ops::operator!=;
using ops::length_sq;
using ops::length;
using ops::abs;

namespace traits
{
template<typename T, std::size_t N>
struct is_point<point<T, N> > : boost::true_type {};
template<typename T, std::size_t N>
struct dimension<point<T, N> > : boost::integral_constant<std::size_t, N>{};
template<typename T, std::size_t N>
struct scalar_type_of<point<T, N> >{typedef T type;};
template<typename T, std::size_t N>
struct zero_vector_impl<point<T, N> >
{
    static BOOST_FORCEINLINE point<T, N> invoke() BOOST_NOEXCEPT_OR_NOTHROW
    {
        return point<T, N>();
    }
};
}// traits
}// perior
#endif// PERIOR_TREE_POINT
