#ifndef PERIOR_TREE_CONTAINERS
#define PERIOR_TREE_CONTAINERS
#include <boost/version.hpp>
#include <vector>

#if BOOST_VERSION >= 105400
#include <boost/container/vector.hpp>
#include <boost/container/static_vector.hpp>
#define PERIOR_TREE_HAS_BOOST_CONTAINER_VECTOR 1
#define PERIOR_TREE_HAS_BOOST_STATIC_VECTOR 1
#endif

#if BOOST_VERSION >= 105800
#define PERIOR_TREE_HAS_BOOST_SMALL_VECTOR 1
#include <boost/container/small_vector.hpp>
#endif

namespace perior
{

template<typename T, typename Alloc>
struct gen_vector
{
#ifdef PERIOR_TREE_HAS_BOOST_CONTAINER_VECTOR
    typedef boost::container::vector<T, Alloc> type;
#else
    typedef std::vector<T, Alloc> type;
#endif
};

template<typename T, std::size_t N, typename Alloc>
struct gen_small_vector
{
#ifdef PERIOR_TREE_HAS_BOOST_SMALL_VECTOR
    typedef boost::container::small_vector<T, N, Alloc> type;
#else
    typedef std::vector<T, Alloc> type;
#endif
};

template<typename T, std::size_t N>
struct gen_static_vector
{
#ifdef PERIOR_TREE_HAS_BOOST_STATIC_VECTOR
    typedef boost::container::static_vector<T, N> type;
#else
    typedef std::vector<T> type;
#endif
};

} // perior
#endif// PERIOR_TREE_CONTAINERS
