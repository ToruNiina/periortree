#ifndef PERIOR_TREE_RTREE_NODE_HPP
#define PERIOR_TREE_RTREE_NODE_HPP
#include <periortree/config.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/variant.hpp>

namespace perior
{
namespace detail
{

template<typename boxT, std::size_t Min, std::size_t Max>
struct internal_node
{
    static BOOST_CONSTEXPR_OR_CONST std::size_t max_entry = Max;
    static BOOST_CONSTEXPR_OR_CONST std::size_t min_entry = Min;

    typedef boost::container::static_vector<std::size_t, Max>
            children_container_type;
    typedef children_container_type::iterator       children_iterator;
    typedef children_container_type::const_iterator children_const_iterator;

    internal_node(const std::size_t parent_): parent(parent_){}
    ~internal_node(){}

    BOOST_FORCEINLINE bool has_enough_storage() const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return this->children.size() < Max;
    }
    BOOST_FORCEINLINE bool has_enough_entry() const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return this->children.size() >= Min;
    }

    std::size_t             parent;
    children_container_type children;
    boxT                    box;
};

template<typename boxT, std::size_t Min, std::size_t Max>
struct leaf_node
{
    static BOOST_CONSTEXPR_OR_CONST std::size_t max_entry = Max;
    static BOOST_CONSTEXPR_OR_CONST std::size_t min_entry = Min;

    typedef boost::container::static_vector<std::size_t, Max>
            entry_container_type;
    typedef entry_container_type::iterator       entry_iterator;
    typedef entry_container_type::const_iterator entry_const_iterator;

    explicit leaf_node(const std::size_t parent_): parent(parent_) {}
    ~leaf_node(){};

    BOOST_FORCEINLINE bool has_enough_storage() const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return this->children.size() < Max;
    }
    BOOST_FORCEINLINE bool has_enough_entry()   const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return this->children.size() >= Min;
    }

    std::size_t          parent;
    entry_container_type entries;
    boxT                 box;
};

template<typename boxT, std::size_t Min, std::size_t Max>
struct rtree_node
{
    typedef boxT box_type;
    typedef internal_node_type<boxT, Min, Max> internal_node_type;
    typedef leaf_node_type<boxT, Min, Max>     leaf_node_type;

    struct box_visitor : boost::static_visitor<box_type&>
    {
        BOOST_FORCEINLINE box_type&
        operator()(leaf_node_type& n)     const BOOST_NOEXCEPT_OR_NOTHROW
        {return n.box;}

        BOOST_FORCEINLINE box_type&
        operator()(internal_node_type& n) const BOOST_NOEXCEPT_OR_NOTHROW
        {return n.box;}
    };
    struct box_const_visitor : boost::static_visitor<box_type const&>
    {
        BOOST_FORCEINLINE box_type const&
        operator()(const leaf_node_type& n)     const BOOST_NOEXCEPT_OR_NOTHROW
        {return n.box;}

        BOOST_FORCEINLINE box_type const&
        operator()(const internal_node_type& n) const BOOST_NOEXCEPT_OR_NOTHROW
        {return n.box;}
    };

    template<typename nodeT>
    explicit rtree_node(nodeT const& n): node_(n){}
    template<typename nodeT>
    explicit rtree_node& operator=(nodeT const& n){node_ = n;}

    rtree_node(rtree_node const& rhs): node_(rhs.node_){}
    rtree_node& operator=(rtree_node const& rhs){node_ = rhs.node_;}

    BOOST_FORCEINLINE bool is_leaf()     const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return node_.which() == 0;
    }
    BOOST_FORCEINLINE bool is_internal() const BOOST_NOEXCEPT_OR_NOTHROW
    {
        return node_.which() == 1;
    }

    BOOST_FORCEINLINE leaf_node_type& as_leaf()
    {
        return boost::get<leaf_node_type>(node_);
    }
    BOOST_FORCEINLINE internal_node_type& as_internal()
    {
        return boost::get<internal_node_type>(node_);
    }

    BOOST_FORCEINLINE leaf_node_type const&     as_leaf()     const
    {
        return boost::get<leaf_node_type>(node_);
    }
    BOOST_FORCEINLINE internal_node_type const& as_internal() const
    {
        return boost::get<internal_node_type>(node_);
    }

    BOOST_FORCEINLINE box_type const& box() const
    {
        return boost::apply_visitor(box_const_visitor(), node_);
    }
    BOOST_FORCEINLINE box_type& box()
    {
        return boost::apply_visitor(box_visitor(), node_);
    }

    boost::variant<leaf_node_type, internal_node_type> node_;
};

}// detail
}// perior
#endif//PERIOR_TREE_RTREE_NODE_HPP
