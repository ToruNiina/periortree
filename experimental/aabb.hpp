#ifndef PR_TREE_AABB_HPP
#define PR_TREE_AABB_HPP
#include "point.hpp"

namespace perior
{

template<typename realT, std::size_t N>
struct aabb
{
    typedef T value_type;
    typedef value_type scalar_type;
    constexpr static std::size_t dim = N;
    typedef point<scalar_type, dim> point_type;

    aabb()  noexcept = default;
    ~aabb() noexcept = default;
    aabb(const aabb&) noexcept = default;
    aabb(aabb&&)      noexcept = default;
    aabb& operator=(const aabb&) noexcept = default;
    aabb& operator=(aabb&&)      noexcept = default;

    aabb(const point_type& lw, const point_type& up) noexcept
      : lower_(lw), upper_(up)
    {}

    point_type&       upper()       noexcept {return upper_;}
    point_type const& upper() const noexcept {return upper_;}
    point_type&       lower()       noexcept {return lower_;}
    point_type const& lower() const noexcept {return lower_;}

  private:
    point_type lower_, upper_;
};


} // perior
#endif//PR_TREE_AABB_HPP
