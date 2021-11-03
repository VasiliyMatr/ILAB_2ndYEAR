
#include <iostream>
#include <vector>
#include "geom3D.hh"

#if 1

int main( int argc, char ** argv)
{
    size_t trNum = 0;
    std::cin >> trNum;

    geom3D::IndexedTrGroup triangles {};

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

    geom3D::OrganizedTriangles orgTriangles {std::move (triangles), 20};
    geom3D::TrIndexes crossIds = orgTriangles.cross ();

    for (size_t i = 0, num = crossIds.size (); i != num; ++i)
        std::cout << crossIds[i] << std::endl;

    // std::cout << "crossed num:" << crossIds.size () << std::endl;

    std::cout << std::endl;
}

#else

using trPair = std::pair<geom3D::Triangle, bool>;

int main( int argc, char ** argv)
{
    size_t trNum = 0;
    std::cin >> trNum;

    std::vector<trPair> trVec (trNum);

    for (size_t i = 0; i < trNum; ++i)
    {
        geom3D::Point pts[3] = {};
        for (size_t j = 0; j < 3; ++j)
        {
            geom3D::fp_t coords[3] = {};
            std::cin >> coords[0] >> coords[1] >> coords[2];
            pts[j] = geom3D::Point {coords[0], coords[1], coords[2]};
        }

        trVec[i] = {geom3D::Triangle {pts[0], pts[1], pts[2]}, false};
    }

    for (size_t i = 0; i < trNum; ++i)
    {
        for (size_t j = i + 1; j < trNum; ++j)
            if (trVec[i].first.crosses (trVec[j].first))
            {
                trVec[i].second = true;
                trVec[j].second = true;
            }
    }

    for (size_t i = 0; i < trNum; ++i)
        if (trVec[i].second)
            std::cout << i << std::endl;

    std::cout << std::endl;
}

#endif