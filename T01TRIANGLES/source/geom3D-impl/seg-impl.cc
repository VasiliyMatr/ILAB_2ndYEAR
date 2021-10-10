
#include "geom3D.hh"

// have strange linkage error with this:
// using namespace geom3D;

namespace geom3D
{

Segment::Segment( const Point& A, const Point& B ) : A_(A), B_(B) {}

bool Segment::isValid() const
{
    return A_.isValid () && B_.isValid ();
}

Point operator|( const Segment& seg, const Line& line )
{
    return line | seg;
}

} // namespace geom3D