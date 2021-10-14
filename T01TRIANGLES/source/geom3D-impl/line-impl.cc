
#include "geom3D.hh"
namespace geom3D
{

Line::Line( const Vector& dirVec, const Point& point ) :
    dirVec_ (dirVec), point_ (point) {}

Line::Line( const Segment& seg ) :
    dirVec_ (seg.A_ == seg.B_ ? Vector {} : Vector {seg.A_, seg.B_}), point_ (seg.A_) {}

bool Line::isValid() const
{
    return dirVec_.isValid () && point_.isValid ();
}

bool Line::ifBelongs( const Point& point ) const
{
    return isEqual ((point_.x_ - point.x_) * dirVec_.y_,
                    (point_.y_ - point.y_) * dirVec_.x_) &&
           isEqual ((point_.y_ - point.y_) * dirVec_.z_,
                    (point_.z_ - point.z_) * dirVec_.y_);
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
    Point linesCross = line | Line {seg};
    if (linesCross.isValid ())
    {
        if (seg.sqLen_ < sqDst (seg.A_, linesCross) ||
            seg.sqLen_ < sqDst (seg.B_, linesCross))
            return Point {};

        return linesCross;
    }

    return line.ifBelongs (seg.A_) ? seg.A_ : Point {};
}

Point operator|( const Line& line, const Plane& plane )
{
    return plane | line;
}

} // namespace geom3D