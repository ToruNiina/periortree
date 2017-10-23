#ifndef PERIOR_TREE_QUERY_H
#define PERIOR_TREE_QUERY_H
#include <periortree/point_traits.hpp>
#include <periortree/boundary_conditions.hpp>
#include <periortree/rectangle.hpp>
#include <periortree/intersects.hpp>
#include <boost/static_assert.hpp>

namespace perior
{
namespace query
{

template<typename pointT>
struct query_intersects_box
{
    BOOST_STATIC_ASSERT(::perior::traits::is_point<pointT>::value);

    explicit query_intersects_box(const rectangle<pointT>& rct): rect(rct){}

    template<typename boundaryT>
    BOOST_FORCEINLINE
    bool match(const pointT& p, boundaryT const& b) const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return ::perior::intersects(p, rect, b);
    }

    template<typename boundaryT>
    BOOST_FORCEINLINE
    bool match(const rectangle<pointT>& r, boundaryT const& b)
        const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return ::perior::intersects(r, rect, b);
    }

    // for objects
    template<typename T>
    BOOST_FORCEINLINE
    bool match(T const& r) const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return true;
    }

    BOOST_FORCEINLINE
    rectangle<pointT> const& box() const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return rect;
    }

    rectangle<pointT> rect;
};

template<typename pointT>
struct query_within_box
{
    BOOST_STATIC_ASSERT(::perior::traits::is_point<pointT>::value);

    explicit query_within_box(const rectangle<pointT>& rct): rect(rct){}

    template<typename boundaryT>
    BOOST_FORCEINLINE
    bool match(const pointT& p, boundaryT const& b) const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return ::perior::within(p, rect, b);
    }

    template<typename boundaryT>
    BOOST_FORCEINLINE
    bool match(const rectangle<pointT>& r, boundaryT const& b)
        const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return ::perior::within(r, rect, b);
    }

    template<typename T>
    BOOST_FORCEINLINE
    bool match(T const& r) const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return true;
    }

    BOOST_FORCEINLINE
    rectangle<pointT> const& box() const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return rect;
    }

    rectangle<pointT> rect;
};

template<typename pointT>
inline typename boost::enable_if<
    ::perior::traits::is_point<pointT>, query_intersects_box<pointT> >::type
intersects_box(rectangle<pointT> const& rect)
{
    return query_intersects_box<pointT>(rect);
}

template<typename pointT>
inline typename boost::enable_if<
    ::perior::traits::is_point<pointT>, query_intersects_box<pointT> >::type
within_box(rectangle<pointT> const& rect)
{
    return query_within_box<pointT>(rect);
}

} // query
} // perior
#endif//PERIOR_TREE_QUERY_H
