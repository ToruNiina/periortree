#define BOOST_TEST_MODULE "test_point"

#ifdef UNITTEST_FRAMEWORK_LIBRARY_EXIST
#include <boost/test/unit_test.hpp>
#else
#define BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>
#endif

#include <periortree/point_traits.hpp>
#include <test/point_type.hpp>

BOOST_AUTO_TEST_CASE(test_xyz_traits)
{
    BOOST_CHECK((boost::is_same<
        typename perior::traits::tag<perior::test::xyz>::type,
        perior::traits::point_tag>::value));

    BOOST_CHECK((boost::is_same<
        typename perior::traits::coordinate_type_of<perior::test::xyz>::type,
        double>::value));

    BOOST_CHECK(perior::traits::dimension_of<perior::test::xyz>::value == 3);
}

BOOST_AUTO_TEST_CASE(test_position_traits)
{
    BOOST_CHECK((boost::is_same<
        typename perior::traits::tag<perior::test::position>::type,
        perior::traits::point_tag>::value));

    BOOST_CHECK((boost::is_same<
        typename perior::traits::coordinate_type_of<perior::test::position>::type,
        double>::value));

    BOOST_CHECK(perior::traits::dimension_of<perior::test::position>::value == 3);
}

BOOST_AUTO_TEST_CASE(test_xyz_construct)
{
    {
        const perior::test::xyz p;
        BOOST_CHECK_EQUAL(p.x, 0.0);
        BOOST_CHECK_EQUAL(p.y, 0.0);
        BOOST_CHECK_EQUAL(p.z, 0.0);
    }
    {
        const perior::test::xyz p(1., 2., 3.);
        BOOST_CHECK_EQUAL(p.x, 1.0);
        BOOST_CHECK_EQUAL(p.y, 2.0);
        BOOST_CHECK_EQUAL(p.z, 3.0);
    }
    {
        const perior::test::xyz p(1., 2., 3.);
        const perior::test::xyz q(p);

        BOOST_CHECK_EQUAL(q.x, 1.0);
        BOOST_CHECK_EQUAL(q.y, 2.0);
        BOOST_CHECK_EQUAL(q.z, 3.0);
    }
    {
        const perior::test::xyz p(1., 2., 3.);
        perior::test::xyz q;
        BOOST_CHECK_EQUAL(q.x, 0.0);
        BOOST_CHECK_EQUAL(q.y, 0.0);
        BOOST_CHECK_EQUAL(q.z, 0.0);
        q = p;
        BOOST_CHECK_EQUAL(q.x, 1.0);
        BOOST_CHECK_EQUAL(q.y, 2.0);
        BOOST_CHECK_EQUAL(q.z, 3.0);
    }
}

BOOST_AUTO_TEST_CASE(test_position_construct)
{
    {
        const perior::test::position p;
        BOOST_CHECK_EQUAL(p.v[0], 0.0);
        BOOST_CHECK_EQUAL(p.v[1], 0.0);
        BOOST_CHECK_EQUAL(p.v[2], 0.0);
    }
    {
        const perior::test::position p(1., 2., 3.);
        BOOST_CHECK_EQUAL(p.v[0], 1.0);
        BOOST_CHECK_EQUAL(p.v[1], 2.0);
        BOOST_CHECK_EQUAL(p.v[2], 3.0);
    }
    {
        const perior::test::position p(1., 2., 3.);
        const perior::test::position q(p);

        BOOST_CHECK_EQUAL(q.v[0], 1.0);
        BOOST_CHECK_EQUAL(q.v[1], 2.0);
        BOOST_CHECK_EQUAL(q.v[2], 3.0);
    }
    {
        const perior::test::position p(1., 2., 3.);
        perior::test::position q;
        BOOST_CHECK_EQUAL(q.v[0], 0.0);
        BOOST_CHECK_EQUAL(q.v[1], 0.0);
        BOOST_CHECK_EQUAL(q.v[2], 0.0);
        q = p;
        BOOST_CHECK_EQUAL(q.v[0], 1.0);
        BOOST_CHECK_EQUAL(q.v[1], 2.0);
        BOOST_CHECK_EQUAL(q.v[2], 3.0);
    }
}

BOOST_AUTO_TEST_CASE(test_xyz_access)
{
    perior::test::xyz p(1., 2., 3.);
    BOOST_CHECK_EQUAL(
            p.x, (perior::traits::point_access<perior::test::xyz, 0>::get(p)));
    BOOST_CHECK_EQUAL(
            p.y, (perior::traits::point_access<perior::test::xyz, 1>::get(p)));
    BOOST_CHECK_EQUAL(
            p.z, (perior::traits::point_access<perior::test::xyz, 2>::get(p)));

    perior::traits::point_access<perior::test::xyz, 0>::set(p, 0.0);
    perior::traits::point_access<perior::test::xyz, 1>::set(p, 2.0);
    perior::traits::point_access<perior::test::xyz, 2>::set(p, 4.0);

    BOOST_CHECK_EQUAL(p.x, 0.0);
    BOOST_CHECK_EQUAL(p.y, 2.0);
    BOOST_CHECK_EQUAL(p.z, 4.0);
}

