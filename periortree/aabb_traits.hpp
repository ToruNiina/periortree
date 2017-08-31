#ifndef PERIOR_TREE_AABB_TRAITS_HPP
#define PERIOR_TREE_AABB_TRAITS_HPP
#include <cstddef> // std::size_t

namespace perior
{
namespace traits
{

struct aabb_tag{};

template<typename T> struct tag{};
template<typename T> struct point_type_of{};

struct max_corner{};
struct min_corner{};

template<typename T, typename Corner, std::size_t D>
struct box_access{};

}// traits
}// perior
#endif//PERIOR_TREE_AABB_TRAITS_HPP
