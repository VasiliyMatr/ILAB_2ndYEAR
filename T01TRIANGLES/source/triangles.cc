
#include <iostream>
#include <vector>
#include "geom3D.hh"
#include "geom3D-split.hh"

int main()
{
    size_t trNum = 0;
    std::cin >> trNum;

    geom3D::IndexedTrsGroup triangles {};

    for (size_t i = 0; i < trNum; ++i)
    {
        geom3D::Point pts[3] = {};
        for (size_t j = 0; j < 3; ++j)
        {
            geom3D::fp_t coords[3] = {};
            std::cin >> coords[0] >> coords[1] >> coords[2];
            pts[j] = geom3D::Point {coords[0], coords[1], coords[2]};
        }

        triangles.push_back ({geom3D::Triangle {pts[0], pts[1], pts[2]}, i});
    }

#if 1
    geom3D::SplittedTrsGroup splTriangles {triangles, 20};
    geom3D::TrsIndexes crossIds = splTriangles.cross ();
#else
    geom3D::TrsIndexes crossIds = geom3D::cross (triangles);
#endif

    for (size_t i = 0, num = crossIds.size (); i != num; ++i)
        std::cout << crossIds[i] << std::endl;

    std::cout << std::endl << crossIds.size () << std::endl;
}
