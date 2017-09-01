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
//
// BOOST_AUTO_TEST_CASE(test_position_access)
// {
// }
