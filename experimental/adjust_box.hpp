#ifndef PERIOR_TREE_ADJUST_BOX_HPP
#define PERIOR_TREE_ADJUST_BOX_HPP
#include "point.hpp"
#include "aabb.hpp"
#include "boundary_condition.hpp"

namespace perior
{

template<typename T, std::size_t N>
inline aabb<T, N>
adjust_box(aabb<T, N> x, const unlimited_boundary<T, N>& b) noexcept
{
    return x;
}

template<typename T, std::size_t N>
inline aabb<T, N>
adjust_box(aabb<T, N> x, const cubic_periodic_boundary<T, N>& b) noexcept
{
    for(std::size_t i=0; i<N; ++i)
    {
        if(x.upper()[i] - x.lower()[i] >= b.upper()[i] - b.lower()[i])
        {
            // both lower and upper exceeds boundary.
            // in this dimension, box size is equal to that of boundary.
            x.lower()[i] = b.lower()[i];
            x.upper()[i] = b.upper()[i];
        }
        else if(x.lower()[i] < b.lower()[i])
        {
            x.lower()[i] += b.range()[i];
        }
        else if(b.upper()[i] < x.upper()[i])
        {
            x.upper()[i] -= b.range()[i];
        }
    }
    return x;
}

}// perior
#endif//PERIOR_TREE_ADJUST_BOX_HPP
