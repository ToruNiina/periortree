#ifndef PERIOR_TREE_AREA_HPP
#define PERIOR_TREE_AREA_HPP
#include "point.hpp"
#include "aabb.hpp"
#include "boundary_condition.hpp"

namespace perior
{

template<typename T, std::size_t N, typename boundaryT>
inline T area(const aabb<T, N>& box, const boundaryT& b) noexcept
{
    const point<T, N> spn = span(box.lower(), box.upper(), b);
    T retval = 1;
    for(std::size_t i=0; i<N; ++i)
    {
        retval *= spn[i];
    }
    return retval;
}

}// perior
#endif//PERIOR_TREE_AREA_HPP
