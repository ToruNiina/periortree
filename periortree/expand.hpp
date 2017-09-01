#ifndef PERIOR_TREE_EXPAND_HPP
#define PERIOR_TREE_EXPAND_HPP
#include <periortree/aabb_traits.hpp>
#include <periortree/boundary_condition.hpp>
#include <limit>

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
    static coordinate_type
    calc_range(const coordinate_type up, const coordinate_type lw,
               const coordinate_type period)
    {
        const coordinate_type dx = up - lw;
        return (dx > 0) ? dx : dx + period;
    }

    BOOST_FORCEINLINE
    static std::pair<coordinate_type, coordinate_type>
    merge_range(const std::pair<coordinate_type, coordinate_type>& r1,
                const std::pair<coordinate_type, coordinate_type>& r2)
    {
        return std::make_pair(
                std::min(r1.first, r2.first), std::max(r1.second, r2.second));
    }

    inline
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
        const coordinate_type period = access_range::get(bd);

        boost::container::small_vector<
            std::pair<coordinate_type, coordinate_type>, 3> origins;
        boost::container::small_vector<
            std::pair<coordinate_type, coordinate_type>, 3> entries;

        const coordinate_type o_lw = access_min::get(box);
        const coordinate_type o_up = access_min::get(box);
        const coordinate_type e_lw = access_min::get(entry);
        const coordinate_type e_up = access_min::get(entry);

        if(o_lw < o_up)
        {
            origins.push_back(o_lw, o_up);
            origins.push_back(o_lw - period, o_up - period);
            origins.push_back(o_lw + period, o_up + period);
        }
        else
        {
            origins.push_back(o_lw, o_up + period);
            origins.push_back(o_lw - period, o_up);
        }

        if(e_lw < e_up)
        {
            entries.push_back(e_lw, e_up);
            entries.push_back(e_lw - period, e_up - period);
            entries.push_back(e_lw + period, e_up + period);
        }
        else
        {
            entries.push_back(e_lw, e_up + period);
            entries.push_back(e_lw - period, e_up);
        }

        coordinate_type range = std::numeric_limits<coordinate_type>::max();
        std::pair<coordinate_type, coordinate_type> expanded;

        for(std::size_t i=0; i<origins.size(); ++i)
        {
            for(std::size_t j=0; j<entries.size(); ++j)
            {
                const std::pair<coordinate_type, coordinate_type> tmpe =
                    merge_range(origins[i], entries[j])
                const coordinate_type tmpr = calc_range(tmpe);
                if(tmpr < range)
                {
                    range    = tmpr;
                    expanded = tmpe;
                }
            }
        }

        if(range > period)
        {
            access_min::set(box, access_min::get(bd));
            access_max::set(box, access_max::get(bd));
        }
        else
        {
            access_min::set(box, adjust_position(expanded.first,  bd));
            access_max::set(box, adjust_position(expanded.second, bd));
        }
        return expand_impl<box_type, boundary_type, I-1>::invoke(box, entry, bd);
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
