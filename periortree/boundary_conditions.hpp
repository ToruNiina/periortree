#ifndef PERIOR_TREE_BOUNDARY_CONDITIONS
#define PERIOR_TREE_BOUNDARY_CONDITIONS
#include <periortree/point_traits.hpp>
#include <periortree/point_ops.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/static_assert.hpp>
#include <boost/config.hpp>

namespace perior
{

template<typename pointT>
struct unlimited_boundary
{
    typedef pointT point_type;
    BOOST_STATIC_ASSERT(traits::is_point<point_type>::value);
};

template<typename pointT>
struct cubic_periodic_boundary
{
    typedef pointT point_type;
    BOOST_STATIC_ASSERT(traits::is_point<point_type>::value);

    using ops::operator-;
    using ops::operator/;

    cubic_periodic_boundary(const point_type& l, const point_type& u)
        : lower_(l), upper_(u), width_(u - l), half_width((u - l) / 2)
    {}

    explicit cubic_periodic_boundary(const point_type& u)
        : lower_(traits::zero_vector<point_type>()), upper_(u),
          width_(u), half_width(u / 2)
    {}
    ~cubic_periodic_boundary(){}

    cubic_periodic_boundary(const cubic_periodic_boundary& rhs)
        : lower_(rhs.lower_), upper_(rhs.upper_),
          width_(rhs.width_), half_width_(rhs.half_width_)
    {}
    cubic_periodic_boundary& operator=(const cubic_periodic_boundary& rhs)
    {
        lower_ = rhs.lower_;
        upper_ = rhs.upper_;
        width_ = rhs.width_;
        half_width_ = rhs.half_width_;
        return *this;
    }

    BOOST_FORCEINLINE
    point_type const& upper() const BOOST_NOEXCEPT_OR_NOTHROW {return upper_;}
    BOOST_FORCEINLINE
    point_type const& lower() const BOOST_NOEXCEPT_OR_NOTHROW {return lower_;}
    BOOST_FORCEINLINE
    point_type const& width() const BOOST_NOEXCEPT_OR_NOTHROW {return width_;}
    BOOST_FORCEINLINE
    point_type const& half_width() const BOOST_NOEXCEPT_OR_NOTHROW {return half_width_;}

  private:

    point_type lower_, upper_;
    point_type width_, half_width_;
};

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<traits::is_point<pointT>, pointT>::type
restrict_position(const pointT& p, const unlimited_boundary<pointT>& u)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    return p;
}

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<traits::is_point<pointT>, pointT>::type
restrict_direction(const pointT& d, const unlimited_boundary<pointT>& u)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    return d;
}

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<traits::is_point<pointT>, pointT>::type
restrict_position(pointT p, const cubic_periodic_boundary<pointT>& u)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
             if(p[i] <  u.lower()[i]){p[i] += u.width()[i];}
        else if(p[i] >= u.upper()[i]){p[i] -= u.width()[i];}
    }
    return p;
}

template<typename pointT>
BOOST_FORCEINLINE
typename boost::enable_if<traits::is_point<pointT>, pointT>::type
restrict_direction(pointT d, const cubic_periodic_boundary<pointT>& u)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
             if(p[i] <  -(u.half_width()[i])){p[i] += u.width()[i];}
        else if(p[i] >=   u.half_width()[i]) {p[i] -= u.width()[i];}
    }
    return d;
}

} // perior
#endif//PERIOR_TREE_BOUNDARY_CONDITIONS
