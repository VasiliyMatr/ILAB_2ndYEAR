
#include "geom3D.hh"

namespace geom3D
{

Vector::Vector( fp_t x, fp_t y, fp_t z ) : x_(x), y_(y), z_(z) {}

Vector::Vector( const Point& A, const Point& B ) :
    x_(B.x_-A.x_), y_(B.y_-A.y_), z_(B.z_-A.z_) {}

bool Vector::isValid() const
{
    return geom3D::isValid (x_) && geom3D::isValid (y_) && geom3D::isValid (z_);
}

Vector Vector::operator-() const
{
    return Vector {-x_, -y_, -z_};
}

Vector Vector::operator+( const Vector& second ) const
{
    return Vector {x_ + second.x_, y_ + second.y_, z_ + second.z_};
}

Vector Vector::operator-( const Vector& second ) const
{
    return Vector {x_ - second.x_, y_ - second.y_, z_ - second.z_};
}

Vector Vector::operator*( const fp_t num ) const
{
    return Vector {x_ * num, y_ * num, z_ * num};
}

Vector operator*( fp_t num, Vector vec )
{
    return vec * num;
}

Vector Vector::operator/( const fp_t num ) const
{
    return Vector {x_ / num, y_ / num, z_ / num};
}

bool Vector::operator==( const Vector& second ) const
{
    return isEqual (x_, second.x_) && isEqual (y_, second.y_) && isEqual (z_, second.z_);
}

fp_t Vector::squareLen() const
{
    return x_ * x_ + y_ * y_ + z_ * z_;
}

fp_t Vector::len() const
{
    return std::sqrt (squareLen ());
}

Vector Vector::normalized() const
{
    fp_t length = len();
    return Vector {x_ / length, y_ / length, z_ / length};
}

Vector Vector::crossProduct( const Vector& first, const Vector& second )
{
    return Vector {first.y_ * second.z_ - first.z_ * second.y_,
                  -first.x_ * second.z_ + first.z_ * second.x_,
                   first.x_ * second.y_ - first.y_ * second.x_};
}

fp_t Vector::scalarProduct( const Vector& first, const Vector& second )
{
    return first.x_ * second.x_ + first.y_ * second.y_ + first.z_ * second.z_;
}

} // namespace geom3D
