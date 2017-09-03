#ifndef PERIOR_TREE_RTREE_HPP
#define PERIOR_TREE_RTREE_HPP
#include <periortree/aabb_traits.hpp>
#include <periortree/expand.hpp>
#include <periortree/area.hpp>
#include <periortree/rtree_node.hpp>

#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/tuples/tuple.hpp>
#include <boost/optional.hpp>
#include <boost/array.hpp>

#include <functional>
#include <memory>

namespace perior
{

namespace detail
{
template<typename T>
struct indexable_type_of
{
    typedef T type;

    BOOST_FORCEINLINE static type const& invoke(T const& v) {return v;}
    BOOST_FORCEINLINE static type&       invoke(T& v)       {return v;}
};

template<typename T1, typename T2>
struct indexable_type_of<std::pair<T1, T2> >
{
    typedef T1 type;

    BOOST_FORCEINLINE static
    type const& invoke(std::pair<T1, T2> const& v) {return v.first;}
    BOOST_FORCEINLINE static
    type&       invoke(std::pair<T1, T2>& v)       {return v.first;}
};

#ifdef PERIOR_TREE_ENABLE_CXX11

template<typename T0, typename ...Ts>
struct indexable_type_of<std::tuple<T0, Ts...>>
{
    typedef T0 type;
    BOOST_FORCEINLINE static
    type const& invoke(std::tuple<T0, Ts...> const& v) {return std::get<0>(v);}
    BOOST_FORCEINLINE static
    type&       invoke(std::tuple<T0, Ts...>& v)       {return std::get<0>(v);}
};

template<typename T0, typename ...Ts>
struct indexable_type_of<boost::tuple<T0, Ts...>>
{
    typedef T0 type;
    BOOST_FORCEINLINE static
    type const& invoke(boost::tuple<T0, Ts...> const& v) {return boost::get<0>(v);}
    BOOST_FORCEINLINE static
    type&       invoke(boost::tuple<T0, Ts...>& v)       {return boost::get<0>(v);}
};

#else // before c++11...

#define PERIOR_TREE_EXPAND_INDEXABLE_TYPE_OF(z, N, dummy)\
    template<BOOST_PP_ENUM_PARAMS(N, typename T)>\
    struct indexable_type_of<boost::tuple<BOOST_PP_ENUM_PARAMS(N, T)> >\
    {\
        typedef T0 type;\
        BOOST_FORCEINLINE static\
        type const& invoke(boost::tuple<T0, Ts...> const& v)\
        {return boost::get<0>(v);}\
        BOOST_FORCEINLINE static\
        type&       invoke(boost::tuple<T0, Ts...>& v)\
        {return boost::get<0>(v);}\
    };\
    /**/

BOOST_PP_REPEAT_FROM_TO(1, 10, PERIOR_TREE_EXPAND_INDEXABLE_TYPE_OF, BOOST_PP_EMPTY)

#undef PERIOR_TREE_EXPAND_INDEXABLE_TYPE_OF
#endif//PERIOR_TREE_ENABLE_CXX11
} // detail

//XXX: TODO now implemented quadratic algorithm only...
template<typename std::size_t Min, std::size_t Max>
struct quadratic
{
    static BOOST_CONSTEXPR_OR_CONST std::size_t min_elem = Min;
    static BOOST_CONSTEXPR_OR_CONST std::size_t max_elem = Max;
    // split_node();
};

// T = AABB or std::pair<AABB, T> or boost::tuple<AABB, T1, T2, ...>
template<typename T, typename Params, typename Boundary,
         typename EqualTo   = std::equal_to<T>,
         typename Allocator = std::allocator<T> >
class rtree
{
  public:
    typedef T         value_type;
    typedef Params    parameter_type;
    typedef Boundary  boundary_type;
    typedef EqualTo   equal_to_type;
    typedef Allocator allocator_type;

    //TODO now {indexable_type} is aabb only. point is not indexable.
    typedef detail::indexable_type_of<value_type> indexable_t;
    typedef typename indexable_t::type indexable_type;
    BOOST_STATIC_ASSERT_MSG((boost::is_same<traits::aabb_tag,
        typename traits::tag<indexable_type>::type>::value),
        "rtree element type must have an indexable type.")
    typedef typename traits::point_type_of<indexable_type>::type      point_type;
    typedef typename traits::coordinate_type_of<indexable_type>::type coordinate_type;

    typedef boost::container::vector<value_type, allocator_type> container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    static BOOST_CONSTEXPR_OR_CONST std::size_t NIL = std::numeric_limits<std::size_t>::max();
    static BOOST_CONSTEXPR_OR_CONST std::size_t min_entry = parameter_type::min_elem;
    static BOOST_CONSTEXPR_OR_CONST std::size_t max_entry = parameter_type::max_elem;

    typedef detail::rtree_node<indexable_type, min_entry, max_entry> node_type;
    typedef typename node_type::internal_node_type         internal_node_type;
    typedef typename node_type::leaf_node_type             leaf_node_type;
    typedef boost::container::vector<node_type>            tree_type;
    typedef boost::container::small_vector<std::size_t, 8> index_buffer_type;

  public:

    rtree(): root_(NIL){}
    explicit rtree(const boundary_type& b): root_(NIL), boundary_(b){}
    ~rtree(){};

    void insert(const value_type& v)
    {
        const std::size_t    idx = container_.size();
        const indexable_type box = indexable_t::invoke(v);

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
    void remove(const value_type& v)
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
            const box_type&  EI      = this->tree_.at(*i).box();
            const coord_type area_EI = area(EI, this->boundary_);
            box_type expanded(EI);
            expand(expanded, bx, this->boundary_);
            const coord_tyep area_expanded = area(expanded, boundary_);
            const coord_type da_ = area_expanded - area_EI;

            if(da_ < darea || (da_ == darea && area_expanded < area))
            {
                F     = *i;
                darea = da_;
                area  = area_expanded;
            }
        }
        return choose_leaf_impl(this->tree_.at(F), bx);
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
    equal_to_type     equal_to_;
    boundary_type     boundary_;
    tree_type         tree_;
    container_type    container_;
    index_buffer_type empty_nodes_;
};

} // perior
#endif//PERIOR_TREE_RTREE_HPP
