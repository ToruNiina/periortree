#ifndef PERIOR_TREE_POINT_OPERATIONS
#define PERIOR_TREE_POINT_OPERATIONS
#include <periortree/point_traits.hpp>
#include <boost/utility/enable_if.hpp>

namespace perior
{
namespace ops
{

template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type
operator+(const T& lhs, const T& rhs) BOOST_NOEXCEPT_OR_NOTHROW
{
    T retval;
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        retval[i] = lhs[i] + rhs[i];
    }
    return retval;
}
template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type&
operator+=(T& lhs, const T& rhs) BOOST_NOEXCEPT_OR_NOTHROW
{
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        lhs[i] += rhs[i];
    }
    return lhs;
}

template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type
operator-(const T& lhs, const T& rhs) BOOST_NOEXCEPT_OR_NOTHROW
{
    T retval;
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        retval[i] = lhs[i] - rhs[i];
    }
    return retval;
}
template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type&
operator-=(T& lhs, const T& rhs) BOOST_NOEXCEPT_OR_NOTHROW
{
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        lhs[i] -= rhs[i];
    }
    return lhs;
}

template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type
operator*(const T& lhs, const typename traits::scalar_type_of<T>::type& rhs)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    T retval;
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        retval[i] = lhs[i] * rhs;
    }
    return retval;
}
template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type
operator*(const typename traits::scalar_type_of<T>::type& lhs, const T& rhs)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    T retval;
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        retval[i] = lhs * rhs[i];
    }
    return retval;
}
template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type&
operator*=(T& lhs, const typename traits::scalar_type_of<T>::type& rhs)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        lhs[i] *= rhs;
    }
    return lhs;
}

template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type
operator/(const T& lhs, const typename traits::scalar_type_of<T>::type& rhs)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    T retval;
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        retval[i] = lhs[i] / rhs;
    }
    return retval;
}
template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type&
operator/=(T& lhs, const typename traits::scalar_type_of<T>::type& rhs)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        lhs[i] /= rhs;
    }
    return lhs;
}

template<typename T>
BOOST_FORCEINLINE
typename boost::enable_if<traits::is_point<T>,
         typename traits::scalar_type_of<T>::type>::type
length_sq(const T& lhs) BOOST_NOEXCEPT_OR_NOTHROW
{
    typename traits::scalar_type_of<T>::type retval(0);
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        retval += lhs[i] * lhs[i];
    }
    return retval;
}
template<typename T>
BOOST_FORCEINLINE
typename boost::enable_if<traits::is_point<T>,
         typename traits::scalar_type_of<T>::type>::type
length(const T& lhs) BOOST_NOEXCEPT_OR_NOTHROW
{
    return std::sqrt(length_sq(lhs));
}

template<typename T>
BOOST_FORCEINLINE typename boost::enable_if<traits::is_point<T>, T>::type
abs(const T& lhs) BOOST_NOEXCEPT_OR_NOTHROW
{
    T retval;
    for(std::size_t i=0; i<traits::dimension<T>::value; ++i)
    {
        retval += std::abs(lhs[i]);
    }
    return retval;
}

}// ops
}// perior
#endif// PERIOR_TREE_POINT_OPERATIONS
