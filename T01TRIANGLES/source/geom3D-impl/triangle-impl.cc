
#include "geom3D.hh"

namespace geom3D
{

TriangleInfo::TriangleInfo( const Point& A, const Point& B, const Point& C ) :
    plane_ (A, B, C), AB_{A, B}, BC_{B, C}, CA_{C, A}
{}

bool TriangleInfo::isValid() const
{
    return plane_.isValid () && AB_.isValid () && BC_.isValid () && CA_.isValid ();
}

// TO MANY BRANCHES!!!
namespace
{

Segment crossTriangleNLine( const TriangleInfo& tr, const Line& line )
{
    Point ABCross = line | tr.AB_;
    Point BCCross = line | tr.BC_;
    bool ABValid = ABCross.isValid ();
    bool BCValid = BCCross.isValid ();

    if (ABValid && BCValid)
        return Segment {ABCross, BCCross};

    Point CACross = line | tr.CA_;
    bool CAValid = CACross.isValid ();

    if (BCValid && CAValid)
        return Segment {BCCross, CACross};

    if (ABValid && CAValid)
        return Segment {ABCross, CACross};

    return Segment {};
}

// copy pasted!!! - move to separate func
bool pointBelongsToSegment( const Segment& seg, const Point& point )
{
    fp_t segSqrLen = Vector {seg.A_, seg.B_}.squareLen ();
    fp_t crossSqrLen1 = Vector {seg.A_, point}.squareLen ();
    fp_t crossSqrLen2 = Vector {seg.B_, point}.squareLen ();

    return segSqrLen >= crossSqrLen1 && segSqrLen >= crossSqrLen2;
}

} // namespace

bool TriangleInfo::isInter( const TriangleInfo& second ) const
{
    Line planesCross = plane_ | second.plane_;
    if (!planesCross.isValid ()) // have parallel or equal planes
    {
        return false;
    }

    Segment thisCross = crossTriangleNLine (*this, planesCross);
    if (!thisCross.isValid ())
        return false;

    Segment secondCross = crossTriangleNLine (second, planesCross);

    return secondCross.isValid () &&
        (pointBelongsToSegment (secondCross, thisCross.A_) ||
         pointBelongsToSegment (secondCross, thisCross.B_));
}

} // namespace geom3D