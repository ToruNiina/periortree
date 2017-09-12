#ifndef PERIOR_TREE_BOUNDARY_CONDITION_HPP
#define PERIOR_TREE_BOUNDARY_CONDITION_HPP
#include "point.hpp"

namespace perior
{

template<typename realT, std::size_t N>
struct unlimited_boundary
{
    typedef realT value_type;
    typedef value_type scalar_type;
    constexpr static std::size_t dim = N;
};

template<typename T, std::size_t N>
inline point<T, N>
adjust_position(point<T, N> x, const unlimited_boundary<T, N>& b) noexcept
{
    return x;
}

template<typename T, std::size_t N>
inline point<T, N>
adjust_direction(point<T, N> x, const unlimited_boundary<T, N>& b) noexcept
{
    return x;
}

template<typename T, std::size_t N>
inline point<T, N>
span(const point<T, N>& lw, const point<T, N>& up,
     const unlimited_boundary<T, N>& b) noexcept
{
    return up - lw;
}

template<typename realT, std::size_t N>
struct cubic_periodic_boundary
{
    typedef realT value_type;
    typedef value_type scalar_type;
    constexpr static std::size_t dim = N;
    typedef point<scalar_type, dim> point_type;

    cubic_periodic_boundary()  noexcept = default;
    ~cubic_periodic_boundary() noexcept = default;
    cubic_periodic_boundary(const cubic_periodic_boundary&) noexcept = default;
    cubic_periodic_boundary(cubic_periodic_boundary&&)      noexcept = default;
    cubic_periodic_boundary& operator=(const cubic_periodic_boundary&) noexcept = default;
    cubic_periodic_boundary& operator=(cubic_periodic_boundary&&)      noexcept = default;

    cubic_periodic_boundary(const point_type& low, const point_type& up)
        : lower_(low), upper_(up), range_(up - low), range_half_((up-low) * 0.5)
    {}

    point_type const& upper() const noexcept {return upper_;}
    point_type const& lower() const noexcept {return lower_;}
    point_type const& range() const noexcept {return range_;}
    point_type const& range_half() const noexcept {return range_half_;}

    void set_upper(const point_type& p) noexcept
    {
        upper_ = p;
        range_ = upper_ - lower_;
    }
    void set_lower(const point_type& p) noexcept
    {
        lower_ = p;
        range_ = upper_ - lower_;
    }

  private:
    point_type lower_, upper_, range_, range_half_;
};

template<typename T, std::size_t N>
inline point<T, N>
adjust_position(point<T, N> x, const cubic_periodic_boundary<T, N>& b) noexcept
{
    for(std::size_t i=0; i<N; ++i)
    {
             if(x[i] < b.lower()[i]) {x[i] += b.range()[i];}
        else if(x[i] > b.upper()[i]) {x[i] -= b.range()[i];}
    }
    return x;
}

template<typename T, std::size_t N>
inline point<T, N>
adjust_direction(point<T, N> x, const cubic_periodic_boundary<T, N>& b) noexcept
{
    for(std::size_t i=0; i<N; ++i)
    {
             if(x[i] < -(b.range_half()[i])) {x[i] += b.range()[i];}
        else if(x[i] >   b.range_half()[i])  {x[i] -= b.range()[i];}
    }
    return x;
}

template<typename T, std::size_t N>
inline point<T, N>
span(const point<T, N>& lw, const point<T, N>& up,
     const cubic_periodic_boundary<T, N>& b) noexcept
{
    point<T, N> retval;
    for(std::size_t i=0; i<N; ++i)
    {
             if(lw[i] < up[i]) {retval[i] = up[i] - lw[i];}
        else if(lw[i] > up[i]) {retval[i] = up[i] - lw[i] + b.range()[i];}
    }
    return retval;
}

}// perior
#endif//PERIOR_TREE_BOUNDARY_CONDITION_HPP
