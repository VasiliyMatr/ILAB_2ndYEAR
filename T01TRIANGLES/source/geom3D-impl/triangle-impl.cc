
#include "geom3D.hh"

namespace geom3D
{

TriangleInfo::TriangleInfo( const Point& A, const Point& B, const Point& C ) :
    plane_ (A, B, C), isDegen_ (!plane_.isValid ()),
    AB_ ({A, B}), BC_ ({B, C}), CA_ ({C, A})
{
    if (isDegen_)
    {
        if (BC_.sqLen () > AB_.sqLen ())
        {
            if (BC_.sqLen () > CA_.sqLen ())
                AB_ = BC_;
            else
                AB_ = CA_;
        }
        else if (CA_.sqLen () > AB_.sqLen ())
            AB_ = CA_;
    }
}

Plane TriangleInfo::plane() const
{
    return plane_;
}

bool TriangleInfo::isDegen() const
{
    return isDegen_;
}

Segment TriangleInfo::AB() const
{
    return AB_;
}

Segment TriangleInfo::BC() const
{
    return BC_;
}

Segment TriangleInfo::CA() const
{
    return CA_;
}

namespace
{

// To check if two segments on one line are crossed.
bool linearAreCrossed( const Segment&, const Segment& );

// To check if objects crosses.
// For not degenerate triangles.
bool areCrossed( const TriangleInfo&, const TriangleInfo& );
// For not degenerate triangle.
bool areCrossed( const TriangleInfo&, const Segment& );
bool areCrossed( const Segment&, const Segment& );

// To check if objects crosses (for obj on one plane).
// For not degenerate triangles.
bool flatAreCrossed( const TriangleInfo&, const TriangleInfo& );
// For not degenerate triangle.
bool flatAreCrossed( const TriangleInfo&, const Segment& );
bool flatAreCrossed( const TriangleInfo&, const Point& );

} // namespace

bool TriangleInfo::crosses( const TriangleInfo& second ) const
{
    if (isDegen_)
    {
        if (second.isDegen_) 
            // Crossing 2 segments.
            return areCrossed (AB_, second.AB_);

        // Crossing segment & triangle.
        return areCrossed (second, AB_);
    }

    if (second.isDegen_)
        // Crossing segment & triangle.
        return areCrossed (*this, second.AB_);

    // Crossing 2 triangles.
    return areCrossed (*this, second);
}

namespace
{

bool areCrossed( const TriangleInfo& ft, const TriangleInfo& sd )
{
    Point abCross = ft.AB () | sd.plane ();
    Point bcCross = ft.BC () | sd.plane ();
    Point caCross = ft.CA () | sd.plane ();

    // Crossing segment & triangle?
    if (abCross.isValid ())
    {
        if (bcCross.isValid ())
            return flatAreCrossed (sd, Segment {abCross, bcCross});

        return flatAreCrossed (sd, Segment {abCross, caCross});
    }

    if (bcCross.isValid ())
        return flatAreCrossed (sd, Segment {bcCross, caCross});
    // Not segment & triangle case.

    if (sd.plane ().contains (ft.AB ().A ()))
        // Crossing 2 triangles in the same plane.
        return flatAreCrossed (ft, sd);

    return false;
}

bool areCrossed( const TriangleInfo& trInfo, const Segment& seg )
{
    Point segPlaneCross = Line {seg} | trInfo.plane ();
    if (segPlaneCross.isValid () && seg.linearContains (segPlaneCross))
        // Crossing point & triangle.
        return flatAreCrossed (trInfo, segPlaneCross);

    // Crossing segment & triangle.
    return trInfo.plane ().contains (seg.A ()) &&
           flatAreCrossed (trInfo, seg);
}

bool areCrossed( const Segment& ft, const Segment& sd )
{
    Line ftLine = Line {ft};
    Line sdLine = Line {sd};
    if (ftLine == sdLine)
        return linearAreCrossed (ft, sd);

    if (ftLine.isValid ())
    {
        if (sdLine.isValid ())
            return ft.linearContains (ftLine | sd);

        return ftLine.contains (sd.A ());
    }

    if (sdLine.isValid ())
        return sdLine.contains (ft.A ());

    return ft.A () == sd.A ();
}

bool flatAreCrossed( const TriangleInfo& ft, const TriangleInfo& sd )
{
    return flatAreCrossed (ft, sd.AB ()) || flatAreCrossed (ft, sd.BC ()) ||
           flatAreCrossed (ft, sd.CA ()) || flatAreCrossed (sd, ft.AB ());
}

bool flatAreCrossed( const TriangleInfo& tr, const Segment& seg )
{
    Line segLine = Line {seg};
    if (!segLine.isValid ())
        return flatAreCrossed (tr, seg.A ());

    Point abCross = segLine | tr.AB ();
    Point bcCross = segLine | tr.BC ();
    Point caCross = segLine | tr.CA ();

    Segment abbcSeg = Segment {abCross, bcCross};
    Segment abcaSeg = Segment {abCross, caCross};
    Segment bccaSeg = Segment {bcCross, caCross};

    if (abCross.isValid ())
    {
        if (bcCross.isValid ())
        {
            if (caCross.isValid ())
                // segLine crossed triangle vertex & opposite segment.
                return isEqual (abbcSeg.sqLen (), 0) ?
                       linearAreCrossed (abcaSeg, seg) :
                       linearAreCrossed (abbcSeg, seg);

            return linearAreCrossed (abbcSeg, seg);
        }

        return linearAreCrossed (abcaSeg, seg);
    }

    return bcCross.isValid () && linearAreCrossed (bccaSeg, seg);
}

bool flatAreCrossed( const TriangleInfo& tr, const Point& P )
{
    const Point& A = tr.AB ().A ();
    Line PA = Line {Segment {P, A}};
    if (!PA.isValid ())// P == A
        return true;

    return Segment {PA | tr.BC (), A}.linearContains (P);
}

bool linearAreCrossed( const Segment& ft, const Segment& sd )
{
    return ft.linearContains (sd.A ()) ||
           ft.linearContains (sd.B ()) ||
           sd.linearContains (ft.A ());
}

} // namespace

} // namespace geom3D