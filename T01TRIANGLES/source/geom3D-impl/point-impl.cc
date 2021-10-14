
#include "geom3D.hh"

namespace geom3D
{

bool Point::isValid() const
{
    return geom3D::isValid (x_) &&
           geom3D::isValid (y_) &&
           geom3D::isValid (z_);
}

bool Point::operator==( const Point& second ) const
{
    return isEqual (x_, second.x_) &&
           isEqual (y_, second.y_) &&
           isEqual (z_, second.z_);
}

fp_t sqDst( const Point& A, const Point& B )
{
    return Vector {A, B}.sqLen ();
}

} // namespace geom3D
