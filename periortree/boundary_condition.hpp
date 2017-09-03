#ifndef PERIOR_TREE_BOUNDARY_CONDITION_HPP
#define PERIOR_TREE_BOUNDARY_CONDITION_HPP
#include <periortree/config.hpp>
#include <periortree/point_traits.hpp>
#include <periortree/aabb_traits.hpp>

#if BOOST_VERSION >= 105600
#include <boost/core/enable_if.hpp>
#else
#include <boost/utility/enable_if.hpp>
#endif

#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>

namespace perior
{

template<typename pointT>
struct unlimited_boundary
{
    BOOST_STATIC_ASSERT_MSG(
        (boost::is_same<typename traits::tag<pointT>::type,
                        traits::point_tag>::value),
        "perior::unlimited_boundary: pointT must be tagged as point_type");

    typedef pointT   point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;

    unlimited_boundary(){}
    ~unlimited_boundary(){}

};

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<
    boost::is_same<typename traits::tag<pointT>::type, traits::point_tag>,
    pointT>::type
adjust_position(pointT x, const unlimited_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    return x;
}

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<
    boost::is_same<typename traits::tag<pointT>::type, traits::point_tag>,
    pointT>::type
adjust_direction(pointT x, const unlimited_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    return x;
}

template<typename pointT>
struct cubic_periodic_boundary
{
    BOOST_STATIC_ASSERT_MSG(
        (boost::is_same<typename traits::tag<pointT>::type,
                        traits::point_tag>::value),
        "perior::cubic_periodic_boundary: pointT must be tagged as point_type");

    typedef pointT   point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;

    cubic_periodic_boundary(){};
    ~cubic_periodic_boundary(){};

    cubic_periodic_boundary(const point_type& low, const point_type& up)
        : lower_(low), upper_(up)
    {}
    cubic_periodic_boundary(const cubic_periodic_boundary& rhs)
        : lower_(rhs.lower_), upper_(rhs.upper_)
    {}
    cubic_periodic_boundary& operator=(const cubic_periodic_boundary& rhs)
    {
        lower_ = rhs.lower_;
        upper_ = rhs.upper_;
    }

#ifdef PERIOR_TREE_ENABLE_CXX11
    cubic_periodic_boundary(point_type&& lhs, point_type&& rhs)
        noexcept(std::is_nothrow_move_constructible<point_type>::value)
        : lower_(std::move(lhs)), upper_(std::move(rhs))
    {}
    cubic_periodic_boundary(cubic_periodic_boundary&&)
        noexcept(std::is_nothrow_move_constructible<point_type>::value) = default;
    cubic_periodic_boundary& operator=(cubic_periodic_boundary&&)
        noexcept(std::is_nothrow_move_constructible<point_type>::value) = default;
#endif

    BOOST_FORCEINLINE
    point_type&       lower()       BOOST_NOEXCEPT_OR_NOTHROW {return lower_;}
    BOOST_FORCEINLINE
    point_type&       upper()       BOOST_NOEXCEPT_OR_NOTHROW {return upper_;}
    BOOST_FORCEINLINE
    point_type const& lower() const BOOST_NOEXCEPT_OR_NOTHROW {return lower_;}
    BOOST_FORCEINLINE
    point_type const& upper() const BOOST_NOEXCEPT_OR_NOTHROW {return upper_;}

  private:
    point_type lower_, upper_;
};

// access and motify cubic_periodic_boundary in the same way as aabb
namespace traits
{
template<typename T>
struct tag<cubic_periodic_boundary<T> > {typedef aabb_tag type;};
template<typename T>
struct point_type_of<cubic_periodic_boundary<T> >{typedef T type;};
template<typename T>
struct coordinate_type_of<cubic_periodic_boundary<T> >
{typedef typename coordinate_type_of<T>::type type;};
template<typename T>
struct dimension_of<cubic_periodic_boundary<T> >
: dimension_of<typename point_type_of<cubic_periodic_boundary<T> >::type>{};

template<typename T, std::size_t D>
struct box_access<cubic_periodic_boundary<T>, max_corner, D>
{
    typedef cubic_periodic_boundary<T> box_type;
    typedef typename point_type_of<box_type>::type point_type;
    typedef typename coordinate_type_of<box_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type get(const box_type& b)
        BOOST_NOEXCEPT_IF(noexcept(point_access<point_type, D>::get(
            std::declval<point_type>(), std::declval<coordinate_type>()))
        )
    {
        return point_access<point_type, D>::get(b.upper());
    }

    BOOST_FORCEINLINE
    static void set(box_type& b, coordinate_type x)
        BOOST_NOEXCEPT_IF(noexcept(point_access<point_type, D>::set(
            std::declval<point_type&>(), std::declval<coordinate_type>()))
        )
    {
        return point_access<point_type, D>::set(b.upper(), x);
    }
};

template<typename T, std::size_t D>
struct box_access<cubic_periodic_boundary<T>, min_corner, D>
{
    typedef cubic_periodic_boundary<T> box_type;
    typedef typename point_type_of<box_type>::type      point_type;
    typedef typename coordinate_type_of<box_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type get(const box_type& b)
        BOOST_NOEXCEPT_IF(noexcept(point_access<point_type, D>::get(
            std::declval<point_type>(), std::declval<coordinate_type>()))
        )
    {
        return point_access<point_type, D>::get(b.lower());
    }

