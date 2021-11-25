
#include "geom3D.hh"

namespace geom3D
{

Line::Line(const Segment &seg) : Line{Vector{seg.P1(), seg.P2()}, seg.P1()}
{
    assert(isConsistent());
}

Point Line::operator|(const Line &sd) const
{
    // General determinant should be compared with higher accuracy.
    static constexpr fp_t DET_CMP_PRECISION = // std::pow (fpCmpW::CMP_PRECISION, 3);
        fpCmpW::CMP_PRECISION * fpCmpW::CMP_PRECISION * fpCmpW::CMP_PRECISION;
    const Vector &a = dir_;
    const Vector &b = sd.dir_;
    Vector c = Vector::crossProduct(dir_, sd.dir_);
    c.scale();
    Vector d{P_, sd.P_};

    // Solving system: a*k1 + b*k2 + c*k3 = d
    const fp_t D = det(a, b, c);  // Not zero if there are solutions.
    const fp_t D3 = det(a, b, d); // Should be zero.
    if (std::abs(D) < DET_CMP_PRECISION || fpCmpW{} != D3)
        return Point{};

    const fp_t k = det(d, b, c) / D; // k1

    return P_ + a * k;
}

Point Line::operator|(const Plane &plane) const
{
    // Scalar product should be compared with higher accuracy.
    static constexpr fp_t SCAL_PROD_CMP_PRECISION = // std::pow (fpCmpW::CMP_PRECISION, 2);
        fpCmpW::CMP_PRECISION * fpCmpW::CMP_PRECISION;
    const Vector &n = plane.n();

    fp_t dirNormScal = Vector::scalarProduct(dir_, n);
    if (std::abs(dirNormScal) < SCAL_PROD_CMP_PRECISION)
        return Point{};
    fp_t planeEVal = P_[X] * n[X] + P_[Y] * n[Y] + P_[Z] * n[Z] + plane.D();

    fp_t k = -planeEVal / dirNormScal;
    return P_ + k * dir_;
}

Point Line::operator|(const Segment &seg) const
{
    Point linesCross = *this | Line{seg};
    return seg.linearContains(linesCross) ? linesCross : Point{};
}

} // namespace geom3D
