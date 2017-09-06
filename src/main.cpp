#include <experimental/rtree.hpp>

int main()
{
    typedef perior::point<double,2> point_t;
    typedef perior::aabb<double,2>  aabb_t;
    typedef perior::unlimited_boundary<double,2> boundary_t;
    typedef std::pair<point_t, std::size_t> value_t;

    perior::rtree<value_t, perior::quadratic<2, 6>, boundary_t> tree;




    return 0;
}
