#ifndef PERIOR_TREE_BOUNDARY_CONDITION_HPP
#define PERIOR_TREE_BOUNDARY_CONDITION_HPP
#include <periortree/config.hpp>
#include <periortree/point_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>

namespace perior
{

template<typename pointT>
struct unlimited_boundary
{
    BOOST_STATIC_ASSERT_MSG(
        boost::is_same<typename traits::tag<pointT>::type,
                       traits::point_tag>::value,
        "perior::unlimited_boundary: pointT must be tagged as point_type");

    typedef pointT   point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;
};

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<
    boost::is_same<traits::tag<pointT>::type, traits::point_tag>,
    pointT>::type
adjust_position(pointT x, const unlimited_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    return x;
}

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<
    boost::is_same<traits::tag<pointT>::type, traits::point_tag>,
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
        boost::is_same<typename traits::tag<pointT>::type,
                       traits::point_tag>::value,
        "perior::unlimited_boundary: pointT must be tagged as point_type");

    typedef pointT   point_type;
    typedef typename traits::coordinate_type_of<point_type>::type coordinate_type;

    cubic_periodic_boundary(){};
    ~cubic_periodic_boundary(){};

    cubic_periodic_boundary(const point_type& lhs, const point_type& rhs)
        : lower_(lhs), upper_(rhs)
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
    cubic_periodic_boundary()
        noexcept(std::is_nothrow_move_constructible<point_type>::value) = default;
    cubic_periodic_boundary& operator=()
        noexcept(std::is_nothrow_move_constructible<point_type>::value) = default;
#endif

    point_type&       lower()       BOOST_NOEXCEPT_OR_NOTHROW {return lower_;}
    point_type&       upper()       BOOST_NOEXCEPT_OR_NOTHROW {return upper_;}
    point_type const& lower() const BOOST_NOEXCEPT_OR_NOTHROW {return lower_;}
    point_type const& upper() const BOOST_NOEXCEPT_OR_NOTHROW {return upper_;}

  private:
    point_type lower_, upper_;
};

namespace detail
{

// adjust position -------------------------------------------------------------

template<typename pointT, std::size_t I>
struct adjust_position_impl
{
    typedef traits::point_access<pointT, I-1>         access;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

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

template<typename pointT, std::size_t I>
struct adjust_direction_impl
{
    typedef traits::point_access<pointT, I-1>         access;
    typedef typename coordinate_type_of<pointT>::type coordinate_type;

    // dsh = ds / 2
    BOOST_FORCEINLINE
    static void
    invoke(pointT& p, const pointT& ds, const pointT& dsh)
        BOOST_NOEXCEPT_IF(
            noexcept(access::get(p)) &&
            noexcept(access::set(p, std::declval<coordinate_type>()))
        )
    {
        const coordinate_type x = access::get(p);

        if(x < -access::get(dsh))
        {
            access::set(p, x + access::get(ds));
        }
        else if(x > access::get(dsh))
        {
            access::set(p, x - access::get(ds));
        }
        return adjust_position_impl<pointT, I-1>::invoke(p, ds, dsh);
    }
};

template<typename pointT>
struct adjust_direction_impl<pointT, 0>
{
    BOOST_FORCEINLINE
    static void
    invoke(pointT& p, const pointT& ds, const pointT& dsh)
        BOOST_NOEXCEPT_OR_NOTHROW
    {
        return;
    }
};

}// detail

template<typename pointT>
inline typename boost::enable_if<
    boost::is_same<traits::tag<pointT>::type, traits::point_tag>,
    pointT>::type
adjust_position(pointT x, const cubic_periodic_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    const pointT& up = b.upper();
    const pointT& lw = b.lower();
    const pointT ds = traits::sub_point(up, lw);

    detail::adjust_position_impl<pointT, traits::dimension_of<pointT>::value
        >::invoke(x, up, lw, ds);
    return x;
}

template<typename pointT>
inline typename boost::enable_if<
    boost::is_same<traits::tag<pointT>::type, traits::point_tag>,
    pointT>::type
adjust_direction(pointT x, const cubic_periodic_boundary<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    const pointT ds      = traits::sub_point(b.upper(), b.lower());
    const pointT ds_half = traits::mul_point(ds, 0.5);

    detail::adjust_position_impl<pointT, traits::dimension_of<pointT>::value
        >::invoke(x, ds, ds_half);
    return x;
}

}// perior
#define PERIOR_TREE_BOUNDARY_CONDITION_HPP
