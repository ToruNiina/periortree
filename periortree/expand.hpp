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

    const pointT l1 = lhs.centroid - lhs.width / 2;
    const pointT u1 = lhs.centroid + lhs.width / 2;
    const pointT l2 = rhs.centroid - rhs.width / 2;
    const pointT u2 = rhs.centroid + rhs.width / 2;

    pointT center, width;
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        const scalar_type l = std::min(l1[i], l2[i]);
        const scalar_type u = std::max(u1[i], u2[i]);
        center[i] = (u + l) / 2;
         width[i] =  u - l;
    }
    return rectangle<pointT>(restrict_position(center, b), width);
}


template<typename pointT>
inline typename boost::enable_if<traits::is_point<pointT>,
       rectangle<pointT> >::type
expand(const rectangle<pointT>& lhs, const rectangle<pointT>& rhs,
       const cubic_periodic_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    typedef typename traits::scalar_type_of<pointT>::type scalar_type;

    const pointT dc(restrict_direction(rhs.centroid - lhs.centroid, b));
    const pointT l1 = lhs.centroid - lhs.width / 2;
    const pointT u1 = lhs.centroid + lhs.width / 2;
    const pointT l2 = lhs.centroid + dc - rhs.width / 2;
    const pointT u2 = lhs.centroid + dc + rhs.width / 2;

    pointT center, width;
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        const scalar_type l = std::min(l1[i], l2[i]);
        const scalar_type u = std::max(u1[i], u2[i]);
        center[i] = (u + l) / 2;
         width[i] =  u - l;
    }
    return rectangle<pointT>(restrict_position(center, b), width);
}

template<typename pointT>
inline typename boost::enable_if<traits::is_point<pointT>,
       rectangle<pointT> >::type
expand(const rectangle<pointT>& rct, const pointT& p,
       const unlimited_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    typedef typename traits::scalar_type_of<pointT>::type scalar_type;
    const pointT lower = rct.centroid - rct.width / 2;
    const pointT upper = rct.centroid + rct.width / 2;

    pointT center, width;
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        const scalar_type l = std::min(lower[i], p[i]);
        const scalar_type u = std::max(upper[i], p[i]);
        center[i] = (u + l) / 2;
         width[i] =  u - l;
    }
    return rectangle<pointT>(restrict_position(center, b), width);
}

template<typename pointT>
inline typename boost::enable_if<traits::is_point<pointT>,
       rectangle<pointT> >::type
expand(const rectangle<pointT>& rct, const pointT& p,
       const cubic_periodic_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    typedef typename traits::scalar_type_of<pointT>::type scalar_type;

    const pointT dc(restrict_direction(p - rct.centroid, b));
    const pointT lower = rct.centroid - rct.width / 2;
    const pointT upper = rct.centroid + rct.width / 2;
    const pointT p_ = rct.centroid + dc;

    pointT center, width;
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        const scalar_type l = std::min(lower[i], p_[i]);
        const scalar_type u = std::max(upper[i], p_[i]);
        center[i] = (u + l) / 2;
         width[i] =  u - l;
    }
    return rectangle<pointT>(restrict_position(center, b), width);
}

} // perior
#endif//PERIOR_TREE_EXPAND
