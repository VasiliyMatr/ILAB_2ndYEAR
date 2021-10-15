
#include "geom3D.hh"

namespace geom3D
{

TriangleInfo::TriangleInfo( const Point& A, const Point& B, const Point& C ) :
    plane_ (A, B, C), isDegen_ (!plane_.isValid ()),
    AB_ ({A, B}), BC_ ({B, C}), CA_ ({C, A}),
    maxLenSeg_ (AB_.sqLen_ > BC_.sqLen_ ?
               (AB_.sqLen_ > CA_.sqLen_ ? AB_ : CA_) :
               (BC_.sqLen_ > CA_.sqLen_ ? BC_ : CA_))
{}

namespace
{

// To check if two segments on one line are crossed.
bool linearIfCross( const Segment&, const Segment& );

// To check if objects crosses.
// For not degenerate triangles.
bool ifCross( const TriangleInfo&, const TriangleInfo& );
// For not degenerate triangle.
bool ifCross( const TriangleInfo&, const Segment& );
bool ifCross( const Segment&, const Segment& );

// To check if objects crosses (for obj on one plane).
// For not degenerate triangles.
bool flatIfCross( const TriangleInfo&, const TriangleInfo& );
// For not degenerate triangle.
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
    Point abCross = ft.AB_ | sd.plane_;
    Point bcCross = ft.BC_ | sd.plane_;
    Point caCross = ft.CA_ | sd.plane_;

    // Crossing segment & triangle?
    if (abCross.isValid ())
    {
        if (bcCross.isValid ())
            return flatIfCross (sd, Segment {abCross, bcCross});

        return flatIfCross (sd, Segment {abCross, caCross});
    }

    if (bcCross.isValid ())
        return flatIfCross (sd, Segment {bcCross, caCross});
    // Not segment & triangle case.

    if (sd.plane_.contains (ft.AB_.A_))
        // Crossing 2 triangles in the same plane.
        return flatIfCross (ft, sd);

    return false;
}

bool ifCross( const TriangleInfo& trInfo, const Segment& seg )
{
    Point segPlaneCross = Line {seg} | trInfo.plane_;
    if (segPlaneCross.isValid () && seg.linearContains (segPlaneCross))
        // Crossing point & triangle.
        return flatIfCross (trInfo, segPlaneCross);

    // Crossing segment & triangle.
    return trInfo.plane_.contains (seg.A_) &&
           flatIfCross (trInfo, seg);
}

bool ifCross( const Segment& ft, const Segment& sd )
{
    Line ftLine = Line {ft};
    Line sdLine = Line {sd};
    if (ftLine == sdLine)
        return linearIfCross (ft, sd);

    if (ftLine.isValid ())
    {
        if (sdLine.isValid ())
            return ft.linearContains (ftLine | sd);

        return ftLine.contains (sd.A_);
    }

    if (sdLine.isValid ())
        return sdLine.contains (ft.A_);

    return ft.A_ == sd.A_;
}

bool flatIfCross( const TriangleInfo& ft, const TriangleInfo& sd )
{
    return flatIfCross (ft, sd.AB_) || flatIfCross (ft, sd.BC_) ||
           flatIfCross (ft, sd.CA_) || flatIfCross (sd, ft.AB_);
}

bool flatIfCross( const TriangleInfo& tr, const Segment& seg )
{
    Line segLine = Line {seg};
    if (!segLine.isValid ())
        return flatIfCross (tr, seg.A_);

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
            if (caCross.isValid ())
                // segLine crossed triangle vertex & opposite segment.
                return isEqual (abbcSeg.sqLen_, 0) ?
                       linearIfCross (abcaSeg, seg) :
                       linearIfCross (abbcSeg, seg);

            return linearIfCross (abbcSeg, seg);
        }

        return linearIfCross (abcaSeg, seg);
    }

    return bcCross.isValid () && linearIfCross (bccaSeg, seg);
}

bool flatIfCross( const TriangleInfo& tr, const Point& P )
{
    const Point& A = tr.AB_.A_;
    Line PA = Line {Segment {P, A}};
    if (!PA.isValid ())// P == A
        return true;

    return Segment {PA | tr.BC_, A}.linearContains (P);
}

bool linearIfCross( const Segment& ft, const Segment& sd )
{
    return ft.linearContains (sd.A_) ||
           ft.linearContains (sd.B_) ||
           sd.linearContains (ft.A_);
}

} // namespace

} // namespace geom3D