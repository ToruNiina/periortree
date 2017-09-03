#define BOOST_TEST_MODULE "test_boundary"

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

// BOOST_AUTO_TEST_CASE(test_xyz_cubic_periodic)
// {
//     const perior::test::xyz lw(0, 0, 0);
//     const perior::test::xyz up(1, 1, 1);
//     const perior::cubic_periodic_boundary<perior::test::xyz> boundary(lw, up);
//     {
//         const perior::test::xyz p(0.3, 0.3, 0.3);
//
//         const perior::test::xyz ap = adjust_position(p, boundary);
//         BOOST_CHECK_EQUAL(ap.x, p.x);
//         BOOST_CHECK_EQUAL(ap.y, p.y);
//         BOOST_CHECK_EQUAL(ap.z, p.z);
//         const perior::test::xyz ad = adjust_direction(p, boundary);
//         BOOST_CHECK_EQUAL(ad.x, p.x);
//         BOOST_CHECK_EQUAL(ad.y, p.y);
//         BOOST_CHECK_EQUAL(ad.z, p.z);
//     }
//
//     {
//         const perior::test::xyz p(1.3, 0.3, 0.3);
//
//         const perior::test::xyz ap = adjust_position(p, boundary);
//         BOOST_CHECK_CLOSE(ap.x, 1.3 - 1.0, 1e-10);
//         BOOST_CHECK_EQUAL(ap.y, p.y);
//         BOOST_CHECK_EQUAL(ap.z, p.z);
//     }
//     {
//         const perior::test::xyz p(0.6, 0.3, 0.3);
//         const perior::test::xyz ad = adjust_direction(p, boundary);
//         BOOST_CHECK_CLOSE(ad.x, 0.6 - 1.0, 1e-10);
//         BOOST_CHECK_EQUAL(ad.y, p.y);
//         BOOST_CHECK_EQUAL(ad.z, p.z);
//     }
// }
