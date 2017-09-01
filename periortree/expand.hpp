#ifndef PERIOR_TREE_EXPAND_HPP
#define PERIOR_TREE_EXPAND_HPP
#include <periortree/aabb_traits.hpp>
#include <periortree/boundary_condition.hpp>

namespace perior
{
namespace detail
{

template<typename boxT, typename boundaryT, std::size_t I>
struct expand_impl;

template<typename boxT, std::size_t I>
struct expand_impl<
    boxT, unlimited_boundary<typename traits::point_type_of<boxT>::type>, I>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef traits::box_access<box_type, traits::max_corner, I-1> access_max;
    typedef traits::box_access<box_type, traits::min_corner, I-1> access_min;
    typedef traits::box_range_access<box_type, I-1>               access_range;
    typedef unlimited_boundary<point_type> boundary_type;

    BOOST_FORCEINLINE
    static void
    invoke(box_type& box, const box_type& entry, const boundary_type& bd)
        BOOST_NOEXCEPT_IF(
            noexcept(access_min::get(std::declval<box_type>())) &&
            noexcept(access_min::set(std::declval<box_type&>(),
                     std::declval<coordinate_type>())) &&
            noexcept(access_max::get(std::declval<box_type>())) &&
            noexcept(access_max::set(std::declval<box_type&>(),
                     std::declval<coordinate_type>())))
    {
        access_min::set(box, std::min(access_min(box), access_min(entry)));
        access_max::set(box, std::max(access_max(box), access_max(entry)));
        return expand_impl<box_type, boundary_type, I>::invoke(box, entry, bd);
    }
};

template<typename boxT>
struct expand_impl<
    boxT, unlimited_boundary<typename traits::point_type_of<boxT>::type>, 0>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef traits::box_access<box_type, traits::max_corner, I-1> access_max;
    typedef traits::box_access<box_type, traits::min_corner, I-1> access_min;
    typedef traits::box_range_access<box_type, I-1>               access_range;
    typedef unlimited_boundary<point_type> boundary_type;

    BOOST_FORCEINLINE
    static void
    invoke(box_type& box, const box_type& entry, const boundary_type& bd)
        BOOST_NOEXCEPT_IF_OR_NOTHROW
    {
        return;
    }
};


template<typename boxT, std::size_t I>
struct expand_impl<
    boxT, cubic_periodic_boundary<typename traits::point_type_of<boxT>::type>, I>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef traits::box_access<box_type, traits::max_corner, I-1> access_max;
    typedef traits::box_access<box_type, traits::min_corner, I-1> access_min;
    typedef traits::box_range_access<box_type, I-1>               access_range;
    typedef cubic_periodic_boundary<point_type> boundary_type;

    BOOST_FORCEINLINE
    static void
    invoke(box_type& box, const box_type& entry, const boundary_type& bd)
        BOOST_NOEXCEPT_IF(
            noexcept(access_min::get(std::declval<box_type>())) &&
            noexcept(access_min::set(std::declval<box_type&>(),
                     std::declval<coordinate_type>())) &&
            noexcept(access_max::get(std::declval<box_type>())) &&
            noexcept(access_max::set(std::declval<box_type&>(),
                     std::declval<coordinate_type>())))
    {
//         access_min::set(box, std::min(access_min(box), access_min(entry)));
//         access_max::set(box, std::max(access_max(box), access_max(entry)));
//         return expand_impl<box_type, boundary_type, I>::invoke(box, entry, bd);
    }
};

template<typename boxT>
struct expand_impl<
    boxT, cubic_periodic_boundary<typename traits::point_type_of<boxT>::type>, 0>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef traits::box_access<box_type, traits::max_corner, I-1> access_max;
    typedef traits::box_access<box_type, traits::min_corner, I-1> access_min;
    typedef traits::box_range_access<box_type, I-1>               access_range;
    typedef cubic_periodic_boundary<point_type> boundary_type;

    BOOST_FORCEINLINE
    static void
    invoke(box_type& box, const box_type& entry, const boundary_type& bd)
        BOOST_NOEXCEPT_IF_OR_NOTHROW
    {
        return;
    }
};

} // detail

template<typename boxT, typename boundaryT>
inline
typename boost::enable_if<
    boost::is_same<typename traits::tag<boxT>::type, traits::aabb_tag>, void
    >::type
expand(boxT& box, const boxT& entry, const boundaryT& bd)
    BOOST_NOEXCEPT_IF(noexcept(detail::expand_impl<
            typename traits::point_type_of<boxT>::type,
            traits::dimension_of<typename traits::point_type_of<boxT>::type>::value
        >::invoke(1, bx, bd)))
{
    detail::expand_impl<boxT, boundaryT,
        traits::dimension_of<typename traits::point_type_of<boxT>::type>::value
            >::invoke(box, entry, bd);
    return;
}


}// perior
#endif// PERIOR_TREE_EXPAND_HPP
