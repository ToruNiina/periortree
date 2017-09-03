#ifndef PERIOR_TREE_AREA_HPP
#define PERIOR_TREE_AREA_HPP
#include <periortree/aabb_traits.hpp>
#include <periortree/boundary_condition.hpp>

namespace perior
{
namespace detail
{

template<typename boxT, typename boundaryT, std::size_t I>
struct area_impl;

template<typename boxT, std::size_t I>
struct area_impl<
    boxT, unlimited_boundary<typename traits::point_type_of<boxT>::type>, I>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef unlimited_boundary<point_type>  boundary_type;
    typedef traits::box_range_access<box_type, I-1>      access_bx_range;
    typedef traits::box_range_access<boundary_type, I-1> access_bd_range;

    BOOST_FORCEINLINE
    static coordinate_type
    invoke(coordinate_type a, const box_type& bx, const boundary_type& bd)
        BOOST_NOEXCEPT_IF(noexcept(access_bx_range::get(std::declval<box_type>())))
    {
        return area_impl<box_type, boundary_type, I-1>::invoke(
                a * access_bx_range::get(bx), bx, bd);
    }
};

template<typename boxT>
struct area_impl<
    boxT, unlimited_boundary<typename traits::point_type_of<boxT>::type>, 0>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef unlimited_boundary<point_type>  boundary_type;

    BOOST_FORCEINLINE
    static coordinate_type
    invoke(coordinate_type a, const box_type& bx, const boundary_type&)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return a;
    }
};

template<typename boxT, std::size_t I>
struct area_impl<
    boxT, cubic_periodic_boundary<typename traits::point_type_of<boxT>::type>, I>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef cubic_periodic_boundary<point_type>  boundary_type;
    typedef traits::box_access<box_type, traits::max_corner, I-1> access_max;
    typedef traits::box_access<box_type, traits::min_corner, I-1> access_min;
    typedef traits::box_range_access<box_type, I-1>      access_bx_range;
    typedef traits::box_range_access<boundary_type, I-1> access_bd_range;

    BOOST_FORCEINLINE
    static coordinate_type
    invoke(coordinate_type a, const box_type& bx, const boundary_type& bd)
        BOOST_NOEXCEPT_IF(
            noexcept(access_bx_range::get(std::declval<box_type>())) &&
            noexcept(access_bd_range::get(std::declval<boundary_type>()))
            )
    {
        const coordinate_type dx = access_bx_range::get(bx);
        return area_impl<box_type, boundary_type, I-1>::invoke(
                a * (dx >= 0 ? dx : dx + access_bd_range::get(bd)), bx, bd);
    }
};

template<typename boxT>
struct area_impl<
    boxT, cubic_periodic_boundary<typename traits::point_type_of<boxT>::type>, 0>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef cubic_periodic_boundary<point_type>           boundary_type;

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
typename boost::enable_if<
    boost::is_same<typename traits::tag<boxT>::type, traits::aabb_tag>,
    typename traits::coordinate_type_of<boxT>::type>::type
area(const boxT& bx, const boundaryT& bd)
    BOOST_NOEXCEPT_IF(noexcept(detail::area_impl<boxT, boundaryT,
            traits::dimension_of<boxT>::value>::invoke(1, bx, bd)))
{
    return detail::area_impl<boxT, boundaryT, traits::dimension_of<boxT>::value
            >::invoke(1, bx, bd);
}

}// perior
#endif//PERIOR_TREE_AREA_HPP
