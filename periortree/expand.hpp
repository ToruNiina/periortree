#ifndef PERIOR_TREE_EXPAND
#define PERIOR_TREE_EXPAND
#include <periortree/boundary_conditions.hpp>
#include <periortree/rectangle.hpp>
#include <cmath>

namespace perior
{

template<typename pointT>
inline typename boost::enable_if<traits::is_point<pointT>,
       rectangle<pointT> >::type
expand(const rectangle<pointT>& lhs, const rectangle<pointT>& rhs,
       const unlimited_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    typedef typename traits::scalar_type_of<pointT>::type scalar_type;

    const pointT l1 = lhs.center - lhs.radius;
    const pointT u1 = lhs.center + lhs.radius;
    const pointT l2 = rhs.center - rhs.radius;
    const pointT u2 = rhs.center + rhs.radius;

    pointT center, radius;
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        const scalar_type l = std::min(l1[i], l2[i]);
        const scalar_type u = std::max(u1[i], u2[i]);
        center[i] = (u + l) / 2;
        radius[i] = (u - l) / 2;
    }
    return rectangle<pointT>(restrict_position(center, b), radius);
}


template<typename pointT>
inline typename boost::enable_if<traits::is_point<pointT>,
       rectangle<pointT> >::type
expand(const rectangle<pointT>& lhs, const rectangle<pointT>& rhs,
       const cubic_periodic_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    typedef typename traits::scalar_type_of<pointT>::type scalar_type;

    const pointT dc(restrict_direction(rhs.center - lhs.center, b));
    const pointT l1 = lhs.center - lhs.radius;
    const pointT u1 = lhs.center + lhs.radius;
    const pointT l2 = lhs.center + dc - rhs.radius;
    const pointT u2 = lhs.center + dc + rhs.radius;

    pointT center, radius;
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        const scalar_type l = std::min(l1[i], l2[i]);
        const scalar_type u = std::max(u1[i], u2[i]);
        center[i] = (u + l) / 2;
        radius[i] = (u - l) / 2;
    }
    return rectangle<pointT>(restrict_position(center, b), radius);
}

template<typename pointT>
inline typename boost::enable_if<traits::is_point<pointT>,
       rectangle<pointT> >::type
expand(const rectangle<pointT>& rct, const pointT& p,
       const unlimited_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    typedef typename traits::scalar_type_of<pointT>::type scalar_type;
    const pointT lower = rct.center - rct.radius;
    const pointT upper = rct.center + rct.radius;

    pointT center, radius;
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        const scalar_type l = std::min(lower[i], p[i]);
        const scalar_type u = std::max(upper[i], p[i]);
        center[i] = (u + l) / 2;
        radius[i] = (u - l) / 2;
    }
    return rectangle<pointT>(restrict_position(center, b), radius);
}

template<typename pointT>
inline typename boost::enable_if<traits::is_point<pointT>,
       rectangle<pointT> >::type
expand(const rectangle<pointT>& rct, const pointT& p,
       const cubic_periodic_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    typedef typename traits::scalar_type_of<pointT>::type scalar_type;

    const pointT dc(restrict_direction(p - rct.center, b));
    const pointT lower = rct.center - rct.radius;
    const pointT upper = rct.center + rct.radius;
    const pointT p_ = rct.center + dc;

    pointT center, radius;
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        const scalar_type l = std::min(lower[i], p_[i]);
        const scalar_type u = std::max(upper[i], p_[i]);
        center[i] = (u + l) / 2;
        radius[i] = (u - l) / 2;
    }
    return rectangle<pointT>(restrict_position(center, b), radius);
}

} // perior
#endif//PERIOR_TREE_EXPAND
