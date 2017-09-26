#ifndef PERIOR_TREE_RTREE_HPP
#define PERIOR_TREE_RTREE_HPP
#include <periortree/point_traits.hpp>
#include <periortree/rectangle_traits.hpp>
#include <periortree/indexable.hpp>
#include <periortree/node.hpp>
#include <periortree/expand.hpp>
#include <periortree/within.hpp>
#include <periortree/area.hpp>
#include <periortree/to_svg.hpp>
#include <periortree/containers.hpp>

#include <boost/optional.hpp>
#include <limits>

namespace perior
{

template<std::size_t Max, std::size_t Min = Max / 3>
struct quadratic
{
    static constexpr std::size_t min_entry = Min;
    static constexpr std::size_t max_entry = Max;
};

template<typename T,
         typename Params,
         typename Boundary,
         typename IndexableGetter = indexable_getter<T>,
         typename EqualTo         = std::equal_to<T>,
         typename Allocator       = std::allocator<T> >
class rtree
{
  public:
    typedef T               value_type;
    typedef Params          parameter_type;
    typedef Boundary        boundary_type;
    typedef IndexableGetter indexable_getter_type;
    typedef EqualTo         equal_to_type;
    typedef Allocator       allocator_type;

    typedef typename indexable_getter_type::indexable_type        indexable_type;
    typedef typename traits::point_type_of<indexable_type>::type  point_type;
    typedef typename traits::scalar_type_of<indexable_type>::type scalar_type;

    BOOST_STATIC_CONSTEXPR std::size_t dimension = traits::dimension<point_type>::value;
    BOOST_STATIC_CONSTEXPR std::size_t min_entry = parameter_type::min_entry;
    BOOST_STATIC_CONSTEXPR std::size_t max_entry = parameter_type::max_entry;

    typedef boost::container::vector<value_type, allocator_type> container_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    typedef detail::rtree_node<point_type, min_entry, max_entry> node_type;
    typedef typename node_type::aabb_type aabb_type;

    typedef typename allocator_type::template rebind<node_type>::other
            node_allocator_type;
    typedef typename allocator_type::template rebind<std::size_t>::other
            size_t_allocator_type;
    typedef typename gen_vector<node_type, node_allocator_type>::type tree_type;
    typedef typename gen_small_vector<std::size_t, 8, size_t_allocator_type
        >::type index_buffer_type;

    BOOST_STATIC_CONSTEXPR std::size_t nil = std::numeric_limits<std::size_t>::max();

  public:

    rtree(): root_(nil){}
    ~rtree(){}
    rtree(const rtree& rhs)
        : root_(rhs.root_), equal_to_(rhs.equal_to_), boundary_(rhs.boundary_),
          tree_(rhs.tree_), container_(rhs.container_),
          overwritable_values_(rhs.overwritable_values_),
          overwritable_nodes_(rhs.overwritable_nodes_)
    {}
    rtree& operator=(const rtree& rhs)
    {
        root_      = rhs.root_;
        equal_to_  = rhs.equal_to_;
        boundary_  = rhs.boundary_;
        tree_      = rhs.tree_;
        container_ = rhs.container_;
        overwritable_values_ = rhs.overwritable_values_;
        overwritable_nodes_  = rhs.overwritable_nodes_;
        return *this;
    }

    explicit rtree(const boundary_type& b): root_(nil), boundary_(b){}
    explicit rtree(const equal_to_type& e): root_(nil), equal_to_(e){}
    rtree(const boundary_type& b, const equal_to_type& e)
        : root_(nil), equal_to_(e), boundary_(b)
    {}

    std::size_t size() const BOOST_NOEXCEPT_OR_NOTHROW {return container_.size();}
    bool empty()       const BOOST_NOEXCEPT_OR_NOTHROW {return this->root_ == nil;}
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
        const indexable_type& entry = indexable_getter_(v);
        const std::size_t     L     = this->choose_leaf(entry);

