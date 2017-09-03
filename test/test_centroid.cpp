#define BOOST_TEST_MODULE "test_centroid"

#ifdef UNITTEST_FRAMEWORK_LIBRARY_EXIST
#include <boost/test/unit_test.hpp>
#else
#define BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>
#endif

#include <periortree/boundary_condition.hpp>
#include <periortree/centroid.hpp>
#include <test/point_type.hpp>
#include <test/aabb_type.hpp>

BOOST_AUTO_TEST_CASE(test_centroid_unlimited)
{
    {
        const perior::test::xyz  l(0., 0., 0.);
        const perior::test::xyz  u(10., 10., 10.);
        const perior::test::aabb box(l, u);
        const perior::unlimited_boundary<perior::test::xyz> boundary;

        const perior::test::xyz c = perior::centroid(box, boundary);
        BOOST_CHECK_CLOSE_FRACTION(c.x, 5.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(c.y, 5.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(c.z, 5.0, 1e-12);
    }
}

BOOST_AUTO_TEST_CASE(test_centroid_cubic_periodic)
{
    const perior::test::xyz lw(0., 0., 0.);
    const perior::test::xyz up(10., 10., 10.);
    const perior::cubic_periodic_boundary<perior::test::xyz> boundary(lw, up);

    {
        const perior::test::xyz  l(3.0, 3.0, 3.0);
        const perior::test::xyz  u(7.0, 7.0, 7.0);
        const perior::test::aabb box(l, u);

        const perior::test::xyz c = perior::centroid(box, boundary);
        BOOST_CHECK_CLOSE_FRACTION(c.x, 5.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(c.y, 5.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(c.z, 5.0, 1e-12);
    }

    {
        const perior::test::xyz  l(7.0, 7.0, 7.0);
        const perior::test::xyz  u(3.0, 3.0, 3.0);
        const perior::test::aabb box(l, u);
        const perior::test::xyz c = perior::centroid(box, boundary);
        BOOST_CHECK_CLOSE_FRACTION(c.x, 10.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(c.y, 10.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(c.z, 10.0, 1e-12);
    }

    {
        const perior::test::xyz l(8.0, 4.0, 8.0);
        const perior::test::xyz u(2.0, 8.0, 2.0);
        const perior::test::aabb box(l, u);
        const perior::test::xyz c = perior::centroid(box, boundary);
        BOOST_CHECK_CLOSE_FRACTION(c.x, 10.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(c.y,  6.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(c.z, 10.0, 1e-12);
    }
}
