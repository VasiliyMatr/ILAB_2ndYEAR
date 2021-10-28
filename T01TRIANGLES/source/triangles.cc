
#include <iostream>
#include <vector>
#include "geom3D.hh"

int main( int argc, char ** argv)
{
    size_t trNum = 0;
    std::cin >> trNum;

    geom3D::TrianglesUnion triangles{};

    for (size_t i = 0; i < trNum; ++i)
    {
        geom3D::Point pts[3] = {};
        for (size_t j = 0; j < 3; ++j)
        {
            geom3D::fp_t coords[3] = {};
            std::cin >> coords[0] >> coords[1] >> coords[2];
            pts[j] = geom3D::Point {coords[0], coords[1], coords[2]};
        }

        triangles.data_.push_back ({geom3D::Triangle {pts[0], pts[1], pts[2]}, i});
    }

    std::vector<size_t> crossIds = triangles.cross ();

    for (size_t i = 0, num = crossIds.size (); i != num; ++i)
        std::cout << crossIds[i] << std::endl;

    std::cout << std::endl;
}