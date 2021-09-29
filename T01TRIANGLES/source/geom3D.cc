
#include "geom3D.hh"

namespace geom3D
{

Point::Point( fp_t x, fp_t y, fp_t z ) : x_(x), y_(y), z_(z) {}

bool Point::isValid() const
{
    return !(std::isnan (x_) || std::isnan (y_) || std::isnan (z_) ||
             std::isinf (x_) || std::isinf (y_) || std::isinf (z_));
}

} // namespace geom3D