
#include "geom3D.hh"

namespace geom3D
{

bool isValid( fp_t value )
{
    return !(std::isnan (value) || std::isinf (value));
}

int diff( fp_t a, fp_t b )
{
    if (std::abs (a - b) < std::numeric_limits<fp_t>::min ())
        return 0;

    if (a < b)
        return -1;
    
    return 1;
}

Point::Point( fp_t x, fp_t y, fp_t z ) : x_(x), y_(y), z_(z) {}

bool Point::isValid() const
{
    return geom3D::isValid (x_) && geom3D::isValid (y_) && geom3D::isValid (z_);
}

Vector::Vector( fp_t x, fp_t y, fp_t z ) : x_(x), y_(y), z_(z) {}

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

Vector Vector::operator+=( const Vector& second )
{
    x_ += second.x_; y_ += second.y_; z_ += second.z_;
    return *this;
}

Vector Vector::operator-=( const Vector& second )
{
    x_ -= second.x_; y_ -= second.y_; z_ -= second.z_;
    return *this;
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

Vector Vector::operator*=( const fp_t num )
{
    x_ *= num; y_ *= num; z_ *= num;
    return *this;
}

Vector Vector::operator/=( const fp_t num )
{
    x_ /= num; y_ /= num; z_ /= num;
    return *this;
}

bool Vector::operator==( const Vector& second ) const
{
    return !(diff (x_, second.x_) || diff (y_, second.y_) || diff (z_, second.z_));
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