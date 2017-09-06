#ifndef PERIOR_TREE_RTREE_HPP
#define PERIOR_TREE_RTREE_HPP
#include "rtree_node.hpp"
#include "expand.hpp"
#include "area.hpp"
#include <boost/container/vector.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/optional.hpp>
#include <utility>
#include <memory>
#include <functional>
#include <limits>
#include <tuple>

namespace perior
{

namespace detail
{
template<typename T>
struct indexable_type_of
{
    typedef typename std::tuple_element<0, T>::type type;

    static inline type const&
    invoke(const T& t) noexcept {return std::get<0>(t);}

    static inline type&
    invoke(T& t)       noexcept {return std::get<0>(t);}

    static inline type&&
    invoke(T&& t)      noexcept {return std::get<0>(t);}
};
} // detail

template<std::size_t Min, std::size_t Max>
struct quadratic
{
    static constexpr std::size_t min_elem = Min;
    static constexpr std::size_t max_elem = Max;
};

template<typename T, std::size_t N>
aabb<T, N> make_aabb(const aabb<T, N>& b) noexcept
{
    return b;
}

template<typename T, std::size_t N>
aabb<T, N> make_aabb(const point<T, N>& p) noexcept
{
    return aabb<T, N>(p, p);
}

template<typename T,
         typename Params,
         typename Boundary,
         typename EqualTo   = std::equal_to<T>,
         typename Allocator = boost::container::new_allocator<T> >
class rtree
{
  public:
    typedef T         value_type;
    typedef Params    parameter_type;
    typedef Boundary  boundary_type;
    typedef EqualTo   equal_to_type;
    typedef Allocator allocator_type;

    typedef detail::indexable_type_of<value_type> indexable;
    typedef typename indexable::type              indexable_type;
    typedef typename indexable_type::scalar_type  scalar_type;
    static constexpr std::size_t dim = indexable_type::dim;
    typedef point<scalar_type, dim> point_type;
    typedef aabb<scalar_type, dim>  aabb_type;

    static constexpr std::size_t min_entry = parameter_type::min_elem;
    static constexpr std::size_t max_entry = parameter_type::max_elem;

    typedef boost::container::vector<value_type, allocator_type> container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    typedef detail::rtree_node<scalar_type, dim, min_entry, max_entry> node_type;
    typedef typename allocator_type::template rebind<node_type>::other
            node_allocator_type;
    typedef boost::container::vector<node_type, node_allocator_type> tree_type;
    typedef boost::container::small_vector<std::size_t, 8> index_buffer_type;

  private:

    static constexpr std::size_t nil = std::numeric_limits<std::size_t>::max();

  public:

    rtree(): root_(nil){}
    ~rtree() = default;
    rtree(const rtree&) = default;
    rtree(rtree&&)      = default;
    rtree& operator=(const rtree&) = default;
    rtree& operator=(rtree&&)      = default;

    explicit rtree(const boundary_type& b): boundary_(b){}
    explicit rtree(const equal_to_type& e): equal_to_(e){}
    rtree(const boundary_type& b, const equal_to_type& e)
        : equal_to_(e), boundary_(b)
    {}

    std::size_t size() const noexcept {return container_.size();}
    bool empty()       const noexcept {return container_.empty();}
    void clear()
    {
        this->root_ = nil;
        this->tree_.clear();
        this->container_.clear();
        this->overwritable_nodes_.clear();
        this->overwritable_values_.clear();
        return;
    }

    void insert(const value_type& v)
    {
        const std::size_t     idx   = this->add_value(v);
        const indexable_type& entry = indexable::invoke(v);
        const std::size_t L = this->choose_leaf(entry);

        if(tree_.at(L).has_enough_storage())
        {
            tree_.at(L).entry.push_back(idx);
            expand(tree_.at(L).box, entry, this->boundary_);
            this->adjust_tree(L);
        }
        else
        {
            const std::size_t LL = this->add_node(this->split_leaf(L, idx, entry));
            this->adjust_tree(L, LL);
        }
        return;
    }
    bool remove(const value_type& v)
    {
        if(this->root_ == nil)
        {
            return false;
        }
        if(boost::optional<std::pair<std::size_t, typename node_type::const_iterator>>
                found = this->find_leaf(this->root_, v))
        {
            const std::size_t node_idx  = found->first;
            const std::size_t value_idx = *(found->second);
            this->tree_.at(node_idx).entries.erase(found->second);
            this->erase_value(value_idx);
            this->condense_tree(node_idx);
            return true;
        }
        return false;
    }

    template<typename Query, typename OutputIterator>
    void query(Query q, OutputIterator out) const
    {
        if(this->root_ == nil){return;}
        return query_impl(this->root_, q, out);
    }

