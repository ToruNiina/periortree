#define BOOST_TEST_MODULE "test_area"

#ifdef UNITTEST_FRAMEWORK_LIBRARY_EXIST
#include <boost/test/unit_test.hpp>
#else
#define BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>
#endif

#include <periortree/boundary_condition.hpp>
#include <periortree/area.hpp>
#include <test/point_type.hpp>
#include <test/aabb_type.hpp>

BOOST_AUTO_TEST_CASE(test_aabb_unlimited)
{
    {
        const perior::test::xyz  l(0., 0., 0.);
        const perior::test::xyz  u(10., 10., 10.);
        const perior::test::aabb box(l, u);
        const perior::unlimited_boundary<perior::test::xyz> boundary;

        const double area = perior::area(box, boundary);
        BOOST_CHECK_CLOSE_FRACTION(area, 1000.0, 1e-12);
    }
}

BOOST_AUTO_TEST_CASE(test_xyz_cubic_periodic)
{
    const perior::test::xyz lw(0., 0., 0.);
    const perior::test::xyz up(10., 10., 10.);
    const perior::cubic_periodic_boundary<perior::test::xyz> boundary(lw, up);

    {
        const perior::test::xyz l(3.0, 3.0, 3.0);
        const perior::test::xyz u(7.0, 7.0, 7.0);
        const perior::test::aabb box(l, u);
        const double area = perior::area(box, boundary);
        BOOST_CHECK_CLOSE_FRACTION(area, 64.0, 1e-12);
    }

    {
        const perior::test::xyz l(8.0, 8.0, 8.0);
        const perior::test::xyz u(2.0, 2.0, 2.0);
        const perior::test::aabb box(l, u);
        const double area = perior::area(box, boundary);
        BOOST_CHECK_CLOSE_FRACTION(area, 64.0, 1e-12);
    }

    {
        const perior::test::xyz l(8.0, 4.0, 8.0);
        const perior::test::xyz u(2.0, 8.0, 2.0);
        const perior::test::aabb box(l, u);
        const double area = perior::area(box, boundary);
        BOOST_CHECK_CLOSE_FRACTION(area, 64.0, 1e-12);
    }
}
