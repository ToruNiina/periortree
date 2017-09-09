#ifndef PERIOR_TREE_EXPAND_HPP
#define PERIOR_TREE_EXPAND_HPP
#include "point.hpp"
#include "aabb.hpp"
#include "boundary_condition.hpp"
#include "adjust_box.hpp"
#include "centroid.hpp"
#include "intersects.hpp"
#include <algorithm>

namespace perior
{

// if expanded, return true.
template<typename T, std::size_t N>
inline bool
expand(aabb<T, N>& box, const point<T, N>& entry,
       const unlimited_boundary<T, N>& b) noexcept
{
    if(intersects(entry, box, b)) {return false;}
    for(std::size_t i=0; i<N; ++i)
    {
        box.upper()[i] = std::max(box.upper()[i], entry[i]);
        box.lower()[i] = std::min(box.lower()[i], entry[i]);
    }
    return true;
}

template<typename T, std::size_t N>
inline bool
expand(aabb<T, N>& box, const aabb<T, N>& entry,
       const unlimited_boundary<T, N>& b) noexcept
{
    if(intersects(entry, box, b)) {return false;}
    for(std::size_t i=0; i<N; ++i)
    {
        box.upper()[i] = std::max(box.upper()[i], entry.upper()[i]);
        box.lower()[i] = std::min(box.lower()[i], entry.lower()[i]);
    }
    return true;
}
// done

template<typename T, std::size_t N>
inline bool
expand(aabb<T, N>& box, const point<T, N>& entry,
       const cubic_periodic_boundary<T, N>& b) noexcept
{
    if(intersects(entry, box, b)) {return false;}
    const point<T, N> c = centroid(box, b);
    const point<T, N> d = adjust_direction(entry - c, b);
    const point<T, N> e = c + d;

    for(std::size_t i=0; i<N; ++i)
    {
        box.upper()[i] = std::max(box.upper()[i], e[i]);
        box.lower()[i] = std::min(box.lower()[i], e[i]);
    }
    box.upper() = adjust_position(box.upper(), b);
    box.lower() = adjust_position(box.lower(), b);
    return true;
}

template<typename T, std::size_t N>
inline bool
expand(aabb<T, N>& box, const aabb<T, N>& entry,
       const cubic_periodic_boundary<T, N>& b) noexcept
{
    const point<T, N> sb = span(box.lower(),   box.upper(),   b);
    const point<T, N> se = span(entry.lower(), entry.upper(), b);
    const point<T, N> cb = centroid(box, b);
    const point<T, N> ce = cb + adjust_direction(centroid(entry) - cb, b);

    // reconstruct aabbs at their nearest relative position without boundary
    const point<T, N> ub = cb + sb * 0.5;
    const point<T, N> lb = cb - sb * 0.5;
    const point<T, N> ue = ce + se * 0.5;
    const point<T, N> le = ce - se * 0.5;

    for(std::size_t i=0; i<N; ++i)
    {
        box.upper()[i] = std::max(ub[i], ue[i]);
        box.lower()[i] = std::min(lb[i], le[i]);
    }
    box = adjust_box(box, b);
    return true;
}

}// perior
#endif// PERIOR_TREE_EXPAND_HPP
