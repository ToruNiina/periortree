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

template<typename charT, typename traits, typename T>
std::basic_ostream<charT, traits>&
to_svg(std::basic_ostream<charT, traits>&   os,
       const aabb<T, 2>&                    box,
       const cubic_periodic_boundary<T, 2>& b,
       const std::string                    stroke       = "black",
       const std::size_t                    stroke_width = 1,
       const std::string                    fill         = "none")
{
    // split box
    boost::container::static_vector<aabb<T, 2>, 4> boxes(1, box);
    for(std::size_t dim=0; dim < 2; ++dim)
    {
        const std::size_t sz = boxes.size();
        for(std::size_t i=0; i<sz; ++i)
        {
            aabb<T, 2> bx = boxes.at(i);
            if(bx.lower()[dim] > bx.upper()[dim])
            {
                aabb<T, 2> splitted(bx);
                bx.lower()[dim] = b.lower()[dim];
                splitted.upper()[dim] = b.upper()[dim];
                boxes.at(i) = bx;
                boxes.push_back(splitted);
            }
        }
    }

    for(auto i(boxes.begin()), e(boxes.end()); i!=e; ++i)
    {
        const point<T, 2>& xy = i->lower();
        const point<T, 2>  rg = i->upper() - i->lower();
        os << "<rect x = \""   << xy[0]  << "\" y = \""      << xy[1]
           << "\" width = \""  << rg[0]  << "\" height = \"" << rg[1]
           << "\" stroke = \"" << stroke << "\" stroke-width = \"" << stroke_width
           << "\" fill = \""   << fill   << "\"/>" << std::flush;
    }
    return os;
}

}// perior
#endif//PERIOR_TREE_DUMP_SVG_HPP
