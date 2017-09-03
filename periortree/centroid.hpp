#ifndef PERIOR_TREE_CENTROID_HPP
#define PERIOR_TREE_CENTROID_HPP
#include <periortree/aabb_traits.hpp>
#include <periortree/boundary_condition.hpp>

namespace perior
{
namespace detail
{

template<typename boxT, typename boundaryT, std::size_t I>
struct centroid_impl;

template<typename boxT, std::size_t I>
struct centroid_impl<
    boxT, unlimited_boundary<typename traits::point_type_of<boxT>::type>, I>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef traits::point_access<point_type, I-1>                 access_pt;
    typedef traits::box_access<box_type, traits::max_corner, I-1> access_max;
    typedef traits::box_access<box_type, traits::min_corner, I-1> access_min;
    typedef unlimited_boundary<point_type>  boundary_type;

    BOOST_FORCEINLINE
    static void
    invoke(point_type& a, const box_type& bx, const boundary_type& bd)
        BOOST_NOEXCEPT_IF(noexcept(
            access_min::get(std::declval<box_type>()) &&
            access_max::get(std::declval<box_type>()) &&
            access_pt::set(std::declval<point_type&>(),
                           std::declval<coordinate_type>())))
    {
        access_pt::set(a, (access_min::get(bx) + access_max::get(bx)) * 0.5);
        return centroid_impl<box_type, boundary_type, I-1>::invoke(a, bx, bd);
    }
};

template<typename boxT>
struct centroid_impl<
    boxT, unlimited_boundary<typename traits::point_type_of<boxT>::type>, 0>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef unlimited_boundary<point_type>                        boundary_type;

    BOOST_FORCEINLINE
    static void
    invoke(point_type& a, const box_type& bx, const boundary_type& bd)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }
};

template<typename boxT, std::size_t I>
struct centroid_impl<
    boxT, cubic_periodic_boundary<typename traits::point_type_of<boxT>::type>, I>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef traits::point_access<point_type, I-1>                 access_pt;
    typedef traits::box_access<box_type, traits::max_corner, I-1> access_max;
    typedef traits::box_access<box_type, traits::min_corner, I-1> access_min;
    typedef unlimited_boundary<point_type>               boundary_type;
    typedef traits::box_range_access<boundary_type, I-1> access_range;

    BOOST_FORCEINLINE
    static void
    invoke(point_type& a, const box_type& bx, const boundary_type& bd)
        BOOST_NOEXCEPT_IF(noexcept(
            access_min::get(bx) && access_min::get(bd) &&
            access_max::get(bx) && access_max::get(bd) &&
            access_pt::set(a, std::declval<coordinate_type>())))
    {
        const coordinate_type box_min = access_min::get(bx);
        const coordinate_type box_max = access_max::get(bx);
        const coordinate_type d =
            adjust_direction_1D(box_max - box_min, access_range::get(bd));
        access_pt::set(a, adjust_position_1D(box_min + d, box_min, box_max));

        return centroid_impl<box_type, boundary_type, I-1>::invoke(a, bx, bd);
    }
};

template<typename boxT>
struct centroid_impl<
    boxT, cubic_periodic_boundary<typename traits::point_type_of<boxT>::type>, 0>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef unlimited_boundary<point_type>                        boundary_type;

    BOOST_FORCEINLINE
    static void
    invoke(point_type& a, const box_type& bx, const boundary_type& bd)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }
};

} // detail

template<typename boxT, typename boundaryT>
inline
typename boost::enable_if<
    boost::is_same<typename traits::tag<boxT>::type, traits::aabb_tag>,
    typename traits::point_type_of<boxT>::type>::type
centroid(const boxT& bx, const boundaryT& bd)
    BOOST_NOEXCEPT_IF(noexcept(detail::area_impl<boxT, boundaryT,
            traits::dimension_of<boxT>::value>::invoke(1, bx, bd)))
{
    typename traits::point_type_of<boxT>::type center;
    detail::centroid_impl<boxT, boundaryT, traits::dimension_of<boxT>::value
            >::invoke(center, bx, bd);
    return center;
}


} // perior
#endif//PERIOR_TREE_CENTROID_HPP
