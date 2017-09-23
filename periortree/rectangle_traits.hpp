#ifndef PERIOR_TREE_RECTANGLE_TRAITS
#define PERIOR_TREE_RECTANGLE_TRAITS
#include <periortree/point_traits.hpp>
#include <periortree/rectangle.hpp>

namespace perior
{
namespace traits
{

template<typename T>
struct point_type_of{};
template<typename pointT>
struct point_type_of<rectangle<pointT> >{typedef pointT type;};

template<typename T>
struct scalar_type_of{};
template<typename pointT>
struct scalar_type_of<rectangle<pointT> >
{
    typedef typename scalar_type_of<pointT>::type type;
};

template<typename T>
struct dimension<rectangle<T> >
: boost::integral_constants<std::size_t, dimension<T>::vlaue>{};


} // traits
} // perior
#endif//PERIOR_TREE_RECTANGLE_TRAITS
