
#include <iostream>
#include "geom3D.hh"

using trPair = std::pair<geom3D::TriangleInfo, bool>;

int main( int argc, char ** argv)
{
    size_t trNum = 0;

    std::cin >> trNum;

    trPair * tr = new trPair [trNum];

    for (int i = 0; i < trNum; ++i)
    {
        geom3D::Point pts[3] = {};
        for (int j = 0; j < 3; ++j)
        {
            geom3D::fp_t coords [3] = {};
            std::cin >> coords[0] >> coords[1] >> coords[2];
            pts [j] = geom3D::Point {coords[0], coords[1], coords[2]};
        }

        tr[i].first = geom3D::TriangleInfo {pts[0], pts[1], pts[2]};
        tr[i].second = false;
    }

    for (int i = 0; i < trNum; ++i)
    {
        for (int j = i; j < trNum; ++j)
            if (tr[i].first.isInter (tr[j].first))
            {
                tr[i].second = true;
                tr[j].second = true;
            }
    }

    for (int i = 0; i < trNum; ++i)
        if (tr[i].second)
            std::cout << " " << i;

    std::cout << std::endl;

    delete [] tr;
}