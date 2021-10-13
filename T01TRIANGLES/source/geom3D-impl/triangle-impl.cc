
#include "geom3D.hh"

namespace geom3D
{

TriangleInfo::TriangleInfo( const Point& A, const Point& B, const Point& C ) :
    plane_ (A, B, C), isDegen_ (!plane_.isValid ()),
    AB_ {A, B}, BC_ {B, C}, CA_ {C, A},
    maxLenSeg_ (AB_.sqLen_ > BC_.sqLen_ ?
               (AB_.sqLen_ > CA_.sqLen_ ? AB_ : CA_) :
               (BC_.sqLen_ > CA_.sqLen_ ? BC_ : CA_))
{}

bool TriangleInfo::isValid() const
{
    return plane_.isValid () && AB_.isValid () &&
           BC_.isValid () && CA_.isValid ();
}

namespace
{

// Only for seg and point on one line.
// A bit copypasted from Segment::operator|( const Segment& second ). Fix?
bool flatIsBelongs( const Segment& seg, const Point& point );

bool isBelongs( const Plane&, const Point& );

// To check if objects crosses.
// For not degenerate triangles.
bool ifCross( const TriangleInfo&, const TriangleInfo& );
// For not degenerate triangle.
bool ifCross( const TriangleInfo&, const Segment& );
bool ifCross( const Segment&, const Segment& );

// To check if objects crosses (for obj on one plane).
bool flatIfCross( const TriangleInfo&, const TriangleInfo& );
bool flatIfCross( const TriangleInfo&, const Segment& );
bool flatIfCross( const TriangleInfo&, const Point& );

} // namespace

bool TriangleInfo::isInter( const TriangleInfo& second ) const
{
    if (isDegen_)
    {
        if (second.isDegen_) 
            // Crossing 2 segments.
            return ifCross (maxLenSeg_, second.maxLenSeg_);

        // Crossing segment & triangle.
        return ifCross (second, maxLenSeg_);
    }

    if (second.isDegen_)
        // Crossing segment & triangle.
        return ifCross (*this, second.maxLenSeg_);

    // Crossing 2 triangles.
    return ifCross (*this, second);
}

namespace
{

bool ifCross( const TriangleInfo& ft, const TriangleInfo& sd )
{
    Point abLineCross = Line {ft.AB_} | sd.plane_;
    Point bcLineCross = Line {ft.BC_} | sd.plane_;
    Point caLineCross = Line {ft.CA_} | sd.plane_;

    Point abCross = flatIsBelongs (ft.AB_, abLineCross) ? abLineCross : Point{};
    Point bcCross = flatIsBelongs (ft.BC_, bcLineCross) ? bcLineCross : Point{};
    Point caCross = flatIsBelongs (ft.CA_, caLineCross) ? caLineCross : Point{};

    // Crossing segment & triangle in the same plane?
    if (abCross.isValid ())
    {
        if (bcCross.isValid ())
            return flatIfCross (sd, Segment {abCross, bcCross});

        return flatIfCross (sd, Segment {abCross, caCross});
    }

    if (bcCross.isValid ())
        return flatIfCross (sd, Segment {bcCross, caCross});
    // Not segment & triangle case.

    if (isBelongs (sd.plane_, ft.AB_.A_))
        // Crossing 2 triangles in the same plane.
        return flatIfCross (ft, sd);

    return false;
}

bool ifCross( const TriangleInfo& trInfo, const Segment& seg )
{
    Point segPlaneCross = Line {seg} | trInfo.plane_;
    if (segPlaneCross.isValid () && flatIsBelongs (seg, segPlaneCross))
        // Crossing point & triangle.
        return flatIfCross (trInfo, segPlaneCross);

    return isBelongs (trInfo.plane_, seg.A_) ?
           flatIfCross (trInfo, seg) : false;
}

bool ifCross( const Segment& ft, const Segment& sd )
{
    Point lineSegCross = ft | Line{sd};
    return flatIsBelongs (sd, lineSegCross);
}

bool flatIfCross( const TriangleInfo& ft, const TriangleInfo& sd )
{
    return flatIfCross (ft, sd.AB_) || flatIfCross (ft, sd.BC_) ||
           flatIfCross (ft, sd.CA_) || flatIfCross (sd, ft.AB_);
}

bool flatIfCross( const TriangleInfo& tr, const Segment& seg )
{
    Line segLine = Line {seg};

    Point abCross = segLine | tr.AB_;
    Point bcCross = segLine | tr.BC_;
    Point caCross = segLine | tr.CA_;

    Segment abbcSeg = Segment {abCross, bcCross};
    Segment abcaSeg = Segment {abCross, caCross};
    Segment bccaSeg = Segment {bcCross, caCross};

    if (abCross.isValid ())
    {
        if (bcCross.isValid ())
        {
            if (caCross.isValid ()) // On one of triangle sides or don't cross.
            {
                return isEqual (abbcSeg.sqLen_, 0) ?
                    (flatIsBelongs (abcaSeg, seg.A_) ||
                    flatIsBelongs (abcaSeg, seg.B_)) :
                    (flatIsBelongs (abbcSeg, seg.A_) ||
                    flatIsBelongs (abbcSeg, seg.B_));
            }

            return flatIsBelongs (abbcSeg, seg.A_) ||
                   flatIsBelongs (abbcSeg, seg.B_);
        }

        return flatIsBelongs (abcaSeg, seg.A_) ||
               flatIsBelongs (abcaSeg, seg.B_);
    }

    return flatIsBelongs (bccaSeg, seg.A_) ||
           flatIsBelongs (bccaSeg, seg.B_);
}

bool flatIfCross( const TriangleInfo& tr, const Point& P )
{
    Line pa = Line {Segment {P, tr.AB_.A_}};
    if (!pa.isValid ())
        return true;

    Point cross = pa | tr.BC_;

    return Segment {cross, tr.AB_.A_}.sqLen_ > Segment {P, tr.AB_.A_}.sqLen_;
}

bool flatIsBelongs( const Segment& seg, const Point& point )
{
    return seg.sqLen_ >= sqDst (seg.A_, point) &&
           seg.sqLen_ >= sqDst (seg.B_, point);
}

bool isBelongs( const Plane& plane, const Point& P )
{
    const Vector& n = plane.normVec_;

    return isEqual (n.x_*P.x_ + n.y_+P.y_ + n.z_*P.z_ + plane.D_, 0);
}

} // namespace

} // namespace geom3D