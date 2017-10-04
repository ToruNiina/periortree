#ifndef PERIOR_TREE_TO_SVG_HPP
#define PERIOR_TREE_TO_SVG_HPP
#include <periortree/rectangle.hpp>
#include <periortree/point_traits.hpp>
#include <periortree/boundary_conditions.hpp>
#include <iostream>
#include <string>

namespace perior
{

template<typename charT, typename traits, typename pointT>
std::basic_ostream<charT, traits>&
to_svg(std::basic_ostream<charT, traits>& os,
       const rectangle<pointT>&           box,
       const unlimited_boundary<pointT>&  b,
       const std::string&                 stroke       = "black",
       const std::size_t                  stroke_width = 1,
       const std::string&                 fill         = "none")
{
    BOOST_STATIC_ASSERT_MSG(
            ::perior::traits::dimension<pointT>::value == 2,
            "to_svg works only with 2-dimensional rectangle");

    const pointT xy = box.center - box.radius;
    const pointT rg = box.radius * 2;
    os << "<rect x = \""   << xy[0]  << "\" y = \""      << xy[1]
       << "\" width = \""  << rg[0]  << "\" height = \"" << rg[1]
       << "\" stroke = \"" << stroke << "\" stroke-width = \"" << stroke_width
       << "\" fill = \""   << fill   << "\"/>";
    return os;
}

template<typename charT, typename traits, typename pointT>
std::basic_ostream<charT, traits>&
to_svg(std::basic_ostream<charT, traits>&     os,
       const rectangle<pointT>&               box,
       const cubic_periodic_boundary<pointT>& b,
       const std::string&                     stroke       = "black",
       const std::size_t                      stroke_width = 1,
       const std::string&                     fill         = "none")
{
    BOOST_STATIC_ASSERT_MSG(
            ::perior::traits::dimension<pointT>::value == 2,
            "to_svg works only with 2-dimensional rectangle");

    // split box according to the boundary
    // here, use box
    std::vector<std::pair<pointT, pointT> > boxes; boxes.reserve(4);
    pointT lower = restrict_position(box.center - box.radius, b);
    pointT upper = restrict_position(box.center + box.radius, b);
    for(std::size_t i=0; i<2; ++i)
    {
        if(box.radius[i] >= b.half_width()[i])
        {
            lower[i] = b.lower()[i];
            upper[i] = b.upper()[i];
        }
    }

    boxes.push_back(std::make_pair(lower, upper));

    for(std::size_t dim=0; dim < 2; ++dim)
    {
        const std::size_t sz = boxes.size();
        for(std::size_t i=0; i<sz; ++i)
        {
            std::pair<pointT, pointT> bx = boxes.at(i);
            if(bx.first[dim] > bx.second[dim])
            {
                std::pair<pointT, pointT> splitted(bx);
                bx.first[dim]        = b.lower()[dim];
                splitted.second[dim] = b.upper()[dim];
                boxes.at(i) = bx;
                boxes.push_back(splitted);
            }
        }
    }

    for(typename std::vector<std::pair<pointT, pointT> >::const_iterator
            i(boxes.begin()), e(boxes.end()); i != e; ++i)
    {
        const pointT& xy = i->first;
        const pointT  rg = i->second - i->first;
        os << "<rect x = \""   << xy[0]  << "\" y = \""      << xy[1]
           << "\" width = \""  << rg[0]  << "\" height = \"" << rg[1]
           << "\" stroke = \"" << stroke << "\" stroke-width = \"" << stroke_width
           << "\" fill = \""   << fill   << "\"/>\n";
    }
    return os;
}

}// perior
#endif//PERIOR_TREE_DUMP_SVG_HPP
