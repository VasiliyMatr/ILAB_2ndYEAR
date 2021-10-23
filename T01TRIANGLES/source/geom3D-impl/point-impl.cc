
#include "geom3D.hh"

namespace geom3D
{
   
Point Point::operator+( const Vector& vec ) const
{
    return Point {x_ + vec.x_, y_ + vec.y_, z_ + vec.z_};
}

fp_t sqDst( const Point& ft, const Point& sd )
{
    return Vector {ft, sd}.sqLen ();
}

} // namespace geom3D
