#ifndef PERIOR_TREE_AABB_TRAITS_HPP
#define PERIOR_TREE_AABB_TRAITS_HPP
#include <periortree/point_traits.hpp>

namespace perior
{
namespace traits
{

struct aabb_tag{};

template<typename T> struct point_type_of{};

struct max_corner{};
struct min_corner{};

template<typename T, typename Corner, std::size_t D>
struct box_access{};
template<typename T, std::size_t D>
struct box_range_access{};

}// traits
}// perior
#endif//PERIOR_TREE_AABB_TRAITS_HPP
