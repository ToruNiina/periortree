#ifndef PERIOR_TREE_WITHIN_HPP
#define PERIOR_TREE_WITHIN_HPP
#include "point.hpp"
#include "aabb.hpp"
#include "boundary_condition.hpp"

namespace perior
{

//! returns whether point is within box.
template<typename T, std::size_t N>
inline bool
within(const point<T, N>& p, const aabb<T, N>& box,
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
within(const point<T, N>& p, const aabb<T, N>& box,
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

//! returns whether 1st-arg is within 2nd-arg.
//  if the edges are in contact, return true.
template<typename T, std::size_t N>
inline bool
within(const aabb<T, N>& inside, const aabb<T, N>& covers,
       const unlimited_boundary<T, N>& b) noexcept
{
    for(std::size_t i=0; i<N; ++i)
    {
        // :   |---|   : inside
        // : |------|  : covers
        if(inside.lower()[i] < covers.lower()[i] ||
           covers.upper()[i] < inside.upper()[i])
        {
            return false;
        }
    }
    return true;
}

template<typename T, std::size_t N>
inline bool
within(const aabb<T, N>& inside, const aabb<T, N>& covers,
       const cubic_periodic_boundary<T, N>& b) noexcept
{
    for(std::size_t i=0; i<N; ++i)
    {
        const T l1 = inside.lower()[i];
        const T u1 = inside.upper()[i];
        const T l2 = covers.lower()[i];
        const T u2 = covers.upper()[i];

        if(l1 <= u1 && l2 <= u2)
        {
            // :   |---|   : inside
            // : |------|  : covers
            if(l1 < l2 || u2 < u1){return false;}
        }
        else if(l1 <= u1 && l2 > u2)
        {
            // :|---|      : inside
            // :-----|   |-: covers
            if(!(u1 < u2 || l2 < l1)){return false;}
        }
        else if(l1 > u1 && l2 <= u2)
        {
            // :--|      |-: inside
            // : |------|  : covers
            if(b.lower()[i] != l2 || b.upper()[i] != u2){return false;}
        }
        else
        {
            // :-|      |--: inside [l1, u1]
            // :--|   |----: covers [l2, u2]
            if(l1 < l2 || u2 < u1){return false;}
        }
    }
    return true;
}


} // perior
#endif//PERIOR_TREE_CENTROID_HPP