  private:

    std::size_t choose_leaf(const indexable_type& entry)
    {
        std::size_t node_idx = this->root_;
        while(!(this->tree_.at(node_idx).is_leaf))
        {
            scalar_type diff_area_min = std::numeric_limits<scalar_type>::max();
            scalar_type area_min      = std::numeric_limits<scalar_type>::max();

            const node_type& node = this->tree_.at(node_idx);
            for(typename node_type::const_iterator
                    i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
            {
                aabb_type box = tree_.at(*i).box;
                const scalar_type area_initial = area(box);

                expand(box, entry, this->boundary_);

                const scalar_type area_expanded = area(box);
                const scalar_type diff_area     = area_expanded - area_initial;
                if(diff_area < diff_area_min ||
                   (diff_area == diff_area_min && area_expanded < area_min))
                {
                    node_idx = *i;
                    diff_area_min = diff_area;
                    area_min = area_expanded;
                }
            }
        }
        return node_idx;
    }

    void adjust_tree(std::size_t node_idx)
    {
        while(tree_.at(node_idx).parent != nil)
        {
            const node_type& node = tree_.at(node_idx);
            node_type& parent_ = tree_.at(node.parent);
            expand(parent_.box, node.box, this->boundary_);

            node_idx = node.parent;
        }
        return;
    }
    void adjust_tree(const std::size_t N, const std::size_t NN)
    {
        if(tree_.at(N).parent == nil) // grow tree taller
        {
            node_type new_root(false, nil);
            new_root.entry.push_back(N);
            new_root.entry.push_back(NN);
            new_root.box = tree_.at(N).box;
            expand(new_root.box, tree_.at(NN).box, this->boundary_);
            this->root_ = this->add_node(new_root);
            return;
        }
        else
        {
            const node_type& node    = tree_.at(N);
            const node_type& partner = tree_.at(NN);
            node_type& parent_ = tree_.at(node.parent);
            expand(parent_.box, node.box, this->boundary_);
            if(parent_.has_enough_storage())
            {
                expand(parent_.box, partner.box, this->boundary_);
                parent_.entry.push_back(NN);
                return this->adjust_tree(node.parent);
            }
            else
            {
                const node_type parent_partner = this->split_node(node.parent, NN);
                const std::size_t PP = this->add_node(parent_partner);
                return this->adjust_tree(node.parent, PP);
            }
        }
    }

    boost::optional<std::pair<std::size_t, typename node_type::const_iterator>>
    find_leaf(std::size_t node_idx, const indexable_type& entry) const
    {
        const node_type& node = tree_.at(node_idx);
        if(node.is_leaf)
        {
            for(typename node_type::const_iterator
                    i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
            {
                if(equal_to_(container_.at(*i), entry))
                {
                    return std::make_pair(node_idx, i);
                }
            }
            return boost::none;
        }
        else
        {
            for(typename node_type::const_iterator
                    i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
            {
                boost::optional<std::pair<std::size_t, typename node_type::const_iterator>
                    > found = this->find_leaf(*i, entry);
                if(found)
                {
                    return found;
                }
            }
            return boost::none;
        }
    }

    void condense_tree(const std::size_t N)
    {
        std::size_t node_idx = N;
        boost::container::small_vector<std::size_t, 8> eliminated;

        // check whether the node should be eliminated
        while(!(tree_.at(node_idx).has_enough_entry()) &&
                tree_.at(node_idx).parent != nil)
        {
            const std::size_t parent_idx = tree_.at(node_idx).parent;

            node_type& parent = tree_.at(parent_idx);
            typename node_type::const_iterator found = std::find(
                    parent.entry.begin(), parent.entry.end(), node_idx);
            assert(found != parent.entry.end());
            parent.entry.erase(found);

            eliminated.push_back(node_idx);
            node_idx = parent_idx;
        }

        // re-insert eliminated nodes
        for(boost::container::small_vector<std::size_t, 8>::const_iterator
                i(eliminated.begin()), e(eliminated.end()); i != e; ++i)
        {
            this->re_insert(*i);
        }
        return;
    }

    // split nodes because of one entry
    node_type split_leaf(const std::size_t N,
                         const std::size_t vidx, const indexable_type& entry)
    {
        node_type& node = tree_.at(N);
        node_type  partner(true, node.parent);

        boost::container::static_vector<std::pair<std::size_t, indexable_type>,
            max_entry+1> entries;
        entries.push_back(std::make_pair(vidx, entry));

        for(typename node_type::const_iterator
                i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
        {
            entries.push_back(std::make_pair(
                        *i, indexable::invoke(container_.at(*i))));
        }
        node.entry.clear();
        partner.entry.clear(); // for make it sure

        /* assign first 2 entries to node and partner */{
            const std::array<std::size_t, 2> seeds = this->pick_seeds(entries);
               node.entry.push_back(entries.at(seeds[0]).first);
            partner.entry.push_back(entries.at(seeds[1]).first);

               node.box = make_aabb(entries.at(seeds[0]).second);// TODO:
            partner.box = make_aabb(entries.at(seeds[1]).second);//   make_aabb

            // remove them from entries pool
            entries.erase(entries.begin() + seeds[0]);
            entries.erase(entries.begin() + seeds[1]);
        }

        std::size_t    node_requires = node_type::min_entry - 1;
        std::size_t partner_requires = node_type::min_entry - 1;
        while(!entries.empty())
        {
            if(node_requires >= entries.size())
            {
                for(auto i(entries.begin()), e(entries.end()); i != e; ++i)
                {
                    node.entry.push_back(i->first);
                    expand(node.box, i->second);
                }
                return partner;
            }
            if(partner_requires >= entries.size())
            {
                for(auto i(entries.begin()), e(entries.end()); i != e; ++i)
                {
                    partner.entry.push_back(i->first);
                    expand(partner.box, i->second);
                }
                return partner;
            }

            const std::pair<std::size_t, bool> next =
                this->pick_next(entries, node.box, partner.box);
            if(next.second) // next is partner
            {
                node.entry.push_back(entries.at(next.first).first);
                expand(node.box,     entries.at(next.first).second);
            }
            else // next is for partner
            {
                partner.entry.push_back(entries.at(next.first).first);
                expand(partner.box,     entries.at(next.first).second);
            }
            entries.erase(entries.begin() + next.first);
        }
        return partner;
    }

    // now linear algorithm
    // objT should be indexable_type or aabb_type
    template<typename objT>
    std::array<std::size_t, 2> pick_seeds(const boost::container::static_vector<
            std::pair<std::size_t, objT>, max_entry+1>& entries)
    {
        assert(entries.size() >= 2);
        std::array<std::size_t, 2> retval;
        retval[0] = 0;
        retval[1] = 1;
        return retval;
    }

    // now linear algorithm
    // objT should be indexable_type or aabb_type
    template<typename objT>
    std::pair<std::size_t, bool> pick_next(const boost::container::static_vector<
            std::pair<std::size_t, objT>, max_entry+1>& entries,
            const aabb_type& node, const aabb_type& ptnr)
    {
        assert(!entries.size().empty);
        const objT& entry = entries.front().second;
        aabb_type node_expand(node);
        aabb_type ptnr_expand(ptnr);
        expand(node_expand,    entry, this->boundary_);
        expand(ptnr_expand, entry, this->boundary_);
        const scalar_type diff_area_node =
            area(node_expand, boundary_) - area(node, this->boundary_);
        const scalar_type diff_area_ptnr =
            area(ptnr_expand, boundary_) - area(ptnr, this->boundary_);

        if(diff_area_node < diff_area_ptnr)
        {
            return std::make_pair(0, true);
        }
        else
        {
            return std::make_pair(0, false);
        }
    }

    // split nodes because of new node NN by quadratic algorithm
    node_type split_node(const std::size_t P, const std::size_t NN)
    {
        node_type const& entry_node = tree_.at(NN);
        node_type& node = tree_.at(P);          // entries will be copied
        node_type  partner(false, node.parent); // has no entry

        boost::container::static_vector<std::pair<std::size_t, aabb_type>,
            max_entry+1> entries;
        entries.push_back(std::make_pair(NN, entry_node.box));

        for(typename node_type::const_iterator
                i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
        {
            entries.push_back(std::make_pair(*i, tree_.at(*i).box));
        }
        node.entry.clear();
        partner.entry.clear(); // for make it sure

        /* assign first 2 entries to node and partner */{
            const std::array<std::size_t, 2> seeds = this->pick_seeds(entries);
               node.entry.push_back(entries.at(seeds[0]).first);
            partner.entry.push_back(entries.at(seeds[1]).first);

               node.box = entries.at(seeds[0]).second;// these are AABB.
            partner.box = entries.at(seeds[1]).second;

            // remove them from entries pool
            entries.erase(entries.begin() + seeds[0]);
            entries.erase(entries.begin() + seeds[1]);
        }

        std::size_t    node_requires = node_type::min_entry - 1;
        std::size_t partner_requires = node_type::min_entry - 1;
        while(!entries.empty())
        {
            if(node_requires >= entries.size())
            {
                for(auto i(entries.begin()), e(entries.end()); i != e; ++i)
                {
                    node.entry.push_back(i->first);
                    expand(node.box, i->second);
                }
                return partner;
            }
            if(partner_requires >= entries.size())
            {
                for(auto i(entries.begin()), e(entries.end()); i != e; ++i)
                {
                    partner.entry.push_back(i->first);
                    expand(partner.box, i->second);
                }
                return partner;
            }

            const std::pair<std::size_t, bool> next =
                this->pick_next(entries, node.box, partner.box);
            if(next.second) // next is partner
            {
                node.entry.push_back(entries.at(next.first).first);
                expand(node.box,     entries.at(next.first).second);
            }
            else // next is for partner
            {
                partner.entry.push_back(entries.at(next.first).first);
                expand(partner.box,     entries.at(next.first).second);
            }
            entries.erase(entries.begin() + next.first);
        }
        return partner;

    }

    template<typename Query, typename OutputIterator>
    void query_impl(std::size_t node_idx, Query q, OutputIterator out) const
    {
        const node_type& node = tree_.at(node_idx);
        if(node.is_leaf)
        {
            for(typename node_type::const_iterator
                i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
            {
                value_type const& val = container_.at(*i);
                if(q.match(indexable::invoke(val)) && q.match(val))
                {
                    *out = val;
                    ++out;
                }
            }
        }
        else
        {
            for(typename node_type::const_iterator
                i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
            {
                const std::size_t next = *i;
                if(q.match(tree_.at(next).box))
                {
                    this->query_impl(next, q, out);
                }
            }
        }
        return;
    }

  private:

    std::size_t level_of(std::size_t node_idx) const
    {
        std::size_t level = 0;
        while(!(tree_.at(node_idx).is_leaf))
        {
            ++level;
            node_idx = tree_.at(node_idx).entry.front();
        }
        return level;
    }

    void re_insert(const std::size_t N)
    {
        // insert node to its proper parent. to find the parent of this node N,
        // add 1 to level. root node should NOT come here.
        const std::size_t lvl = level_of(N) + 1;
        const aabb_type&  entry = tree_.at(N).box;
        const std::size_t L = choose_node_with_level(entry, lvl);

        if(tree_.at(L).has_enough_storage())
        {
            tree_.at(L).entry.push_back(N);
            expand(tree_.at(L).box, entry, this->boundary_);
            this->adjust_tree(L);
        }
        else
        {
            const std::size_t LL = this->add_node(this->split_node(L, N));
            this->adjust_tree(L, LL);
        }
        return;
    }

    std::size_t choose_node_with_level(const aabb_type& entry, const std::size_t lvl)
    {
        std::size_t node_idx = this->root_;
        if(level_of(this->root_) < lvl)
        {
            throw std::logic_error("root is under the node");
        }

        while(level_of(node_idx) != lvl)
        {
            scalar_type diff_area_min = std::numeric_limits<scalar_type>::max();
            scalar_type area_min      = std::numeric_limits<scalar_type>::max();

            const node_type& node = this->tree_.at(node_idx);
            for(typename node_type::const_iterator
                    i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
            {
                aabb_type box = tree_.at(*i).box;
                const scalar_type area_initial = area(box);

                expand(box, entry, this->boundary_);

                const scalar_type area_expanded = area(box);
                const scalar_type diff_area     = area_expanded - area_initial;
                if(diff_area < diff_area_min ||
                   (diff_area == diff_area_min && area_expanded < area_min))
                {
                    node_idx = *i;
                    diff_area_min = diff_area;
                    area_min = area_expanded;
                }
            }
        }
        return node_idx;
    }

  private:

    std::size_t add_value(const value_type& v)
    {
        if(overwritable_values_.empty())
        {
            const std::size_t idx = container_.size();
            container_.push_back(v);
            return idx;
        }
        else
        {
            const std::size_t idx = overwritable_values_.back();
            overwritable_values_.pop_back();
            container_.at(idx) = v;
            return idx;
        }
    }
    void erase_value(const std::size_t i)
    {
        overwritable_values_.push_back(i);
        return;
    }

    std::size_t add_node(const node_type& n)
    {
        if(overwritable_nodes_.empty())
        {
            const std::size_t idx = tree_.size();
            tree_.push_back(n);
            return idx;
        }
        else
        {
            const std::size_t idx = overwritable_nodes_.back();
            overwritable_nodes_.pop_back();
            tree_.at(idx) = n;
            return idx;
        }
    }
    void erase_node(const std::size_t i)
    {
        overwritable_nodes_.push_back(i);
        return;
    }

  private:

    std::size_t       root_;
    equal_to_type     equal_to_;
    boundary_type     boundary_;
    tree_type         tree_;
    container_type    container_;
    index_buffer_type overwritable_values_;
    index_buffer_type overwritable_nodes_;
};


} // perior
#endif//PERIOR_TREE_RTREE_HPP
