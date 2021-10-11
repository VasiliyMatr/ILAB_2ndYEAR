
#include "geom3D.hh"
namespace geom3D
{

Line::Line( const Vector& dirVec, const Point& point ) :
    dirVec_ (dirVec), point_ (point) {}

bool Line::isValid() const
{
    return dirVec_.isValid () && point_.isValid ();
}

Point Line::operator|( const Line& second ) const
{
    const Vector& a = dirVec_;
    const Vector& b = second.dirVec_;
    const Vector c = Vector::crossProduct (dirVec_, second.dirVec_);
    const Vector d {point_, second.point_};

    // solving system: a + b + c = d
    const fp_t D = det (a, b, c); // != 0 if there are solutions
    const fp_t D3 = det (a, b, d); // should be zero
    if (isEqual (D, 0) || !isEqual (D3, 0))
        return Point {};

    const fp_t k = det (d, b, c) / D;

    return Point {point_.x_ + a.x_ * k,
                  point_.y_ + a.y_ * k,
                  point_.z_ + a.z_ * k};
}

Point operator|( const Line& line, const Segment& seg )
{
    Point linesCross = line | Line {Vector {seg.B_, seg.A_}, seg.A_};
    fp_t segSqrLen = Vector {seg.A_, seg.B_}.squareLen ();
    fp_t crossSqrLen1 = Vector {seg.A_, linesCross}.squareLen ();
    fp_t crossSqrLen2 = Vector {seg.B_, linesCross}.squareLen ();

    if (segSqrLen < crossSqrLen1 || segSqrLen < crossSqrLen2)
        return Point {};

    return linesCross;
}

} // namespace geom3D