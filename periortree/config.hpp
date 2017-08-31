#ifndef PERIOR_TREE_CONFIG_HPP
#define PERIOR_TREE_CONFIG_HPP

#ifndef __cplusplus
#error "this is c++ library but __cplusplus macro is not defined."
#endif

#if __cplusplus >= 201103L
#define PERIOR_TREE_ENABLE_CXX11 1
#define PERIOR_TREE_CONSTEXPR    constexpr
#define PERIOR_TREE_NOEXCEPT     noexcept
#define PERIOR_TREE_OVERRIDE     override
#define PERIOR_TREE_NULLPTR      nullptr
#else
#define PERIOR_TREE_CONSTEXPR
#define PERIOR_TREE_NOEXCEPT  throw()
#define PERIOR_TREE_OVERRIDE
#define PERIOR_TREE_NULLPTR   NULL
#endif

#endif//PERIOR_TREE_CONFIG_HPP
