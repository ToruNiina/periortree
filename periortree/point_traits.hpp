#ifndef PERIOR_TREE_POINT_TRAITS_HPP
#define PERIOR_TREE_POINT_TRAITS_HPP
#include <cstddef> // std::size_t

namespace perior
{
namespace traits
{

struct point_tag{};

template<typename T> struct tag{};
template<typename T> struct dimension_of{};
template<typename T> struct coordinate_type_of{};
template<typename T, std::size_t N> struct point_access{};

}// traits
}// perior
#endif//PERIOR_TREE_POINT_TRAITS_HPP
