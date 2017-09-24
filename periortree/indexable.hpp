#ifndef PERIOR_TREE_INDEXABLE_HPP
#define PERIOR_TREE_INDEXABLE_HPP
#include <periortree/rectangle.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/preprocessor/enum_params.hpp>
#include <boost/preprocessor/repeat_from_to.hpp>
#include <utility>

namespace perior
{

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<traits::is_point<pointT>, rectangle<pointT> >::type const&
make_aabb(rectangle<pointT> const& r) BOOST_NOEXCEPT_OR_NOTHROW
{
    return r;
}

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<traits::is_point<pointT>, rectangle<pointT> >::type
make_aabb(pointT const& p) BOOST_NOEXCEPT_OR_NOTHROW
{
    return rectangle<pointT>(p, p);
}

namespace traits
{
template<typename T>
struct is_indexable: boost::integral_constant<bool, is_point<T>::value>{};

template<typename pointT>
struct is_indexable<rectangle<pointT> >: boost::true_type{};
}// traits

template<typename T>
struct indexable_getter
{
    BOOST_STATIC_ASSERT(traits::is_indexable<T>::value);
    typedef T indexable_type;

    BOOST_FORCEINLINE
    T const& operator()(const T& value) const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return value;
    }
};

template<typename T0, typename T1>
struct indexable_getter<std::pair<T0, T1> >
{
    BOOST_STATIC_ASSERT(traits::is_indexable<T0>::value);
    typedef T0 indexable_type;

    BOOST_FORCEINLINE T0 const&
    operator()(const std::pair<T0, T1>& value) const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return value.first;
    }
};

#define PERIOR_TREE_GENERATE_INDEXABLE_GETTER(z, n, data)\
template<BOOST_PP_ENUM_PARAMS(n, typename T)>\
struct indexable_getter<boost::tuple<BOOST_PP_ENUM_PARAMS(n, T)> >\
{\
    BOOST_STATIC_ASSERT(traits::is_indexable<T0>::value);\
    typedef T0 indexable_type;\
    \
    BOOST_FORCEINLINE T0 const&\
    operator()(const boost::tuple<BOOST_PP_ENUM_PARAMS(n, T)>& value) const\
    BOOST_NOEXCEPT_OR_NOTHROW\
    {\
        return boost::get<0>(value);\
    }\
};\
/**/

BOOST_PP_REPEAT_FROM_TO(1, 10, PERIOR_TREE_GENERATE_INDEXABLE_GETTER, DUMMY)
#undef PERIOR_TREE_GENERATE_FN_GET_INDEXABLE

}// perior

#if __cplusplus >= 201103L
#include <tuple>
namespace perior
{

template<typename ...Ts>
struct indexable_getter<std::tuple<Ts...>>
{
    static_assert(traits::is_indexable<std::tuple<Ts...>>::value,
            "rtree value_type must have indexalbe type for its first type");

    BOOST_FORCEINLINE
    typename std::tuple_element<0, std::tuple<Ts...>>::type const&
    operator()(const std::tuple<Ts...>& value) const noexcept
    {
        return std::get<0>(value);
    }
};

}// perior
#endif // cpp11
#endif// PERIOR_TREE_INDEXABLE_HPP
