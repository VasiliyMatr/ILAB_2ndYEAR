
#include "geom3D.hh"

namespace geom3D
{

bool isValid( fp_t value )
{
    return !(std::isnan (value) || std::isinf (value));
}

int isEqual( fp_t a, fp_t b )
{
    return std::abs (a - b) < FP_CMP_PRECISION;
}

fp_t det( const Vector& a, const Vector& b, const Vector& c )
{
    return a.x_ * (b.y_*c.z_ - b.z_*c.y_)
          -a.y_ * (b.x_*c.z_ - b.z_*c.x_)
          +a.z_ * (b.x_*c.y_ - b.y_*c.x_);
}

} // namespace geom3D