
#include "geom3D.hh"

namespace geom3D
{

Plane::Plane( const Vector& normVec, fp_t D ) : normVec_(normVec), D_(D) {}

bool Plane::isValid() const
{
    bool isValid (fp_t);
    return normVec_.isValid () && isValid (D_);
}

Line Plane::operator|( const Plane& second ) const
{
    const Vector& n1 = normVec_;
    const Vector& n2 = second.normVec_;
    const Vector n3 = Vector::crossProduct (n1, n2);

    const Vector a {n1.x_, n2.x_, n3.x_};
    const Vector b {n1.y_, n2.y_, n3.y_};
    const Vector c {n1.z_, n2.z_, n3.z_};

    const Vector d {D_, second.D_, 0};

    // solving system: a + b + c = d
    fp_t D = det (a, b, c);
    fp_t D1 = det (d, b, c);
    fp_t D2 = det (a, d, c);
    fp_t D3 = det (a, b, d);

    return Line {n3, Point {D1/D, D2/D, D3/D}};
}

} // namespace geom3D