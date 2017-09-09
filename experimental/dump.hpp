#ifndef PERIOR_TREE_DUMP_SVG_HPP
#define PERIOR_TREE_DUMP_SVG_HPP
#include "point.hpp"
#include "aabb.hpp"
#include "boundary_condition.hpp"
#include <string>
#include <iostream>

namespace perior
{

template<typename charT, typename traits, typename T>
std::basic_ostream<charT, traits>&
to_svg(std::basic_ostream<charT, traits>& os,
       const aabb<T, 2>&                  box,
       const unlimited_boundary<T, 2>&    b,
       const std::string                  stroke       = "black",
       const std::size_t                  stroke_width = 1,
       const std::string                  fill         = "none")
{
    const point<T, 2>& xy = box.lower();
    const point<T, 2>  rg = box.upper() - box.lower();
    os << "<rect x = \""   << xy[0]  << "\" y = \""      << xy[1]
       << "\" width = \""  << rg[0]  << "\" height = \"" << rg[1]
       << "\" stroke = \"" << stroke << "\" stroke-width = \"" << stroke_width
       << "\" fill = \""   << fill   << "\"/>" << std::flush;
    return os;
}

}// perior
#endif//PERIOR_TREE_DUMP_SVG_HPP
