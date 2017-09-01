#ifndef TEST_PERIOR_TREE_POINT_TRAITS_HPP
#define TEST_PERIOR_TREE_POINT_TRAITS_HPP
#include <periortree/point_traits.hpp>
#include <boost/array.hpp>
#include <boost/mpl/int.hpp>

namespace perior
{
namespace test
{

struct xyz
{
    xyz(const double x_, const double y_, const double z_)
        : x(x_), y(y_), z(z_)
    {}

    xyz() : x(0), y(0), z(0) {}
    ~xyz(){}

    xyz(const xyz& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
    xyz& operator=(const xyz& rhs)
    {
        x = rhs.x; y = rhs.y; z = rhs.z;
        return *this;
    }

    double x, y, z;
};

struct position
{
    position(const double x_, const double y_, const double z_)
    {
        v[0] = x_; v[1] = y_; v[2] = z_;
    }

    position(){v.fill(0.0);}
    ~position(){}

    position(const position& rhs) : v(rhs.v) {}
    position& operator=(const position& rhs) {v = rhs.v; return *this;}

    boost::array<double, 3> v;
};

}// test
}// perior

namespace perior
{
namespace traits
{
template<> struct tag<perior::test::xyz> {typedef point_tag type;};
template<> struct dimension_of<perior::test::xyz>: boost::mpl::int_<3>{};
template<> struct coordinate_type_of<perior::test::xyz>{typedef double type;};
template<> struct point_access<perior::test::xyz, 0>
{
    typedef perior::test::xyz point_type;
    typedef coordinate_type_of<point_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type
    get(const point_type& p) BOOST_NOEXCEPT_OR_NOTHROW
    {return p.x;}

    BOOST_FORCEINLINE
    static void
    set(point_type& p, coordinate_type x) BOOST_NOEXCEPT_OR_NOTHROW
    {p.x = x;}
};
template<> struct point_access<perior::test::xyz, 1>
{
    typedef perior::test::xyz point_type;
    typedef coordinate_type_of<point_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type
    get(const point_type& p) BOOST_NOEXCEPT_OR_NOTHROW
    {return p.y;}

    BOOST_FORCEINLINE
    static void
    set(point_type& p, coordinate_type y) BOOST_NOEXCEPT_OR_NOTHROW
    {p.y = y;}
};
template<> struct point_access<perior::test::xyz, 2>
{
    typedef perior::test::xyz point_type;
    typedef coordinate_type_of<point_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type
    get(const point_type& p) BOOST_NOEXCEPT_OR_NOTHROW
    {return p.z;}

    BOOST_FORCEINLINE
    static void
    set(point_type& p, coordinate_type z) BOOST_NOEXCEPT_OR_NOTHROW
    {p.z = z;}
};

// traits for perior::test::position -------------------------------------------

template<> struct tag<perior::test::position> {typedef point_tag type;};
template<> struct dimension_of<perior::test::position>: boost::mpl::int_<3>{};
template<> struct coordinate_type_of<perior::test::position>{typedef double type;};
template<std::size_t I>
struct point_access<perior::test::position, I>
{
    typedef perior::test::position point_type;
    typedef coordinate_type_of<point_type>::type coordinate_type;

    BOOST_STATIC_ASSERT(I < dimension_of<point_type>::value);

    BOOST_FORCEINLINE
    static coordinate_type
    get(const point_type& p) BOOST_NOEXCEPT_OR_NOTHROW
    {return p.v[I];}

    BOOST_FORCEINLINE
    static void
    set(point_type& p, coordinate_type x) BOOST_NOEXCEPT_OR_NOTHROW
    {p.v[I] = x;}
};

}// traits
}// perior

#endif//TEST_PERIOR_TREE_POINT_TRAITS_HPP
