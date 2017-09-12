#include <experimental/rtree.hpp>
#include <fstream>
#include <random>

typedef perior::point<double,2> point_t;
typedef perior::aabb<double,2>  aabb_t;
typedef perior::cubic_periodic_boundary<double, 2> boundary_t;
typedef std::pair<aabb_t, std::size_t> value_t;
typedef perior::rtree<value_t, perior::quadratic<2, 6>, boundary_t> rtree_t;

void dump(const std::string& fname, const rtree_t& rtr, const boundary_t& bdry)
{
    std::ofstream ofs(fname.c_str());

    ofs << "<?xml version=\"1.0\" standalone=\"no\"?>"                       << std::endl;
    ofs << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\""                << std::endl;
    ofs << "          \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
    ofs << "<svg width=\"20cm\" height=\"20cm\" viewBox=\"0 0 1000 1000\""   << std::endl;
    ofs << "xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"           << std::endl;
    ofs << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>" << std::endl;
    rtr.dump(ofs);
    ofs << "</svg>" << std::endl;
    ofs.close();
    return;
}

int main()
{
    boundary_t bdry(point_t(0,0), point_t(1000, 1000));
    rtree_t tree(bdry);

    std::mt19937 mt_(123456789);
    std::uniform_int_distribution<std::size_t> uni(0, 10000);

    std::vector<aabb_t> boxes;
    for(std::size_t i=0; i<50; ++i)
    {
        aabb_t box;
        const std::size_t spidx = uni(mt_);
        const std::size_t xidx  =  spidx % 100;
        const std::size_t yidx  = (spidx - xidx) / 100;
        const point_t l(xidx * 10, yidx * 10);
        const point_t u(l[0] + 10, l[1] + 10);

        box.upper() = u;
        box.lower() = l;
        boxes.push_back(box);

        tree.insert(value_t(box, i));

        std::string fname = std::string("rtree_") + std::to_string(i) + std::string(".svg");
        dump(fname, tree, bdry);

    }

    for(std::size_t i=50; i<70; ++i)
    {
        const std::size_t idx = i - 50;
        std::cout << std::boolalpha << tree.remove(value_t(boxes.at(idx), idx)) << std::endl;
        std::string fname = std::string("rtree_") + std::to_string(i) + std::string(".svg");
        dump(fname, tree, bdry);
    }

    for(std::size_t i=70; i<90; ++i)
    {
        aabb_t box;
        const std::size_t spidx = uni(mt_);
        const std::size_t xidx  =  spidx % 100;
        const std::size_t yidx  = (spidx - xidx) / 100;
        const point_t l(xidx * 10, yidx * 10);
        const point_t u(l[0] + 10, l[1] + 10);

        box.upper() = u;
        box.lower() = l;
        boxes.push_back(box);

        tree.insert(value_t(box, i));
        std::string fname = std::string("rtree_") + std::to_string(i) + std::string(".svg");
        dump(fname, tree, bdry);
    }
    return 0;
}
