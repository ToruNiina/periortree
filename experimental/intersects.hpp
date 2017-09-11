#ifndef PERIOR_TREE_INTERSECTS_HPP
#define PERIOR_TREE_INTERSECTS_HPP
#include "point.hpp"
#include "aabb.hpp"
#include "boundary_condition.hpp"

namespace perior
{

//! returns whether point intersects box.
template<typename T, std::size_t N>
inline bool
intersects(const point<T, N>& p, const aabb<T, N>& box,
           const unlimited_boundary<T, N>& b) noexcept
{
    for(std::size_t i=0; i<N; ++i)
    {
        if(p[i] < box.lower()[i] || box.upper()[i] < p[i])
        {
            return false;
        }
    }
    return true;
}

template<typename T, std::size_t N>
inline bool
intersects(const point<T, N>& p, const aabb<T, N>& box,
           const cubic_periodic_boundary<T, N>& b) noexcept
{
    for(std::size_t i=0; i<N; ++i)
    {
        if(box.lower()[i] <= box.upper()[i])
        {
            //: |-----|  : box
            //:        x : point
            if(p[i] < box.lower()[i] || box.upper()[i] < p[i])
            {
                return false;
            }
        }
        else
        {
            //:--|  |----: box
            //:    x     : point
            if(box.upper()[i] < p[i] && p[i] < box.lower()[i])
            {
                return false;
            }
        }
    }
    return true;
}

//! returns whether 1st-arg intersects 2nd-arg.
template<typename T, std::size_t N>
inline bool
intersects(const aabb<T, N>& lhs, const aabb<T, N>& rhs,
           const unlimited_boundary<T, N>& b) noexcept
{
    for(std::size_t i=0; i<N; ++i)
    {
        // :  |------| : lhs
        // : |---|     : rhs
        if(lhs.upper()[i] < rhs.lower()[i] || rhs.upper()[i] < lhs.lower()[i])
        {
            return false;
        }
    }
    return true;
}

template<typename T, std::size_t N>
inline bool
intersects(const aabb<T, N>& lhs, const aabb<T, N>& rhs,
           const cubic_periodic_boundary<T, N>& b) noexcept
{
    for(std::size_t i=0; i<N; ++i)
    {
        const T l1 = lhs.lower()[i];
        const T u1 = lhs.upper()[i];
        const T l2 = rhs.lower()[i];
        const T u2 = rhs.upper()[i];

        if(l1 < u1 && l2 < u2)
        {
            // :   |-----| : lhs [l1, u1]
            // : |----|    : rhs [l2, u2]
            if(u1 < l2 || u2 < l1){return false;}
        }
        else if(l1 < u1 && l2 > u2)
        {
            // :   |---|   : lhs [l1, u1]
            // :-----|   |-: rhs [l2, u2]
            if(u2 < l1 && u1 < l2){return false}
        }
        else if(l1 > u1 && l2 <= u2)
        {
            // :--|      |-: lhs
            // : |------|  : rhs
            if(u1 < l2 && u2 < l1){return false}
        }
        else
        {
            // :-|      |--: lhs
            // :--|   |----: rhs
            // always intersects at the boundary!
        }
    }
    return true;
}

} // perior
#endif//PERIOR_TREE_CENTROID_HPP
