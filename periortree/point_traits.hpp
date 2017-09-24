#ifndef PERIOR_TREE_POINT_TRAITS
#define PERIOR_TREE_POINT_TRAITS
#include <boost/type_traits.hpp>
#include <boost/array.hpp>

namespace perior
{
namespace traits
{

template<typename T>
struct is_point : boost::false_type{};
template<typename T, std::size_t N>
struct is_point<boost::array<T, N> > : boost::true_type{};

template<typename T>
struct dimension : boost::integral_constant<std::size_t, 0> {};
template<typename T, std::size_t N>
struct dimension<boost::array<T, N> >: boost::integral_constant<std::size_t, N>{};

template<typename T>
struct scalar_type_of{};
template<typename T, std::size_t N>
struct scalar_type_of<boost::array<T, N> >{typedef T type;};

template<typename T>
struct zero_vector_impl
{
    static BOOST_FORCEINLINE T invoke() BOOST_NOEXCEPT_OR_NOTHROW
    {
        T retval;
        for(std::size_t i=0; i<dimension<T>::value; ++i)
        {
            retval[i] = 0;
        }
        return retval;
    }
};
template<typename T, std::size_t N>
struct zero_vector_impl<boost::array<T, N> >
{
    static BOOST_FORCEINLINE boost::array<T, N> invoke() BOOST_NOEXCEPT_OR_NOTHROW
    {
        boost::array<T, N> retval; retval.fill(0);
        return retval;
    }
};

template<typename T>
BOOST_FORCEINLINE T zero_vector()
{
    return zero_vector_impl<T>::invoke();
}

} // traits
} // perior
#endif//PERIOR_TREE_POINT_TRAITS
