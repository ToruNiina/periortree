#ifndef TEST_PERIOR_TREE_AABB_TYPE_HPP
#define TEST_PERIOR_TREE_AABB_TYPE_HPP
#include <periortree/aabb_traits.hpp>
#include <test/point_type.hpp>
#include <boost/array.hpp>
#include <boost/mpl/int.hpp>

namespace perior
{
namespace test
{

struct aabb
{
    aabb(const xyz& lw, const xyz& up)
      : lower(lw), upper(up)
    {}

    aabb() {}
    ~aabb(){}

    aabb(const aabb& rhs) : lower(rhs.lower), upper(rhs.upper){}
    aabb& operator=(const aabb& rhs)
    {
        upper = rhs.upper;
        lower = rhs.lower;
        return *this;
    }

    xyz lower, upper;
};

}// test
}// perior

namespace perior
{
namespace traits
{
template<> struct tag<perior::test::aabb> {typedef aabb_tag type;};
template<> struct dimension_of<perior::test::aabb>: boost::mpl::int_<3>{};
template<> struct point_type_of<perior::test::aabb>{typedef perior::test::xyz type;};
template<> struct coordinate_type_of<perior::test::aabb>{typedef double type;};

template<std::size_t D>
struct box_access<perior::test::aabb, max_corner, D>
{
    typedef perior::test::aabb box_type;
    typedef typename point_type_of<box_type>::type point_type;
    typedef typename coordinate_type_of<box_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type get(const box_type& b)
        BOOST_NOEXCEPT_IF(
            noexcept(point_access<point_type, D>::get(b.upper))
        )
    {
        return point_access<point_type, D>::get(b.upper);
    }

    BOOST_FORCEINLINE
    static void set(box_type& b, coordinate_type x)
        BOOST_NOEXCEPT_IF(
            noexcept(point_access<point_type, D>::set(b.upper, x))
        )
    {
        return point_access<point_type, D>::set(b.upper, x);
    }
};

template<std::size_t D>
struct box_access<perior::test::aabb, min_corner, D>
{
    typedef perior::test::aabb box_type;
    typedef typename point_type_of<box_type>::type point_type;
    typedef typename coordinate_type_of<box_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type get(const box_type& b)
        BOOST_NOEXCEPT_IF(
            noexcept(point_access<point_type, D>::get(b.lower))
        )
    {
        return point_access<point_type, D>::get(b.lower);
    }

    BOOST_FORCEINLINE
    static void set(box_type& b, coordinate_type x)
        BOOST_NOEXCEPT_IF(
            noexcept(point_access<point_type, D>::set(b.lower, x))
        )
    {
        return point_access<point_type, D>::set(b.lower, x);
    }
};

template<std::size_t D>
struct box_range_access<perior::test::aabb, D>
{
    typedef perior::test::aabb box_type;
    typedef typename point_type_of<box_type>::type point_type;
    typedef typename coordinate_type_of<box_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type get(const box_type& b)
        BOOST_NOEXCEPT_IF(
            noexcept(point_access<point_type, D>::get(b.lower))
        )
    {
        return point_access<point_type, D>::get(b.upper) -
               point_access<point_type, D>::get(b.lower);
    }
};

}// traits
}// perior
#endif//TEST_PERIOR_TREE_AABB_TYPE_HPP
