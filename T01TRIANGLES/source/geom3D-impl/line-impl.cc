
#include "geom3D.hh"

namespace geom3D
{

Point Line::operator|(const Line &sd) const
{
    const Vector &a = dir_;
    const Vector &b = sd.dir_;
    Vector c = Vector::crossProduct(dir_, sd.dir_);
    c.scale();
    Vector d{P_, sd.P_};

    // Solving system: a*k1 + b*k2 + c*k3 = d
    const fp_t D = det(a, b, c);  // Not zero if there are solutions.
    const fp_t D3 = det(a, b, d); // Should be zero.
    if (fpCmpW<valueOrder_t::THIRD>{} == std::abs(D) || fpCmpW{} != D3)
        return Point{};

    const fp_t k = det(d, b, c) / D; // k1

    return P_ + a * k;
}

Point Line::operator|(const Plane &plane) const
{
    const Vector &n = plane.n();

    fp_t dirNormScal = Vector::scalarProduct(dir_, n);
    if (fpCmpW<valueOrder_t::SECOND>{} == std::abs(dirNormScal))
        return Point{};

    fp_t k = -plane.eVal(P_) / dirNormScal;
    return P_ + k * dir_;
}

Point Line::operator|(const Segment &seg) const
{
    Point linesCross = *this | Line{seg};
    return seg.linearContains(linesCross) ? linesCross : Point{};
}

} // namespace geom3D
