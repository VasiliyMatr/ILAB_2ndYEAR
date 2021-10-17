
#include "geom3D.hh"

// have strange linkage error with this:
// using namespace geom3D;

namespace geom3D
{

Segment::Segment( const Point& A, const Point& B ) : A_ (A), B_ (B),
    sqLen_ (sqDst (A, B))
{}

Point Segment::A() const
{
    return A_;
}

Point Segment::B() const
{
    return B_;
}

fp_t Segment::sqLen() const
{
    return sqLen_;
}

bool Segment::isValid() const
{
    bool isValid( fp_t );
    return A_.isValid () && B_.isValid () && isValid (sqLen_);
}

bool Segment::linearContains( const Point& P ) const
{
    return sqLen_ + FP_CMP_PRECISION >= sqDst (P, A_) &&
           sqLen_ + FP_CMP_PRECISION >= sqDst (P, B_);
}

Point Segment::operator|( const Line& line ) const
{
    return line | *this;
}

Point Segment::operator|( const Plane& plane ) const
{
    Point lineCross = Line {*this} | plane;
    return linearContains (lineCross) ? lineCross : Point {};
}

} // namespace geom3D