    BOOST_FORCEINLINE
    static void set(box_type& b, coordinate_type x)
        BOOST_NOEXCEPT_IF(noexcept(point_access<point_type, D>::set(
            std::declval<point_type&>(), std::declval<coordinate_type>()))
        )
    {
        return point_access<point_type, D>::set(b.lower(), x);
    }
};

template<typename T, std::size_t D>
struct box_range_access<cubic_periodic_boundary<T>, D>
{
    typedef cubic_periodic_boundary<T> box_type;
    typedef typename point_type_of<box_type>::type      point_type;
    typedef typename coordinate_type_of<box_type>::type coordinate_type;

    BOOST_FORCEINLINE
    static coordinate_type get(const box_type& b)
        BOOST_NOEXCEPT_IF(
            noexcept(point_access<point_type, D>::get(b.lower())) &&
            noexcept(std::declval<coordinate_type>() -
                     std::declval<coordinate_type>())
        )
    {
        return point_access<point_type, D>::get(b.upper()) -
               point_access<point_type, D>::get(b.lower());
    }
};

} // traits

namespace detail
{

// adjust position -------------------------------------------------------------

template<typename coordT>
BOOST_FORCEINLINE
coordT adjust_position_1D(coordT x, const coordT mn, const coordT mx)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    const coordT range = mx - mn;
    if(x < mn)
    {
        x += range;
    }
    else if(x > mx)
    {
        x -= range;
    }
    return x;
}

template<typename pointT, std::size_t I>
struct adjust_position_impl
{
    typedef traits::point_access<pointT, I-1>         access;
    typedef typename traits::coordinate_type_of<pointT>::type coordinate_type;

    BOOST_FORCEINLINE
    static void
    invoke(pointT& p, const pointT& up, const pointT& lw, const pointT& ds)
        BOOST_NOEXCEPT_IF(
            noexcept(access::get(p)) &&
            noexcept(access::set(p, std::declval<coordinate_type>()))
        )
    {
        const coordinate_type x = access::get(p);

        if(x < access::get(lw))
        {
            access::set(p, x + access::get(ds));
        }
        else if(x > access::get(up))
        {
            access::set(p, x - access::get(ds));
        }
        return adjust_position_impl<pointT, I-1>::invoke(p, up, lw, ds);
    }
};

template<typename pointT>
struct adjust_position_impl<pointT, 0>
{
    BOOST_FORCEINLINE
    static void
    invoke(pointT& p, const pointT& up, const pointT& lw, const pointT& ds)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }
};

// adjust direction ------------------------------------------------------------

template<typename coordT>
BOOST_FORCEINLINE
coordT adjust_direction_1D(coordT dx, const coordT range) BOOST_NOEXCEPT_OR_NOTHROW
{
    const coordT range_half = range / 2;
    if(dx < -range_half)
    {
        dx += range;
    }
    else if(dx > range_half)
    {
        dx -= range;
    }
    return dx;
}

template<typename pointT, std::size_t I>
struct adjust_direction_impl
{
    typedef traits::point_access<pointT, I-1>         access;
    typedef typename traits::coordinate_type_of<pointT>::type coordinate_type;

    // dsh = ds / 2
    BOOST_FORCEINLINE
    static void
    invoke(pointT& d, const pointT& ds, const pointT& dsh)
        BOOST_NOEXCEPT_IF(
            noexcept(access::get(d)) &&
            noexcept(access::set(d, std::declval<coordinate_type>()))
        )
    {
        const coordinate_type x = access::get(d);

        if(x < -access::get(dsh))
        {
            access::set(d, x + access::get(ds));
        }
        else if(x > access::get(dsh))
        {
            access::set(d, x - access::get(ds));
        }
        return adjust_direction_impl<pointT, I-1>::invoke(d, ds, dsh);
    }
};

template<typename pointT>
struct adjust_direction_impl<pointT, 0>
{
    BOOST_FORCEINLINE
    static void
    invoke(pointT& d, const pointT& ds, const pointT& dsh)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }
};

}// detail

template<typename pointT>
inline typename boost::enable_if<
    boost::is_same<typename traits::tag<pointT>::type, traits::point_tag>,
    pointT>::type
adjust_position(pointT x, const cubic_periodic_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    const pointT& up = b.upper();
    const pointT& lw = b.lower();
    const pointT ds = traits::sub_point_v(up, lw);

    detail::adjust_position_impl<pointT, traits::dimension_of<pointT>::value
        >::invoke(x, up, lw, ds);
    return x;
}

template<typename pointT>
inline typename boost::enable_if<
    boost::is_same<typename traits::tag<pointT>::type, traits::point_tag>,
    pointT>::type
adjust_direction(pointT x, const cubic_periodic_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    const pointT ds      = traits::sub_point_v(b.upper(), b.lower());
    const pointT ds_half = traits::mul_point_v(ds, 0.5);

    detail::adjust_direction_impl<pointT, traits::dimension_of<pointT>::value
        >::invoke(x, ds, ds_half);
    return x;
}

}// perior
#endif//PERIOR_TREE_BOUNDARY_CONDITION_HPP