BOOST_AUTO_TEST_CASE(test_position_access)
{
    perior::test::position p(1., 2., 3.);
    BOOST_CHECK_EQUAL(
            p.v[0], (perior::traits::point_access<perior::test::position, 0>::get(p)));
    BOOST_CHECK_EQUAL(
            p.v[1], (perior::traits::point_access<perior::test::position, 1>::get(p)));
    BOOST_CHECK_EQUAL(
            p.v[2], (perior::traits::point_access<perior::test::position, 2>::get(p)));

    perior::traits::point_access<perior::test::position, 0>::set(p, 0.0);
    perior::traits::point_access<perior::test::position, 1>::set(p, 2.0);
    perior::traits::point_access<perior::test::position, 2>::set(p, 4.0);

    BOOST_CHECK_EQUAL(p.v[0], 0.0);
    BOOST_CHECK_EQUAL(p.v[1], 2.0);
    BOOST_CHECK_EQUAL(p.v[2], 4.0);
}

BOOST_AUTO_TEST_CASE(test_xyz_add)
{
    perior::test::xyz p(1., 2., 3.);
    perior::test::xyz q(2., 5., 8.);
    perior::test::xyz r = perior::traits::add_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x,  3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y,  7.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 11.0, 1e-12);

    perior::traits::add_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x,  5.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 12.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 19.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_position_add)
{
    perior::test::position p(1., 2., 3.);
    perior::test::position q(2., 5., 8.);
    perior::test::position r = perior::traits::add_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0],  3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1],  7.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 11.0, 1e-12);

    perior::traits::add_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0],  5.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 12.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 19.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_xyz_sub)
{
    perior::test::xyz p(2., 5., 8.);
    perior::test::xyz q(1., 2., 3.);
    perior::test::xyz r = perior::traits::sub_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x, 1.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 5.0, 1e-12);

    perior::traits::sub_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x, 0.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 1.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 2.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_position_sub)
{
    perior::test::position p(2., 5., 8.);
    perior::test::position q(1., 2., 3.);
    perior::test::position r = perior::traits::sub_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0], 1.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 5.0, 1e-12);

    perior::traits::sub_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0], 0.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 1.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 2.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_xyz_mul)
{
    perior::test::xyz p(1., 2., 3.);
    perior::test::xyz q(2., 5., 8.);
    perior::test::xyz r = perior::traits::mul_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x,  2.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 10.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 24.0, 1e-12);

    perior::traits::mul_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x,   4.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y,  50.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 192.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_position_mul)
{
    perior::test::position p(1., 2., 3.);
    perior::test::position q(2., 5., 8.);
    perior::test::position r = perior::traits::mul_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0],  2.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 10.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 24.0, 1e-12);

    perior::traits::mul_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0],   4.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1],  50.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 192.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_xyz_scl_mul)
{
    perior::test::xyz p(1., 2., 3.);
    double q = 3;
    perior::test::xyz r = perior::traits::mul_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x, 3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 6.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 9.0, 1e-12);

    perior::traits::mul_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x,  9.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 18.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 27.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_position_scl_mul)
{
    perior::test::position p(1., 2., 3.);
    double q = 3;
    perior::test::position r = perior::traits::mul_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0], 3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 6.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 9.0, 1e-12);

    perior::traits::mul_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0],  9.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 18.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 27.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_xyz_div)
{
    perior::test::xyz p(1., 2., 3.);
    perior::test::xyz q(2., 5., 8.);
    perior::test::xyz r = perior::traits::div_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x, 1./2., 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 2./5., 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 3./8., 1e-12);

    perior::traits::div_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x, 1./2./2., 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 2./5./5., 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 3./8./8., 1e-12);
}

BOOST_AUTO_TEST_CASE(test_position_div)
{
    perior::test::position p(1., 2., 3.);
    perior::test::position q(2., 5., 8.);
    perior::test::position r = perior::traits::div_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0], 1./2., 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 2./5., 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 3./8., 1e-12);

    perior::traits::div_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0], 1./2./2., 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 2./5./5., 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 3./8./8., 1e-12);
}

BOOST_AUTO_TEST_CASE(test_xyz_scl_div)
{
    perior::test::xyz p(1., 2., 3.);
    double q = 3;
    perior::test::xyz r = perior::traits::div_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x, 1./3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 2./3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 3./3.0, 1e-12);

    perior::traits::div_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.x, 1./9.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.y, 2./9.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.z, 3./9.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_position_scl_div)
{
    perior::test::position p(1., 2., 3.);
    double q = 3;
    perior::test::position r = perior::traits::div_point_v(p, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0], 1./3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 2./3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 3./3.0, 1e-12);

    perior::traits::div_point(r, q);
    BOOST_CHECK_CLOSE_FRACTION(r.v[0], 1./9.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[1], 2./9.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r.v[2], 3./9.0, 1e-12);
}
