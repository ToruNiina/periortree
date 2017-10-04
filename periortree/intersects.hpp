#ifndef PERIOR_TREE_INTERSECTS
#define PERIOR_TREE_INTERSECTS
#include <periortree/boundary_conditions.hpp>
#include <periortree/rectangle.hpp>
#include <periortree/point_traits.hpp>
#include <cmath>

namespace perior
{

template<typename pointT, template<typename> class boundaryT>
inline typename boost::enable_if<traits::is_point<pointT>, bool>::type
intersects(const rectangle<pointT>& lhs, const rectangle<pointT>& rhs,
           const boundaryT<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    const pointT dc(restrict_direction(lhs.center - rhs.center, b));
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        if(std::abs(dc[i]) > lhs.radius[i] + rhs.radius[i])
        {
            return false;
        }
    }
    return true;
}

template<typename pointT, template<typename> class boundaryT>
inline typename boost::enable_if<traits::is_point<pointT>, bool>::type
intersects(const pointT& p, const rectangle<pointT>& rect,
           const boundaryT<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    const pointT dc(restrict_direction(p - rect.center, b));
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        if(std::abs(dc[i]) > rect.radius[i])
        {
            return false;
        }
    }
    return true;
}

} // perior
#endif//PERIOR_TREE_INTERSECTS
