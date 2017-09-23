#ifndef PERIOR_TREE_AREA
#define PERIOR_TREE_AREA
#include <periortree/boundary_conditions.hpp>
#include <periortree/rectangle.hpp>
#include <cmath>

namespace perior
{

template<typename pointT, template<typename> class boundaryT>
inline typename boost::enable_if<traits::is_point<pointT>::value, bool>::type
area(const rectangle<pointT>& rec, const boundaryT<pointT>& b)
    BOOST_NOEXCEPT_OR_NOTHROW
{
    typename traits::scalar_type_of<pointT>::type retval(1);
    for(std::size_t i=0; i<traits::dimension<pointT>::value; ++i)
    {
        retval *= rec.width()[i];
    }
    return retval;
}

}// perior
#endif//PERIOR_TREE_AREA_HPP
