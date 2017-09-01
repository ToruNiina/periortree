#ifndef PERIOR_TREE_AABB_TRAITS_HPP
#define PERIOR_TREE_AABB_TRAITS_HPP
#include <periortree/point_traits.hpp>

namespace perior
{
namespace traits
{

struct aabb_tag{};

template<typename T> struct tag{};
template<typename T> struct point_type_of{};

struct max_corner{};
struct min_corner{};

template<typename T, typename Corner, std::size_t D>
struct box_access{};
template<typename T, std::size_t D>
struct box_range_access{};

namespace detail
{

template<typename boxT, std::size_t I>
struct area_impl
{
    typedef boxT box_type;
    typedef typename point_type_of<box_type>::type        point_type;
    typedef typename coordinate_type_of<point_type>::type coordinate_type;
    typedef box_range_access<box_type, max_corner, I-1>   access_range;

    BOOST_FORCEINLINE
    static coordinate_type
    invoke(coordinate_type a, const box_type& bx)
        BOOST_NOEXCEPT_IF(noexcept(access_range::get(std::declval<box_type>())))
    {
        return area_impl<pointT, I-1>::invoke(a * access_range::get(bx), bx);
    }
};

template<typename boxT, std::size_t I>
struct area_impl
{
    typedef boxT box_type;
    typedef typename point_type_of<box_type>::type        point_type;
    typedef typename coordinate_type_of<point_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type
    invoke(coordinate_type a, const box_type& bx) BOOST_NOEXCEPT_OR_NOTHROW
    {
        return a;
    }
};
} // detail

template<typename boxT>
inline
typename boost::enable_if<boost::is_same<typename tag<boxT>::type, aabb_tag>,
         typename coordinate_type_of<typename point_type_of<boxT>::type>::type
         >::type
area(const boxT& b)
{
    typedef typename point_type_of<boxT>::type point_t;
    return detail::area_impl<point_t, dimension_of<point_t>::value
        >::invoke(1, retval, b);
}


}// traits
}// perior
#endif//PERIOR_TREE_AABB_TRAITS_HPP
