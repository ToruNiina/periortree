#ifndef PR_TREE_POINT_HPP
#define PR_TREE_POINT_HPP
#include <boost/type_index.hpp>
#include <type_traits>
#include <utility>
#include <array>
#include <cmath>

namespace perior
{

namespace meta
{
template<typename ... Ts>
struct and_{};
template<typename T>
struct and_<T>: std::integral_constant<bool, T::value>{};
template<typename T, typename ...Ts>
struct and_<T, Ts...>: std::integral_constant<bool,
    (T::value ? and_<Ts...>::value : false)>{};
} // meta

template<typename T, std::size_t N>
struct point
{
    typedef T value_type;
    typedef value_type scalar_type;
    constexpr static std::size_t dim = N;

    point() noexcept   = default;
    ~point() noexcept  = default;

    template<typename ...Ts, typename std::enable_if<(sizeof...(Ts) == dim) &&
        meta::and_<std::is_convertible<Ts, scalar_type>...>::value,
        std::nullptr_t>::type = nullptr>
    point(Ts&& ... xs) noexcept : v_{{std::forward<scalar_type>(xs)...}}{}

    point(const point& rhs) noexcept = default;
    point(point&&      rhs) noexcept = default;
    point& operator=(const point& rhs) noexcept = default;
    point& operator=(point&&      rhs) noexcept = default;

    point& operator+=(const point& rhs) noexcept
    {
        for(std::size_t i=0; i<N; ++i)
        {
            v_[i] += rhs[i];
        }
        return *this;
    }
    point& operator-=(const point& rhs)
    {
        for(std::size_t i=0; i<N; ++i)
        {
            v_[i] -= rhs[i];
        }
        return *this;
    }
    point& operator*=(const scalar_type rhs)
    {
        for(std::size_t i=0; i<N; ++i)
        {
            v_[i] *= rhs;
        }
        return *this;
    }
    point& operator/=(const scalar_type rhs)
    {
        for(std::size_t i=0; i<N; ++i)
        {
            v_[i] /= rhs;
        }
        return *this;
    }

    constexpr inline std::size_t size() const noexcept {return dim;}

    scalar_type& operator[](const std::size_t i)       noexcept {return v_[i];}
    scalar_type  operator[](const std::size_t i) const noexcept {return v_[i];}
    scalar_type& at(const std::size_t i)       {return v_.at(i);}
    scalar_type  at(const std::size_t i) const {return v_.at(i);}

  private:
    std::array<value_type, dim> v_;
};

template<typename T, std::size_t N>
inline point<T, N>
operator+(const point<T, N>& lhs, const point<T, N>& rhs) noexcept
{
    point<T, N> p;
    for(std::size_t i=0; i<N; ++i)
    {
        p[i] = lhs[i] + rhs[i];
    }
    return p;
}
template<typename T, std::size_t N>
inline point<T, N>
operator-(const point<T, N>& lhs, const point<T, N>& rhs) noexcept
{
    point<T, N> p;
    for(std::size_t i=0; i<N; ++i)
    {
        p[i] = lhs[i] - rhs[i];
    }
    return p;
}
template<typename T, std::size_t N>
inline point<T, N>
operator*(const point<T, N>& lhs, const T rhs) noexcept
{
    point<T, N> p;
    for(std::size_t i=0; i<N; ++i)
    {
        p[i] = lhs[i] * rhs;
    }
    return p;
}
template<typename T, std::size_t N>
inline point<T, N>
operator*(const T lhs, const point<T, N>& rhs) noexcept
{
    point<T, N> p;
    for(std::size_t i=0; i<N; ++i)
    {
        p[i] = lhs * rhs[i];
    }
    return p;
}
template<typename T, std::size_t N>
inline point<T, N>
operator/(const point<T, N>& lhs, const T rhs) noexcept
{
    point<T, N> p;
    for(std::size_t i=0; i<N; ++i)
    {
        p[i] = lhs[i] / rhs;
    }
    return p;
}

template<typename charT, typename traits, typename realT, std::size_t N>
inline std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os,
           const point<realT, N>& pos)
{
    os << "point<" << boost::typeindex::type_id<realT>().pretty_name() << ", " << N << ">(";
    for(std::size_t i=0; i<N-1; ++i)
    {
        os << pos[i] << ", ";
    }
    os << pos[N-1] << ')';
    return os;
}

template<typename T, std::size_t N>
inline point<T, N>
abs(const point<T, N>& lhs) noexcept
{
    point<T, N> p;
    for(std::size_t i=0; i<N; ++i)
    {
        p[i] = std::abs(lhs[i]);
    }
    return p;
}

template<typename T, std::size_t N>
inline T
dot_product(const point<T, N>& lhs, const point<T, N>& rhs) noexcept
{
    T retval = 0;
    for(std::size_t i=0; i<N; ++i)
    {
        retval += lhs[i] * rhs[i];
    }
    return retval;
}

template<typename T, std::size_t N>
inline point<T, N>
cross_product(const point<T, 3>& lhs, const point<T, 3>& rhs) noexcept
{
    return point<T, N>(lhs[1] * rhs[2] - lhs[2] * rhs[1],
                    lhs[2] * rhs[0] - lhs[0] * rhs[2],
                    lhs[0] * rhs[1] - lhs[1] * rhs[0]);
}

template<typename T, std::size_t N>
inline T
length_sq(const point<T, N>& lhs) noexcept
{
    return dot_product(lhs, lhs);
}

template<typename T, std::size_t N>
inline T
length(const point<T, N>& lhs) noexcept
{
    return std::sqrt(length_sq(lhs));
}

}// perior
#endif//TEST_PERIOR_TREE_POINT_TRAITS_HPP
