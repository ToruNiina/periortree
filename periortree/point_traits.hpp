#ifndef PERIOR_TREE_POINT_TRAITS_HPP
#define PERIOR_TREE_POINT_TRAITS_HPP
#include <periortree/config.hpp>
#include <utility> // std::declval(if c++11 enabled)
#include <cstddef> // std::size_t

namespace perior
{
namespace traits
{

struct point_tag{};

template<typename T> struct tag{};
template<typename T> struct dimension_of{};
template<typename T> struct coordinate_type_of{};
template<typename T, std::size_t N> struct point_access{};

namespace detail
{
template<typename pointT, std::size_t I>
struct add_point_impl
{
    typedef pointT point_type;
    typedef traits::point_access<pointT, I-1>         access;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

    BOOST_FORCEINLINE
    static void
    invoke(pointT& lhs, const pointT& rhs)
        BOOST_NOEXCEPT_IF(
            noexcept(access::get(std::declval<point_type>())) &&
            noexcept(access::set(std::declval<point_type&>(),
                                 std::declval<coordinate_type>()))
        )
    {
        access::set(lhs, access::get(lhs) + access::get(rhs));
        return add_point_impl<pointT, I-1>::invoke(lhs, rhs);
    }
};

template<typename pointT>
struct add_point_impl<pointT, 0>
{
    typedef pointT   point_type;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

    BOOST_FORCEINLINE
    static void invoke(pointT& lhs, const pointT& rhs) BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }
};

template<typename pointT, std::size_t I>
struct sub_point_impl
{
    typedef pointT point_type;
    typedef traits::point_access<pointT, I-1>         access;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

    BOOST_FORCEINLINE
    static void
    invoke(pointT& lhs, const pointT& rhs)
        BOOST_NOEXCEPT_IF(
            noexcept(access::get(std::declval<point_type>())) &&
            noexcept(access::set(std::declval<point_type&>(),
                                 std::declval<coordinate_type>()))
        )
    {
        access::set(lhs, access::get(lhs) - access::get(rhs));
        return sub_point_impl<pointT, I-1>::invoke(lhs, rhs);
    }
};

template<typename pointT>
struct sub_point_impl<pointT, 0>
{
    typedef pointT   point_type;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

    BOOST_FORCEINLINE
    static void invoke(pointT& lhs, const pointT& rhs) BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }
};

template<typename pointT, std::size_t I>
struct mul_point_impl
{
    typedef pointT point_type;
    typedef traits::point_access<pointT, I-1>         access;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

    BOOST_FORCEINLINE
    static void
    invoke(pointT& lhs, const coordinate_type& rhs)
        BOOST_NOEXCEPT_IF(
            noexcept(access::get(std::declval<point_type>())) &&
            noexcept(access::set(std::declval<point_type&>(),
                                 std::declval<coordinate_type>()))
        )
    {
        access::set(lhs, access::get(lhs) * rhs);
        return mul_point_impl<pointT, I-1>::invoke(lhs, rhs);
    }

    BOOST_FORCEINLINE
    static void
    invoke(pointT& lhs, const pointT& rhs)
        BOOST_NOEXCEPT_IF(
            noexcept(access::get(std::declval<point_type>())) &&
            noexcept(access::set(std::declval<point_type&>(),
                                 std::declval<coordinate_type>()))
        )
    {
        access::set(lhs, access::get(lhs) * access::get(rhs));
        return mul_point_impl<pointT, I-1>::invoke(lhs, rhs);
    }
};

template<typename pointT>
struct mul_point_impl<pointT, 0>
{
    typedef pointT   point_type;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

    BOOST_FORCEINLINE
    static void invoke(pointT& lhs, const pointT& rhs) BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }

    BOOST_FORCEINLINE
    static void invoke(pointT& lhs, const coordinate_type& rhs)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }
};

template<typename pointT, std::size_t I>
struct div_point_impl
{
    typedef pointT point_type;
    typedef traits::point_access<pointT, I-1>         access;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

    BOOST_FORCEINLINE
    static void
    invoke(pointT& lhs, const coordinate_type& rhs)
        BOOST_NOEXCEPT_IF(
            noexcept(access::get(std::declval<point_type>())) &&
            noexcept(access::set(std::declval<point_type&>(),
                                 std::declval<coordinate_type>()))
        )
    {
        access::set(lhs, access::get(lhs) / rhs);
        return div_point_impl<pointT, I-1>::invoke(lhs, rhs);
    }

