
#include "geom3D.hh"

namespace geom3D
{

Vector::Vector( fp_t x, fp_t y, fp_t z ) : x_(x), y_(y), z_(z) {}

Vector::Vector( const Point& P1, const Point& P2 ) :
    x_(P2.x_-P1.x_), y_(P2.y_-P1.y_), z_(P2.z_-P1.z_) {}

bool Vector::isValid() const
{
    bool isValid( fp_t );
    return isValid (x_) && isValid (y_) && isValid (z_);
}

Vector Vector::operator-() const
{
    return {-x_, -y_, -z_};
}

Vector Vector::operator+( const Vector& sd ) const
{
    return {x_ + sd.x_, y_ + sd.y_, z_ + sd.z_};
}

Vector Vector::operator-( const Vector& sd ) const
{
    return {x_ - sd.x_, y_ - sd.y_, z_ - sd.z_};
}

Vector Vector::operator*( const fp_t num ) const
{
    return {x_ * num, y_ * num, z_ * num};
}

Vector operator*( fp_t num, Vector vec )
{
    return vec * num;
}

Vector Vector::operator/( const fp_t num ) const
{
    return {x_ / num, y_ / num, z_ / num};
}

bool Vector::operator==( const Vector& sd ) const
{
    return isEqual (x_, sd.x_) && isEqual (y_, sd.y_) && isEqual (z_, sd.z_);
}

fp_t Vector::sqLen() const
{
    return x_*x_ + y_*y_ + z_*z_;
}

fp_t Vector::len() const
{
    return std::sqrt (sqLen ());
}

Vector Vector::crossProduct( const Vector& ft, const Vector& sd )
{
    return {ft.y_*sd.z_ - ft.z_*sd.y_,
           -ft.x_*sd.z_ + ft.z_*sd.x_,
            ft.x_*sd.y_ - ft.y_*sd.x_};
}

fp_t Vector::scalarProduct( const Vector& ft, const Vector& sd )
{
    return ft.x_*sd.x_ + ft.y_*sd.y_ + ft.z_*sd.z_;
}

} // namespace geom3D
