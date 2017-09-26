#ifndef PERIOR_TREE_WITHIN
#define PERIOR_TREE_WITHIN
#include <periortree/boundary_conditions.hpp>
#include <periortree/rectangle.hpp>
#include <cmath>

namespace perior
{

template<typename pointT, template<typename> class boundaryT>
inline typename boost::enable_if<traits::is_point<pointT>, bool>::type
within(const rectangle<pointT>& inner, const rectangle<pointT>& outer,
       const boundaryT<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    const pointT dc(restrict_direction(outer.centroid - inner.centroid, b));
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        if(std::abs(dc[i]) > (outer.width[i] - inner.width[i]) / 2)
        {
            return false;
        }
    }
    return true;
}

template<typename pointT, template<typename> class boundaryT>
inline typename boost::enable_if<traits::is_point<pointT>, bool>::type
within(const pointT& p, const rectangle<pointT>& r, const boundaryT<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    const pointT dc(restrict_direction(p - r.centroid, b));
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        if(std::abs(dc[i]) > r.width[i] / 2)
        {
            return false;
        }
    }
    return true;
}

} // perior
#endif// PERIOR_TREE_BOUNDARY_CONDITIONS
