#ifndef PERIOR_TREE_AABB_TRAITS_HPP
#define PERIOR_TREE_AABB_TRAITS_HPP
#include <periortree/point_traits.hpp>
#include <periortree/boundary_condition.hpp>

namespace perior
{
namespace traits
{

struct aabb_tag{};

template<typename T> struct point_type_of{};

struct max_corner{};
struct min_corner{};

template<typename T, typename Corner, std::size_t D>
struct box_access{};
template<typename T, std::size_t D>
struct box_range_access{};

namespace detail
{
template<typename boxT, typename boundaryT, std::size_t I>
struct area_impl;

template<typename boxT, std::size_t I>
struct area_impl<boxT, unlimited_boundary<typename point_type_of<boxT>::type>, I>
{
    typedef boxT box_type;
    typedef typename point_type_of<box_type>::type        point_type;
    typedef typename coordinate_type_of<point_type>::type coordinate_type;
    typedef box_range_access<box_type, I-1> access_range;
    typedef unlimited_boundary<point_type>  boundary_type;

    BOOST_FORCEINLINE
    static coordinate_type
    invoke(coordinate_type a, const box_type& bx, const boundary_type& bd)
        BOOST_NOEXCEPT_IF(noexcept(access_range::get(std::declval<box_type>())))
    {
        return area_impl<box_type, boundary_type, I-1>::invoke(
                a * access_range::get(bx), bx, bd);
    }
};

template<typename boxT>
struct area_impl<boxT, unlimited_boundary<typename point_type_of<boxT>::type>, 0>
{
    typedef boxT box_type;
    typedef typename point_type_of<box_type>::type        point_type;
    typedef typename coordinate_type_of<point_type>::type coordinate_type;
    typedef unlimited_boundary<point_type>  boundary_type;

    BOOST_FORCEINLINE
    static coordinate_type
    invoke(coordinate_type a, const box_type& bx, const boundary_type&)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return a;
    }
};
} // detail

template<typename boxT, typename boundaryT>
inline
typename boost::enable_if<boost::is_same<typename tag<boxT>::type, aabb_tag>,
         typename coordinate_type_of<typename point_type_of<boxT>::type>::type
         >::type
area(const boxT& bx, const boundaryT& bd)
    BOOST_NOEXCEPT_IF(noexcept(detail::area_impl<
            typename point_type_of<boxT>::type,
            dimension_of<typename point_type_of<boxT>::type>::value
        >::invoke(1, bx, bd)))
{
    return detail::area_impl<boxT, boundaryT,
        dimension_of<typename point_type_of<boxT>::type>::value
            >::invoke(1, bx, bd);
}











}// traits
}// perior
#endif//PERIOR_TREE_AABB_TRAITS_HPP
