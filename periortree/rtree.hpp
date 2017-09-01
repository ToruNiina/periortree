#ifndef PERIOR_TREE_RTREE_HPP
#define PERIOR_TREE_RTREE_HPP
#include <periortree/aabb_traits.hpp>
#include <periortree/expand.hpp>
#include <periortree/area.hpp>
#include <periortree/rtree_node.hpp>

#include <boost/container/vector.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/tuples/tuple.hpp>
#include <boost/optional.hpp>
#include <boost/array.hpp>

#include <functional>
#include <memory>

namespace perior
{

template<typename std::size_t Min, std::size_t Max>
struct quadratic
{
    static BOOST_CONSTEXPR_OR_CONST std::size_t min_elem = Min;
    static BOOST_CONSTEXPR_OR_CONST std::size_t max_elem = Max;
    // now implemented quadratic algorithm only...
    // split_node();
};

template<typename T, typename Params, typename ToAABB, typename Boundary,
         typename EqualTo   = std::equal_to<T>,
         typename Allocator = std::allocator<T> >
class rtree
{
  public:
    typedef T         entry_type;
    typedef Params    parameter_type;
    typedef ToAABB    to_aabb_type;
    typedef Boundary  boundary_type;
    typedef EqualTo   equal_to_type;
    typedef Allocator allocator_type;

    typedef typename to_aabb::box_type                          box_type;
    typedef typename traits::point_type_of<box_type>::type      point_type;
    typedef typename traits::coordinate_type_of<box_type>::type coordinate_type;

    typedef std::pair<box_type, entry_type> value_type;
    typedef boost::container::vector<value_type, allocator_type> container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    static BOOST_CONSTEXPR_OR_CONST std::size_t NIL = std::numeric_limits<std::size_t>::max();
    static BOOST_CONSTEXPR_OR_CONST std::size_t min_entry = parameter_type::min_elem;
    static BOOST_CONSTEXPR_OR_CONST std::size_t max_entry = parameter_type::max_elem;

    typedef detail::rtree_node<box_type, min_entry, max_entry> node_type;
    typedef typename node_type::internal_node_type         internal_node_type;
    typedef typename node_type::leaf_node_type             leaf_node_type;
    typedef boost::container::vector<node_type>            tree_type;
    typedef boost::container::small_vector<std::size_t, 8> index_buffer_type;

  public:

    rtree(): root_(NIL){}
    explicit rtree(const boundary_type& b): root_(NIL), boundary_(b){}
    ~rtree(){};

    void insert(const entry_type& v)
    {
        const std::size_t idx = container_.size();
        const box_type    box = to_aabb::invoke(v);

        container_.push_back(std::make_pair(box, v));

        leaf_node_type& L = this->choose_leaf(box);
        if(L.has_enough_storage())
        {
            L.entries.push_back(idx);
            this->adjust_tree(L);
        }
        else
        {
            const leaf_node_type LL = this->split_node(L, box);
            this->adjust_tree(L, LL);
        }
        return;
    }
    void remove(const entry_type& v)
    {
        typedef std::pair<leaf_node_type&, leaf_node_type::entry_iterator>
                found_leaf_node_type;

        if(boost::optional<found_leaf_node_type> found = this->find_leaf(v))
        {
            found_leaf_node_type& L = *found;

            const std::size_t idx = *(L.second);
            L.entries.erase(L.second);

            container_.erase(container_.begin() + idx);
            this->condense_tree(L.first);
        }
        return;
    }

    template<typename Query, typename OutputIterator>
    void query(Query q, OutputIterator out) const
    {
        if(this->root_ == NIL){return;}
        return query_impl(this->container_.at(this->root_), q, out);
    }

    std::size_t size() const throw() {return container_.size();}
    bool empty()       const throw() {return container_.empty();}
    void clear(){root_ = NIL; tree_.clear(); container_.clear(); empty_nodes_.clear();}

  private:

    leaf_node_type& choose_leaf(const box_type& bx)
    {
        if(this->root_ == NIL)
        {
            const node_type root(leaf_node_type(NIL));
            tree_.push_back(root);
            return this->tree_.front();
        }
        return choose_leaf_impl(this->tree_.at(this->root_), bx);
    }

    leaf_node_type& choose_leaf_impl(const node_type& node, const box_type& bx)
    {
        if(node.is_leaf())
        {
            return node;
        }
        internal_node_type const& internal = node.as_internal();
        std::size_t F     = NIL;
        coord_type  darea = std::numeric_limits<coord_type>::max();
        coord_type  area  = std::numeric_limits<coord_type>::max();

        // find entry to insert the box `bx`
        for(typename internal_node_type::children_iterator
            i(internal.children.begin()), e(internal.children.end()); i != e; ++i)
        {
            const box_type&  EI  = this->tree_.at(*i).box();
            const coord_type da_ = calc_area_enlargement(EI, bx);
            if(da_ < darea)
            {
                F     = *i;
                darea = da_;
                area  = calc_area(EI);
            }
            else if(da_ == darea) // unlikely
            {
                const coord_type a_ = calc_area(EI);
                if(a_ < area)
                {
                    F     = *i;
                    darea = da_;
                    area  = calc_area(EI);
                }
            }
        }
        return choose_leaf_impl(this->tree_.at(F), bx);
    }

    coordinate_type
    calc_area_enlargement(const box_type& bx, const box_type& entry)
    {
        box_type expanded(bx); expand(expanded, entry, boundary_);
        return area(expanded, boundary_) - area(bx, boundary_);
    }

    boost::optional<std::pair<leaf_node_type&,
        typename leaf_node_type::entry_container_type::iterator> >
    find_leaf(const vlaue_type& v);

    void adjust_tree(leaf_node_type& L);
    void adjust_tree(leaf_node_type& L, const leaf_node_type& LL);

    void condense_tree(leaf_node_type&);
    void condense_tree(internal_node_type&);

    leaf_node_type     split_node(leaf_node_type& nd,     const box_type& bx);
    internal_node_type split_node(internal_node_type& nd, const box_type& bx);

    template<typename Query, typename OutputIterator>
    void query_impl(const node_type& node, Query q, OutputIterator out)
    {
        if(node.is_leaf())
        {
            leaf_node_type const& leaf = node.as_leaf();
            for(typename leaf_node_type::entry_const_iterator
                i(leaf.entries.begin()), e(leaf.entries.end()); i != e; ++i)
            {
                value_type const& val = container_.at(*i);
                // match both bounding-box and property of value_type itself
                // ex) query: all the "red particle" intersecting with this box
                if(q.match(val.first) && q.match(val.second))
                {
                    *out = val;
                    ++out;
                }
            }
        }
        else // node is internal node
        {
            internal_node_type const& internal = node.as_internal();
            for(typename internal_node_type::children_const_iterator
                i(internal.children.begin()), e(internal.children.end());
                i != e; ++i)
            {
                const node_type& next = tree_.at(*i);
                if(q.match(next.box()))
                {
                    // search the child node recursively
                    this->query_impl(next, q, out);
                }
            }
        }
        return ;
    }

  private:

    std::size_t       root_;
    to_aabb_type      to_aabb_;
    equal_to_type     equal_to_;
    boundary_type     boundary_;
    tree_type         tree_;
    container_type    container_;
    index_buffer_type empty_nodes_;
};

} // perior
#endif//PERIOR_TREE_RTREE_HPP
