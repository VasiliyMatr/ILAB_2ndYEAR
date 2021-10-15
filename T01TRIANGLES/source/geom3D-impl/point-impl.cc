
#include "geom3D.hh"

namespace geom3D
{

bool Point::isValid() const
{
    return geom3D::isValid (x_) &&
           geom3D::isValid (y_) &&
           geom3D::isValid (z_);
}

bool Point::operator==( const Point& sd ) const
{
    return isEqual (x_, sd.x_) &&
           isEqual (y_, sd.y_) &&
           isEqual (z_, sd.z_);
}

Point Point::operator+( const Vector& vec ) const
{
    return Point {x_ + vec.x_, y_ + vec.y_, z_ + vec.z_};
}

fp_t sqDst( const Point& ft, const Point& sd )
{
    return Vector {ft, sd}.sqLen ();
}

} // namespace geom3D
