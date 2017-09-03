#ifndef PERIOR_TREE_EXPAND_HPP
#define PERIOR_TREE_EXPAND_HPP
#include <periortree/aabb_traits.hpp>
#include <periortree/boundary_condition.hpp>
#include <boost/assert.hpp>
#include <boost/cstdint.hpp>
#include <limits>
#include <iostream>
#include <algorithm>

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
        access_min::set(box, std::min(access_min::get(box), access_min::get(entry)));
        access_max::set(box, std::max(access_max::get(box), access_max::get(entry)));
        return expand_impl<box_type, boundary_type, I-1>::invoke(box, entry, bd);
    }

    BOOST_FORCEINLINE
    static void
    invoke(box_type& box, const point_type& entry, const boundary_type& bd)
        BOOST_NOEXCEPT_IF(
            noexcept(access_min::get(std::declval<box_type>())) &&
            noexcept(access_min::set(std::declval<box_type&>(),
                     std::declval<coordinate_type>())) &&
            noexcept(access_max::get(std::declval<box_type>())) &&
            noexcept(access_max::set(std::declval<box_type&>(),
                     std::declval<coordinate_type>())))
    {
        access_min::set(box, std::min(access_min::get(box), access_min::get(entry)));
        access_max::set(box, std::max(access_max::get(box), access_max::get(entry)));
        return expand_impl<box_type, boundary_type, I-1>::invoke(box, entry, bd);
    }
};

template<typename boxT>
struct expand_impl<
    boxT, unlimited_boundary<typename traits::point_type_of<boxT>::type>, 0>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef unlimited_boundary<point_type> boundary_type;

    BOOST_FORCEINLINE
    static void
    invoke(box_type& box, const box_type& entry, const boundary_type& bd)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }

    BOOST_FORCEINLINE
    static void
    invoke(box_type& box, const point_type& entry, const boundary_type& bd)
        BOOST_NOEXCEPT_OR_NOTHROW
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
    typedef traits::box_access<boundary_type, traits::max_corner, I-1> access_bd_max;
    typedef traits::box_access<boundary_type, traits::min_corner, I-1> access_bd_min;
    typedef traits::box_range_access<boundary_type, I-1>               access_bd_range;

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
        const coordinate_type period = access_bd_range::get(bd);

        const coordinate_type bx_min = access_min::get(box);
        const coordinate_type bx_max = access_max::get(box);
        const coordinate_type en_min = access_min::get(entry);
        const coordinate_type en_max = access_max::get(entry);

        const boost::int8_t within_ = within(en_min, en_max, bx_min, bx_max, period);
        if(within_ == 1)
        {// no expansion needed
            return expand_impl<box_type, boundary_type, I-1
                >::invoke(box, entry, bd);
        }
        else if(within_ == -1)
        {// entry includes box.
            access_min::set(box, en_min);
            access_max::set(box, en_max);
            return expand_impl<box_type, boundary_type, I-1
                >::invoke(box, entry, bd);
        }

        const coordinate_type box_c   = calc_center(bx_min, bx_max, period);
        const coordinate_type entry_c = calc_center(en_min, en_max, period);
        const coordinate_type dc = adjust_direction_1D(box_c - entry_c, period);

        BOOST_ASSERT(dc != 0);
        coordinate_type expanded_l, expanded_u;
        if(dc < 0)
        {
            expanded_l = box_c   - access_range::get(box)   / 2;
            expanded_u = entry_c + access_range::get(entry) / 2;
        }
        else
        {
            expanded_l = entry_c - access_range::get(entry) / 2;
            expanded_u = box_c   + access_range::get(box)   / 2;
        }

        const coordinate_type bd_min = access_bd_min::get(bd);
        const coordinate_type bd_max = access_bd_max::get(bd);
        expanded_l = adjust_position_1D(expanded_l, bd_min, bd_max);
        expanded_u = adjust_position_1D(expanded_u, bd_min, bd_max);

        access_min::set(box, expanded_l);
        access_max::set(box, expanded_u);

        BOOST_ASSERT(within(en_min, en_max, expanded_l, expanded_u, period)==1);
        BOOST_ASSERT(within(bx_min, bx_max, expanded_l, expanded_u, period)==1);

        return expand_impl<box_type, boundary_type, I-1>::invoke(box, entry, bd);
    }

  private:

    // return 1 if range1 is inside of range2, -1 if range2 is inside of range1,
    //        0 otherwise.
    BOOST_FORCEINLINE
    static boost::int8_t
    within(coordinate_type l1, coordinate_type u1,
           coordinate_type l2, coordinate_type u2,
           const coordinate_type period) BOOST_NOEXCEPT_OR_NOTHROW
    {
        if(l1 > u1){u1 += period;}
        if(l2 > u2){u2 += period;}

        const coordinate_type period_half = period / 2;
        const coordinate_type c1 = (l1 + u1) / 2;
        const coordinate_type c2 = (l2 + u2) / 2;
        if(c1 - c2 > period_half)
        {
            l1 -= period;
            u1 -= period;
        }
        else if(c1 - c2 < -period_half)
        {
            l2 -= period;
            u2 -= period;
        }

        if(l2 <= l1 && u1 <= u2)
        {
            return 1;
        }
        else if(l1 <= l2 && u2 <= u1)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    BOOST_FORCEINLINE
    static coordinate_type
    calc_center(const coordinate_type lw, const coordinate_type up,
                const coordinate_type period) BOOST_NOEXCEPT_OR_NOTHROW
    {
        return (lw < up) ? (up + lw) / 2 : (up + lw + period) / 2;
    }
};

template<typename boxT>
struct expand_impl<
    boxT, cubic_periodic_boundary<typename traits::point_type_of<boxT>::type>, 0>
{
    typedef boxT box_type;
    typedef typename traits::point_type_of<box_type>::type        point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
    typedef cubic_periodic_boundary<point_type> boundary_type;

    BOOST_FORCEINLINE
    static void
    invoke(box_type& box, const box_type& entry, const boundary_type& bd)
        BOOST_NOEXCEPT_OR_NOTHROW
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
