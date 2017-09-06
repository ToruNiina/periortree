#ifndef PERIOR_TREE_NODE_HPP
#define PERIOR_TREE_NODE_HPP
#include "point.hpp"
#include "aabb.hpp"

#include <boost/container/static_vector.hpp>

namespace perior
{
namespace detail
{

template<typename T, std::size_t D, std::size_t Min, std::size_t Max>
struct rtree_node
{
    typedef T value_type;
    typedef value_type scalar_type;
    static constexpr std::size_t dim = D;
    typedef point<scalar_type, dim> point_type;
    typedef aabb<scalar_type, dim>  aabb_type;

    static constexpr std::size_t max_entry = Max;
    static constexpr std::size_t min_entry = Min;

    typedef boost::container::static_vector<std::size_t, Max> container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    rtree_node(const bool is_leaf_, const std::size_t parent_)
        : is_leaf(is_leaf_), parent(parent_)
    {}
    ~rtree_node(){}

    bool has_enough_storage() const noexcept
    {return this->children.size() < Max;}

    bool has_enough_entry() const noexcept
    {return this->children.size() >= Min;}

    bool           is_leaf;
    std::size_t    parent;
    container_type entry;
    aabb_type      box;
};

} // detail
} // perior
#endif//PERIOR_TREE_NODE_HPP