    BOOST_FORCEINLINE
    static void
    invoke(pointT& lhs, const pointT& rhs)
        BOOST_NOEXCEPT_IF(
            noexcept(access::get(std::declval<point_type>())) &&
            noexcept(access::set(std::declval<point_type&>(),
                                 std::declval<coordinate_type>()))
        )
    {
        access::set(lhs, access::get(lhs) / access::get(rhs));
        return div_point_impl<pointT, I-1>::invoke(lhs, rhs);
    }
};

template<typename pointT>
struct div_point_impl<pointT, 0>
{
    typedef pointT   point_type;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

    BOOST_FORCEINLINE
    static void invoke(pointT& lhs, const pointT& rhs) BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }

    BOOST_FORCEINLINE
    static void invoke(pointT& lhs, const coordinate_type& rhs)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }
};
}// detail

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, pointT>::type
add_point_v(const pointT& lhs, const pointT& rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::add_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(), std::declval<pointT>()))
    )
{
    pointT retval(lhs);
    detail::add_point_impl<pointT, dimension_of<pointT>::value>::invoke(retval, rhs);
    return retval;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, void>::type
add_point(pointT& lhs, const pointT& rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::add_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(), std::declval<pointT>()))
    )
{
    detail::add_point_impl<pointT, dimension_of<pointT>::value>::invoke(lhs, rhs);
    return;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, pointT>::type
sub_point_v(const pointT& lhs, const pointT& rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::sub_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(), std::declval<pointT>()))
    )
{
    pointT retval(lhs);
    detail::sub_point_impl<pointT, dimension_of<pointT>::value>::invoke(retval, rhs);
    return retval;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, void>::type
sub_point(pointT& lhs, const pointT& rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::sub_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(), std::declval<pointT>()))
    )
{
    detail::sub_point_impl<pointT, dimension_of<pointT>::value>::invoke(lhs, rhs);
    return;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, pointT>::type
mul_point_v(const pointT& lhs, const pointT& rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::mul_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(), std::declval<pointT>()))
    )
{
    pointT retval(lhs);
    detail::mul_point_impl<pointT, dimension_of<pointT>::value>::invoke(retval, rhs);
    return retval;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, void>::type
mul_point(pointT& lhs, const pointT& rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::mul_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(), std::declval<pointT>()))
    )
{
    detail::mul_point_impl<pointT, dimension_of<pointT>::value>::invoke(lhs, rhs);
    return;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, pointT>::type
mul_point_v(const pointT& lhs, const typename coordinate_type_of<pointT>::type rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::mul_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(),
                      std::declval<typename coordinate_type_of<pointT>::type>()))
    )
{
    pointT retval(lhs);
    detail::mul_point_impl<pointT, dimension_of<pointT>::value>::invoke(retval, rhs);
    return retval;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, void>::type
mul_point(pointT& lhs, const typename coordinate_type_of<pointT>::type rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::mul_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(),
                      std::declval<typename coordinate_type_of<pointT>::type>()))
    )
{
    detail::mul_point_impl<pointT, dimension_of<pointT>::value>::invoke(lhs, rhs);
    return;
}


template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, pointT>::type
div_point_v(const pointT& lhs, const pointT& rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::div_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(), std::declval<pointT>()))
    )
{
    pointT retval(lhs);
    detail::div_point_impl<pointT, dimension_of<pointT>::value>::invoke(retval, rhs);
    return retval;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, void>::type
div_point(pointT& lhs, const pointT& rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::div_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(), std::declval<pointT>()))
    )
{
    detail::div_point_impl<pointT, dimension_of<pointT>::value>::invoke(lhs, rhs);
    return;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, pointT>::type
div_point_v(const pointT& lhs, const typename coordinate_type_of<pointT>::type rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::div_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(),
                      std::declval<typename coordinate_type_of<pointT>::type>()))
    )
{
    pointT retval(lhs);
    detail::div_point_impl<pointT, dimension_of<pointT>::value>::invoke(retval, rhs);
    return retval;
}

template<typename pointT>
BOOST_FORCEINLINE
boost::enable_if<boost::is_same<tag<pointT>::type, point_tag>, void>::type
div_point(pointT& lhs, const typename coordinate_type_of<pointT>::type rhs)
    BOOST_NOEXCEPT_IF(
        noexcept(detail::div_point_impl<pointT, dimension_of<pointT>::value
            >::invoke(std::declval<pointT&>(),
                      std::declval<typename coordinate_type_of<pointT>::type>()))
    )
{
    detail::div_point_impl<pointT, dimension_of<pointT>::value>::invoke(lhs, rhs);
    return;
}

}// traits
}// perior
#endif//PERIOR_TREE_POINT_TRAITS_HPP
