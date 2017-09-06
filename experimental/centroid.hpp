#ifndef PERIOR_TREE_CENTROID_HPP
#define PERIOR_TREE_CENTROID_HPP
#include "point.hpp"
#include "aabb.hpp"
#include "boundary_condition.hpp"

namespace perior
{

template<typename T, std::size_t N>
inline point<T, N>
centroid(const aabb<T, N>& box, const unlimited_boundary<T, N>& b) noexcept
{
    return (box.upper() + box.lower()) * 0.5;
}

template<typename T, std::size_t N>
inline point<T, N>
centroid(const aabb<T, N>& box, const cubic_periodic_boundary<T, N>& b) noexcept
{
    const point<T, N> c = box.lower() + span(box.lower(), box.upper(), b) * 0.5;
    return adjust_position(c, b);
}

} // perior
#endif//PERIOR_TREE_CENTROID_HPP
