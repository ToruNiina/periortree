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

    explicit rectangle(const point_type& r)
        : center(traits::zero_vector<point_type>()), radius(r)
    {}

    rectangle(const point_type& c, const point_type& r)
        : center(c), radius(r)
    {}

    rectangle(const rectangle& rhs)
        : center(rhs.center), radius(rhs.radius)
    {}
    rectangle& operator=(const rectangle& rhs)
    {
        center = rhs.center;
        radius = rhs.radius;
        return *this;
    }

    point_type center, radius;
};

template<typename pointT>
inline bool operator==(const rectangle<pointT>& lhs, const rectangle<pointT>& rhs)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    return lhs.center == rhs.center && lhs.radius == rhs.radius;
}

template<typename pointT>
inline bool operator!=(const rectangle<pointT>& lhs, const rectangle<pointT>& rhs)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    return lhs.center != rhs.center || lhs.radius != rhs.radius;
}

template<typename pointT, typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const rectangle<pointT>& rect)
{
    os << "rectangle {c = " << rect.center << ", r = " << rect.radius << '}';
    return os;
}

} // perior
#endif// PERIOR_TREE_RECTANGLE
