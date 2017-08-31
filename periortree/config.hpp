#ifndef PERIOR_TREE_CONFIG_HPP
#define PERIOR_TREE_CONFIG_HPP
#include <boost/config.hpp>

#ifndef __cplusplus
#error "this is c++ library but __cplusplus macro is not defined."
#endif

#if __cplusplus >= 201103L
#define PERIOR_TREE_ENABLE_CXX11 1
#else
#endif

#endif//PERIOR_TREE_CONFIG_HPP
