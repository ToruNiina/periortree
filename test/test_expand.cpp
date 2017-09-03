#define BOOST_TEST_MODULE "test_expand"

#ifdef UNITTEST_FRAMEWORK_LIBRARY_EXIST
#include <boost/test/unit_test.hpp>
#else
#define BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>
#endif

#include <periortree/boundary_condition.hpp>
#include <periortree/expand.hpp>
#include <test/point_type.hpp>
#include <test/aabb_type.hpp>

BOOST_AUTO_TEST_CASE(test_centroid_unlimited)
{
    {
        const perior::unlimited_boundary<perior::test::xyz> boundary;

        const perior::test::xyz  l1(0., 0., 0.);
        const perior::test::xyz  u1(5., 5., 5.);
        const perior::test::aabb box1(l1, u1);

        const perior::test::xyz  l2(2., 2., 2.);
        const perior::test::xyz  u2(7., 7., 7.);
        const perior::test::aabb box2(l2, u2);

        perior::test::aabb ex(box1);
        perior::expand(ex, box2, boundary);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.x, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.y, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.z, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.x, 7.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.y, 7.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.z, 7.0, 1e-12);
    }

    {
        const perior::unlimited_boundary<perior::test::xyz> boundary;

        const perior::test::xyz  l1(0., 0., 0.);
        const perior::test::xyz  u1(5., 5., 5.);
        const perior::test::aabb box1(l1, u1);

        const perior::test::xyz  l2(-1., 2., -1.);
        const perior::test::xyz  u2( 7., 3.,  5.);
        const perior::test::aabb box2(l2, u2);

        perior::test::aabb ex(box1);
        perior::expand(ex, box2, boundary);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.x, -1.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.y,  0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.z, -1.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.x,  7.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.y,  5.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.z,  5.0, 1e-12);
    }
}

BOOST_AUTO_TEST_CASE(test_centroid_cubic_periodic)
{
    const perior::test::xyz lw(0., 0., 0.);
    const perior::test::xyz up(10., 10., 10.);
    const perior::cubic_periodic_boundary<perior::test::xyz> boundary(lw, up);
    /* inside the boundary */{
        const perior::test::xyz  l1(0., 0., 0.);
        const perior::test::xyz  u1(5., 5., 5.);
        const perior::test::aabb box1(l1, u1);

        const perior::test::xyz  l2(2., 2., 2.);
        const perior::test::xyz  u2(7., 7., 7.);
        const perior::test::aabb box2(l2, u2);

        perior::test::aabb ex(box1);
        perior::expand(ex, box2, boundary);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.x, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.y, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.z, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.x, 7.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.y, 7.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.z, 7.0, 1e-12);
    }
    /* box includes entry */{
        const perior::test::xyz  l1(0., 0., 0.);
        const perior::test::xyz  u1(5., 5., 5.);
        const perior::test::aabb box1(l1, u1);

        const perior::test::xyz  l2(2., 2., 2.);
        const perior::test::xyz  u2(3., 3., 3.);
        const perior::test::aabb box2(l2, u2);

        perior::test::aabb ex(box1);
        perior::expand(ex, box2, boundary);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.x, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.y, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.z, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.x, 5.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.y, 5.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.z, 5.0, 1e-12);
    }
    /* entry includes box */{
        const perior::test::xyz  l1(2., 2., 2.);
        const perior::test::xyz  u1(3., 3., 3.);
        const perior::test::aabb box1(l1, u1);

        const perior::test::xyz  l2(0., 0., 0.);
        const perior::test::xyz  u2(5., 5., 5.);
        const perior::test::aabb box2(l2, u2);

        perior::test::aabb ex(box1);
        perior::expand(ex, box2, boundary);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.x, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.y, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.z, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.x, 5.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.y, 5.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.z, 5.0, 1e-12);
    }
    /* the nearest one is a superposed one */{
        const perior::test::xyz  l1(0., 0., 0.);
        const perior::test::xyz  u1(1., 1., 1.);
        const perior::test::aabb box1(l1, u1);

        const perior::test::xyz  l2( 9.,  9.,  9.);
        const perior::test::xyz  u2(10., 10., 10.);
        const perior::test::aabb box2(l2, u2);

        perior::test::aabb ex(box1);
        perior::expand(ex, box2, boundary);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.x, 9.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.y, 9.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.z, 9.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.x, 1.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.y, 1.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.z, 1.0, 1e-12);
    }
    /* the nearest one is a superposed one in only 1D */{
        const perior::test::xyz  l1( 0., 0., 0.);
        const perior::test::xyz  u1( 1., 1., 1.);
        const perior::test::aabb box1(l1, u1);

        const perior::test::xyz  l2( 9., 0., 0.);
        const perior::test::xyz  u2(10., 1., 1.);
        const perior::test::aabb box2(l2, u2);

        perior::test::aabb ex(box1);
        perior::expand(ex, box2, boundary);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.x, 9.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.y, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.lower.z, 0.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.x, 1.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.y, 1.0, 1e-12);
        BOOST_CHECK_CLOSE_FRACTION(ex.upper.z, 1.0, 1e-12);
    }
}
