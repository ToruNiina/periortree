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

} // perior
#endif// PERIOR_TREE_RECTANGLE
