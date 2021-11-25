
#include "geom3D.hh"

namespace geom3D
{

Point operator+(const Point &P, const Vector &vec)
{
    Point toRet{P};
    for (size_t i = 0; i < DNUM; ++i)
        toRet[i] += vec[i];
    return toRet;
}

fp_t sqDst(const Point &ft, const Point &sd)
{
    return Vector{ft, sd}.sqLen();
}

} // namespace geom3D
