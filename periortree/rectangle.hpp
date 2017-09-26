#ifndef PERIOR_TREE_RECTANGLE
#define PERIOR_TREE_RECTANGLE
#include <periortree/point_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/config.hpp>

namespace perior
{

template<typename pointT>
struct rectangle
{
    typedef pointT point_type;
    const static std::size_t dimension = traits::dimension<point_type>::value;
    BOOST_STATIC_ASSERT(traits::is_point<point_type>::value);

    rectangle(){}

    explicit rectangle(const point_type& w)
        : centroid(traits::zero_vector<point_type>()), width(w)
    {}

    rectangle(const point_type& c, const point_type& w)
        : centroid(c), width(w)
    {}

    rectangle(const rectangle& rhs)
        : centroid(rhs.centroid), width(rhs.width)
    {}
    rectangle& operator=(const rectangle& rhs)
    {
        centroid = rhs.centroid;
        width    = rhs.width;
        return *this;
    }

    point_type centroid, width;
};

template<typename pointT>
inline bool operator==(const rectangle<pointT>& lhs, const rectangle<pointT>& rhs)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    return lhs.centroid == rhs.centroid && lhs.width == rhs.width;
}

template<typename pointT>
inline bool operator!=(const rectangle<pointT>& lhs, const rectangle<pointT>& rhs)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    return lhs.centroid != rhs.centroid || lhs.width != rhs.width;
}

} // perior
#endif// PERIOR_TREE_RECTANGLE
