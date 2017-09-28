#define BOOST_TEST_MODULE "test_point"

#ifdef UNITTEST_FRAMEWORK_LIBRARY_EXIST
#include <boost/test/unit_test.hpp>
#else
#define BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>
#endif

#include <periortree/point.hpp>

template<typename T>
perior::point<T, 3> make_point(T v1, T v2, T v3)
{
    perior::point<T, 3> retval;
    retval[0] = v1;
    retval[1] = v2;
    retval[2] = v3;
    return retval;
}

BOOST_AUTO_TEST_CASE(test_point_traits)
{
    BOOST_CHECK((perior::traits::is_point<perior::point<double, 2> >::value));
    BOOST_CHECK((perior::traits::is_point<perior::point<int,    2> >::value));
    BOOST_CHECK((perior::traits::is_point<perior::point<double, 3> >::value));
    BOOST_CHECK((perior::traits::is_point<perior::point<int,    3> >::value));

    BOOST_CHECK((perior::traits::dimension<perior::point<int, 1> >::value) == 1);
    BOOST_CHECK((perior::traits::dimension<perior::point<int, 2> >::value) == 2);
    BOOST_CHECK((perior::traits::dimension<perior::point<int, 3> >::value) == 3);

    BOOST_CHECK((boost::is_same<int,
        perior::traits::scalar_type_of<perior::point<int, 1> >::type>::value));
    BOOST_CHECK((boost::is_same<float,
        perior::traits::scalar_type_of<perior::point<float, 1> >::type>::value));
    BOOST_CHECK((boost::is_same<double,
        perior::traits::scalar_type_of<perior::point<double, 1> >::type>::value));

    perior::point<int, 3> zi =
        perior::traits::zero_vector<perior::point<int, 3> >();
    BOOST_CHECK_EQUAL(zi[0], 0);
    BOOST_CHECK_EQUAL(zi[1], 0);
    BOOST_CHECK_EQUAL(zi[2], 0);

    perior::point<double, 3> zd =
        perior::traits::zero_vector<perior::point<double, 3> >();
    BOOST_CHECK_EQUAL(zd[0], 0);
    BOOST_CHECK_EQUAL(zd[1], 0);
    BOOST_CHECK_EQUAL(zd[2], 0);
}

BOOST_AUTO_TEST_CASE(test_xyz_access)
{
    perior::point<double, 3> p = make_point(1., 2., 3.);
    BOOST_CHECK_EQUAL(p[0], 1.);
    BOOST_CHECK_EQUAL(p[1], 2.);
    BOOST_CHECK_EQUAL(p[2], 3.);

    p[0] = 0.0;
    p[1] = 2.0;
    p[2] = 4.0;

    BOOST_CHECK_EQUAL(p[0], 0.0);
    BOOST_CHECK_EQUAL(p[1], 2.0);
    BOOST_CHECK_EQUAL(p[2], 4.0);
}

BOOST_AUTO_TEST_CASE(test_point_add)
{
    perior::point<double, 3> p = make_point(1., 2., 3.);
    perior::point<double, 3> q = make_point(2., 5., 8.);
    perior::point<double, 3> r = p + q;
    BOOST_CHECK_CLOSE_FRACTION(r[0],  3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r[1],  7.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r[2], 11.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_point_sub)
{
    perior::point<double, 3> p = make_point(2., 5., 8.);
    perior::point<double, 3> q = make_point(1., 2., 3.);
    perior::point<double, 3> r = p - q;
    BOOST_CHECK_CLOSE_FRACTION(r[0], 1.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r[1], 3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r[2], 5.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_point_scl_mul)
{
    perior::point<double, 3> p = make_point(1., 2., 3.);
    double q = 3;
    perior::point<double, 3> r = p * q;
    BOOST_CHECK_CLOSE_FRACTION(r[0], 3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r[1], 6.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r[2], 9.0, 1e-12);
}

BOOST_AUTO_TEST_CASE(test_position_scl_div)
{
    perior::point<double, 3> p = make_point(1., 2., 3.);
    double q = 3;
    perior::point<double, 3> r = p / q;
    BOOST_CHECK_CLOSE_FRACTION(r[0], 1./3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r[1], 2./3.0, 1e-12);
    BOOST_CHECK_CLOSE_FRACTION(r[2], 3./3.0, 1e-12);
}
