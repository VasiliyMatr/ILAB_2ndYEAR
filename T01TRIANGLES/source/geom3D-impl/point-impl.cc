
#include "geom3D.hh"

namespace geom3D
{

bool Point::isValid() const
{
    return geom3D::isValid (x_) &&
           geom3D::isValid (y_) &&
           geom3D::isValid (z_);
}

fp_t sqDst( const Point& A, const Point& B )
{
    return Vector {A, B}.sqLen ();
}

} // namespace geom3D
