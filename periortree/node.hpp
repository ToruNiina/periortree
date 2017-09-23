#ifndef PERIOR_TREE_NODE_HPP
#define PERIOR_TREE_NODE_HPP
#include <periortree/rectangle.hpp>
#include <periortree/containers.hpp>

namespace perior
{
namespace detail
{

template<typename pointT, std::size_t Min, std::size_t Max>
struct rtree_node
{
    typedef pointT point_type;
    BOOST_STATIC_ASSERT(traits::is_point<point_type>::value);
    typedef rectangle<point_type> aabb_type;
    static const std::size_t dimension = traits::dimension<point_type>::value;
    static const std::size_t max_entry = Max;
    static const std::size_t min_entry = Min;

    typedef typename gen_static_vector<std::size_t, Max>::type container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    rtree_node(const bool is_leaf_, const std::size_t parent_)
        : is_leaf(is_leaf_), parent(parent_)
    {}
    ~rtree_node(){}

    BOOST_FORCEINLINE
    bool has_enough_storage() const BOOST_NOEXCEPT_OR_NOTHROW
    {return this->entry.size() <  max_entry;}

    BOOST_FORCEINLINE
    bool has_enough_entry() const BOOST_NOEXCEPT_OR_NOTHROW
    {return this->entry.size() >= min_entry;}

    bool           is_leaf;
    std::size_t    parent;
    container_type entry;
    aabb_type      box;
};

} // detail
} // perior
#endif//PERIOR_TREE_NODE_HPP