        if(tree_.at(L).has_enough_storage())
        {
            tree_.at(L).entry.push_back(idx);
            tree_.at(L).box = expand(tree_.at(L).box, entry, this->boundary_);
            this->adjust_tree(L);
        }
        else
        {
            const std::size_t LL = this->add_node(this->split_leaf(L, idx, entry));
            this->adjust_tree(L, LL);
        }
        return;
    }
    // if found, erase and return true. if not found, return false.
    bool remove(const value_type& v)
    {
        if(this->root_ == nil)
        {
            return false;
        }
        boost::optional<
            std::pair<std::size_t, typename node_type::const_iterator>
            > found = this->find_leaf(this->root_, v);
        if(found)
        {
            const std::size_t node_idx  =   found->first;
            const std::size_t value_idx = *(found->second);
            this->tree_.at(node_idx).entry.erase(found->second);
            this->erase_value(value_idx);
            this->condense_box(this->tree_.at(node_idx));
            this->condense_leaf(node_idx);
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

    std::ostream& dump(std::ostream& os) const
    {
        if(this->empty()){return os;}

        std::vector<std::string> colors; colors.reserve(3);
        colors.push_back("red");
        colors.push_back("green");
        colors.push_back("blue");
        return dump_node(os, this->root_, 0, colors);
    }

  private:

    std::ostream& dump_node(std::ostream& os,
              const std::size_t N, const std::size_t depth,
              const std::vector<std::string>& clrs) const
    {
        const std::string& clr = clrs.at(depth % 3);
        const node_type& node = tree_.at(N);
        to_svg(os, node.box, this->boundary_, clr, 5, "none");
        os << '\n';
        if(node.is_leaf)
        {
            for(typename node_type::const_iterator
                    i(node.entry.cbegin()), e(node.entry.cend()); i!=e; ++i)
            {
                to_svg(os, indexable_getter_(this->container_.at(*i)),
                       this->boundary_, "black", 1, "black");
                os << '\n';
            }
            return os;
        }
        else // internal node
        {
            for(typename node_type::const_iterator
                    i(node.entry.cbegin()), e(node.entry.cend()); i!=e; ++i)
            {
                this->dump_node(os, *i, depth+1, clrs);
            }
            return os;
        }
    }

    std::size_t choose_leaf(const indexable_type& entry)
    {
        if(this->root_ == nil)
        {
            node_type n(true, nil);
            n.box = make_aabb(entry);
            this->root_ = this->add_node(n);

            return this->root_;
        }

        // choose a leaf to insert
        // so if root is a leaf, return it
        std::size_t node_idx = this->root_;
        while(!(this->tree_.at(node_idx).is_leaf))
        {
            // find minimum expansion
            scalar_type diff_area_min = std::numeric_limits<scalar_type>::max();
            scalar_type area_min      = std::numeric_limits<scalar_type>::max();

            const node_type& node = this->tree_.at(node_idx);
            for(typename node_type::const_iterator
                    i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
            {
                const scalar_type area_initial
                    = area(this->tree_.at(*i).box, this->boundary_);

                const scalar_type area_expanded
                    = area(expand(this->tree_.at(*i).box, entry, this->boundary_),
                           this->boundary_);

                const scalar_type diff_area = area_expanded - area_initial;
                if((diff_area <  diff_area_min) ||
                   (diff_area == diff_area_min  && area_expanded < area_min))
                {
                    node_idx = *i;
                    diff_area_min = diff_area;
                    area_min      = std::min(area_min, area_expanded);
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
            parent_.box = expand(parent_.box, node.box, this->boundary_);
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
            new_root.box = expand(tree_.at(N).box, tree_.at(NN).box, this->boundary_);
            this->root_ = this->add_node(new_root);

            this->tree_.at(N).parent = this->root_;
            this->tree_.at(NN).parent = this->root_;
            return;
        }
        else
        {
            const node_type& node    = tree_.at(N);
            const node_type& partner = tree_.at(NN);
            assert(node.parent == partner.parent);

            node_type& parent_ = tree_.at(node.parent);
            parent_.box = expand(parent_.box, node.box, this->boundary_); // for N

            if(parent_.has_enough_storage())
            {
                parent_.box = expand(parent_.box, partner.box, this->boundary_); // for NN
                parent_.entry.push_back(NN);
                return this->adjust_tree(node.parent);
            }
            else
            {
                const std::size_t PP = this->split_node(node.parent, NN);
                return this->adjust_tree(node.parent, PP);
            }
        }
    }

    boost::optional<std::pair<std::size_t, typename node_type::const_iterator> >
    find_leaf(std::size_t node_idx, const value_type& entry) const
    {
        const node_type& node = tree_.at(node_idx);
        if(within(indexable_getter_(entry), node.box, this->boundary_) == false)
        {
            return boost::none;
        }

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
                if(!within(indexable_getter_(entry), tree_.at(*i).box, this->boundary_))
                {
                    continue;
                }

                boost::optional<
                    std::pair<std::size_t, typename node_type::const_iterator>
                    > found = this->find_leaf(*i, entry);
                if(found)
                {
                    return found;
                }
            }
            return boost::none;
        }
    }

    void condense_leaf(const std::size_t N)
    {
        const node_type& node = this->tree_.at(N);
        assert(node.is_leaf);

        if(node.has_enough_entry() || node.parent == nil)
        {
            return;
        }

        // copy index of objects
        typedef typename gen_small_vector<std::size_t, min_entry>::type temporal_vec_type;
        temporal_vec_type eliminated_objs;
        std::copy(node.entry.begin(), node.entry.end(),
                  std::back_inserter(eliminated_objs));

        // erase the node N from its parent and condense aabb
        typename node_type::iterator found = std::find(
                this->tree_.at(node.parent).entry.begin(),
                this->tree_.at(node.parent).entry.end(), N);
        assert(found != this->tree_.at(node.parent).entry.end());
        this->tree_.at(node.parent).entry.erase(found);
        this->condense_box(this->tree_.at(node.parent));

        // re-insert entries eliminated from node N
        for(typename temporal_vec_type::const_iterator
                i(eliminated_objs.begin()), e(eliminated_objs.end()); i!=e; ++i)
        {
            this->insert(this->container_.at(*i));
        }

        // condense ancester nodes...
        condense_node(node.parent);
        return;
    }

    void condense_node(const std::size_t N)
    {
        const node_type& node = this->tree_.at(N);
        assert(node.is_leaf == false);

        if(node.has_enough_entry())
        {
            return;
        }
        if(node.parent == nil && node.entry.size() == 1)
        {
            this->root_ = node.entry.front();
            this->erase_node(N);
            return;
        }

        // collect index of nodes that are children of the node to be removed
        typedef typename gen_small_vector<std::size_t, min_entry>::type temporal_vec_type;
        temporal_vec_type eliminated_nodes;
        std::copy(node.entry.begin(), node.entry.end(),
                  std::back_inserter(eliminated_nodes));

        // erase the node N from its parent and condense its aabb
        typename node_type::iterator found = std::find(
                this->tree_.at(node.parent).entry.begin(),
                this->tree_.at(node.parent).entry.end(), N);
        assert(found != this->tree_.at(node.parent).entry.end());
        this->tree_.at(node.parent).entry.erase(found);
        this->condense_box(this->tree_.at(node.parent));

        // re-insert nodes eliminated from node N
        for(typename temporal_vec_type::const_iterator
                i(eliminated_nodes.begin()), e(eliminated_nodes.end()); i!=e; ++i)
        {
            this->re_insert(*i);
        }
        this->condense_node(node.parent);
        return;
    }

    // TODO move it to quadratic struct
    node_type split_leaf(const std::size_t N,
                         const std::size_t vidx, const indexable_type& entry)
    {
        node_type& node = tree_.at(N);
        node_type  partner(true, node.parent);

        typedef typename gen_static_vector<std::pair<std::size_t, indexable_type>,
                max_entry+1>::type temporal_entry_container;
        temporal_entry_container entries;
        entries.push_back(std::make_pair(vidx, entry));

        for(typename node_type::const_iterator
                i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
        {
            entries.push_back(std::make_pair(
                        *i, indexable_getter_(container_.at(*i))));
        }
        node.entry.clear();
        partner.entry.clear(); // for make it sure

        /* assign first 2 entries to node and partner */
        {
            const boost::array<std::size_t, 2> seeds =
                this->pick_seeds(entries.begin(), entries.end());
               node.entry.push_back(entries.at(seeds[0]).first);
            partner.entry.push_back(entries.at(seeds[1]).first);

               node.box = make_aabb(entries.at(seeds[0]).second);
            partner.box = make_aabb(entries.at(seeds[1]).second);

            // remove them from entries pool
            entries.erase(entries.begin() + std::max(seeds[0], seeds[1]));
            entries.erase(entries.begin() + std::min(seeds[0], seeds[1]));
        }

        while(!entries.empty())
        {
            if(min_entry > node.entry.size() &&
               min_entry - node.entry.size() >= entries.size())
            {
                for(typename temporal_entry_container::const_iterator
                        i(entries.begin()), e(entries.end()); i != e; ++i)
                {
                    node.entry.push_back(i->first);
                    node.box = expand(node.box, i->second, this->boundary_);
                }
                return partner;
            }
            if(min_entry > partner.entry.size() &&
               min_entry - partner.entry.size() >= entries.size())
            {
                for(typename temporal_entry_container::const_iterator
                        i(entries.begin()), e(entries.end()); i != e; ++i)
                {
                    partner.entry.push_back(i->first);
                    partner.box = expand(partner.box, i->second, this->boundary_);
                }
                return partner;
            }

            const std::pair<std::size_t, bool> next =
                this->pick_next(entries.begin(), entries.end(), node.box, partner.box);
            if(next.second) // next is for node
            {
                node.entry.push_back(entries.at(next.first).first);
                node.box =
                    expand(node.box, entries.at(next.first).second, this->boundary_);
            }
            else // next is for partner
            {
                partner.entry.push_back(entries.at(next.first).first);
                partner.box =
                    expand(partner.box, entries.at(next.first).second, this->boundary_);
            }
            entries.erase(entries.begin() + next.first);
        }
        return partner;
    }

    // ConstIterator::value_type should be
    // std::pair<std::size_t, {indexable_type or aabb_type}>
    template<typename ConstIterator>
    boost::array<std::size_t, 2>
    pick_seeds(const ConstIterator first, const ConstIterator last)
    {
        assert(std::distance(first, last) >= 2);

        boost::array<std::size_t, 2> retval;

        scalar_type max_d = 0;
        for(ConstIterator iter(first), iend(last - 1); iter != iend; ++iter)
        {
            for(ConstIterator jter(iter+1), jend(last); jter != jend; ++jter)
            {
                const aabb_type E1I = make_aabb(iter->second);
                const aabb_type E2I = make_aabb(jter->second);
                const aabb_type J = expand(E1I, E2I, this->boundary_);
                const scalar_type d =
                    area(J, boundary_) - area(E1I, boundary_) - area(E2I, boundary_);
                assert(d >= 0);
                if(max_d < d)
                {
                    max_d = d;
                    retval[0] = std::distance(first, iter);
                    retval[1] = std::distance(first, jter);
                }
            }
        }
        return retval;
    }

    // ConstIterator::value_type should be
    // std::pair<std::size_t, {indexable_type or aabb_type}>
    template<typename ConstIterator>
    std::pair<std::size_t, bool> pick_next(
            const ConstIterator first, const ConstIterator last,
            const aabb_type& node, const aabb_type& ptnr)
    {
        assert(first != last);
        bool is_node;
        std::size_t idx;
        scalar_type max_dd = -1;
        for(ConstIterator iter(first); iter != last; ++iter)
        {
            const aabb_type box1 =
                expand(node, make_aabb(iter->second), this->boundary_);
            const aabb_type box2 =
                expand(ptnr, make_aabb(iter->second), this->boundary_);

            const scalar_type d1 = area(box1, this->boundary_) - area(node, this->boundary_);
            const scalar_type d2 = area(box2, this->boundary_) - area(ptnr, this->boundary_);
            const scalar_type dd = d1 - d2;
            if(max_dd < std::abs(dd))
            {
                max_dd = std::abs(dd);
                idx = std::distance(first, iter);
                is_node = (dd < 0);
            }
        }
        return std::make_pair(idx, is_node);
    }

    // split nodes because of new node NN by quadratic algorithm
    std::size_t split_node(const std::size_t P, const std::size_t NN)
    {
        node_type& node = tree_.at(P);
        const std::size_t PP = this->add_node(node_type(false, node.parent));
        node_type& partner = tree_.at(PP);

        typedef typename gen_static_vector<std::pair<std::size_t, aabb_type>,
                max_entry+1>::type temporal_entry_container;
        temporal_entry_container entries;
        entries.push_back(std::make_pair(NN, tree_.at(NN).box));

        for(typename node_type::const_iterator
                i(node.entry.begin()), e(node.entry.end()); i != e; ++i)
        {
            entries.push_back(std::make_pair(*i, tree_.at(*i).box));

        }
        node.entry.clear();
        partner.entry.clear(); // for make it sure

        /* assign first 2 entries to node and partner */{
            const boost::array<std::size_t, 2> seeds =
                this->pick_seeds(entries.begin(), entries.end());

               node.entry.push_back(entries.at(seeds[0]).first);
            partner.entry.push_back(entries.at(seeds[1]).first);

            tree_.at(entries.at(seeds[0]).first).parent = P;
            tree_.at(entries.at(seeds[1]).first).parent = PP;

               node.box = entries.at(seeds[0]).second;// these are AABB.
            partner.box = entries.at(seeds[1]).second;

            // remove them from entries pool
            entries.erase(entries.begin() + std::min(seeds[0], seeds[1]));
            entries.erase(entries.begin() + std::max(seeds[0], seeds[1]) - 1);
        }

        while(!entries.empty())
        {
            if(min_entry > node.entry.size() &&
               min_entry - node.entry.size() >= entries.size())
            {
                for(typename temporal_entry_container::const_iterator
                        i(entries.begin()), e(entries.end()); i != e; ++i)
                {
                    node.entry.push_back(i->first);
                    node.box = expand(node.box, i->second, this->boundary_);
                }
                return PP;
            }
            if(min_entry > partner.entry.size() &&
               min_entry - partner.entry.size() >= entries.size())
            {
                for(typename temporal_entry_container::const_iterator
                        i(entries.begin()), e(entries.end()); i != e; ++i)
                {
                    partner.entry.push_back(i->first);
                    partner.box = expand(partner.box, i->second, this->boundary_);
                }
                return PP;
            }

            const std::pair<std::size_t, bool> next =
                this->pick_next(entries.begin(), entries.end(), node.box, partner.box);
            if(next.second) // next is partner
            {
                node.entry.push_back(entries.at(next.first).first);
                tree_.at(entries.at(next.first).first).parent = P;
                node.box =
                    expand(node.box, entries.at(next.first).second, this->boundary_);
            }
            else // next is for partner
            {
                partner.entry.push_back(entries.at(next.first).first);
                tree_.at(entries.at(next.first).first).parent = PP;
                partner.box =
                    expand(partner.box, entries.at(next.first).second, this->boundary_);
            }
            entries.erase(entries.begin() + next.first);
        }
        tree_.at(P) = node;
        tree_.at(PP) = partner;
        return PP;
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
                if(q.match(indexable_getter_(val)) && q.match(val))
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
        // reset connection to the parent!

        // insert node to its proper parent. to find the parent of this node N,
        // add 1 to level. root node should NOT come here.
        const std::size_t lvl = level_of(N) + 1;
        const aabb_type&  entry = tree_.at(N).box;
        const std::size_t L = choose_node_with_level(entry, lvl);

        if(tree_.at(L).has_enough_storage())
        {
            tree_.at(L).entry.push_back(N);
            tree_.at(N).parent = L;
            expand(tree_.at(L).box, entry, this->boundary_);
            this->adjust_tree(L);
        }
        else
        {
            const std::size_t LL = this->split_node(L, N);
            this->adjust_tree(L, LL);
        }
        return;
    }

    std::size_t
    choose_node_with_level(const aabb_type& entry, const std::size_t lvl)
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
                const scalar_type area_initial = area(tree_.at(*i).box, this->boundary_);
                const aabb_type box = expand(tree_.at(*i).box, entry, this->boundary_);

                const scalar_type area_expanded = area(box, this->boundary_);
                const scalar_type diff_area     = area_expanded - area_initial;
                if(diff_area < diff_area_min ||
                   (diff_area == diff_area_min && area_expanded < area_min))
                {
                    node_idx = *i;
                    diff_area_min = diff_area;
                    area_min = std::min(area_expanded, area_min);
                }
            }
        }
        return node_idx;
    }

    void condense_box(node_type& node)
    {
        assert(!node.entry.empty());
        if(node.is_leaf)
        {
            typename node_type::const_iterator i(node.entry.begin());
            node.box = indexable_getter_(this->container_.at(*i));
            ++i;
            for(typename node_type::const_iterator e(node.entry.end()); i != e; ++i)
            {
                node.box = expand(node.box,
                        indexable_getter_(this->container_.at(*i)), this->boundary_);
            }
        }
        else
        {
            typename node_type::const_iterator i = node.entry.begin();
            node.box = this->tree_.at(*i).box;
            ++i;
            for(typename node_type::const_iterator e(node.entry.end()); i != e; ++i)
            {
                node.box = expand(node.box, this->tree_.at(*i).box, this->boundary_);
            }
        }
        return;
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
    indexable_getter_type indexable_getter_;
};


} // perior
#endif//PERIOR_TREE_RTREE_HPP
