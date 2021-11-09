
#include "geom3D.hh"

namespace geom3D
{

Triangle::Triangle( const Point& A, const Point& B, const Point& C ) :
    plane_ (A, B, C), isDegen_ (!plane_.isValid ()),
    AB_ ({A, B}), BC_ ({B, C}), CA_ ({C, A})
{
    if (!(Line {AB_} | Line {BC_}).isValid () ||
        !(Line {AB_} | Line {CA_}).isValid ())
        isDegen_ = true;

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

namespace
{

// To check if two segments on one line are crossed.
bool linearAreCrossed( const Segment&, const Segment& );

// To check if objects crosses.
// For not degenerate triangles.
bool areCrossed( const Triangle&, const Triangle& );
// For not degenerate triangle.
bool areCrossed( const Triangle&, const Segment& );
bool areCrossed( const Segment&, const Segment& );

// To check if objects crosses (for obj on one plane).
// For not degenerate triangles.
bool flatAreCrossed( const Triangle&, const Triangle& );
// For not degenerate triangle.
bool flatAreCrossed( const Triangle&, const Segment& );
bool flatAreCrossed( const Triangle&, const Point& );

} // namespace

bool Triangle::crosses( const Triangle& second ) const
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

bool areCrossed( const Triangle& ft, const Triangle& sd )
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

    if (sd.plane ().contains (ft[0]) &&
        sd.plane ().contains (ft[1]) &&
        sd.plane ().contains (ft[2]))
        // Crossing 2 triangles in the same plane.
        return flatAreCrossed (ft, sd);

    return false;
}

bool areCrossed( const Triangle& tr, const Segment& seg )
{
    Point segPlaneCross = seg | tr.plane ();
    if (segPlaneCross.isValid ())
        // Crossing point & triangle.
        return flatAreCrossed (tr, segPlaneCross);

    // Crossing segment & triangle.
    return tr.plane ().contains (seg.P1 ()) &&
           flatAreCrossed (tr, seg);
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

        return ftLine.contains (sd.P1 ());
    }

    if (sdLine.isValid ())
        return sdLine.contains (ft.P1 ());

    return ft.P1 () == sd.P1 ();
}

bool flatAreCrossed( const Triangle& ft, const Triangle& sd )
{
    return flatAreCrossed (ft, sd.AB ()) || flatAreCrossed (ft, sd.BC ()) ||
           flatAreCrossed (ft, sd.CA ()) || flatAreCrossed (sd, ft.AB ());
}

bool flatAreCrossed( const Triangle& tr, const Segment& seg )
{
    Line segLine = Line {seg};
    if (!segLine.isValid ())
        return flatAreCrossed (tr, seg.P1 ());

    Point abCross = segLine | tr.AB ();
    Point bcCross = segLine | tr.BC ();
    Point caCross = segLine | tr.CA ();

    Segment abbcSeg = Segment {abCross, bcCross};
    Segment abcaSeg = Segment {abCross, caCross};
    Segment bccaSeg = Segment {bcCross, caCross};

    if (abCross.isValid ())
    {
        if (bcCross.isValid ())
            return linearAreCrossed (abbcSeg, seg);

        return linearAreCrossed (abcaSeg, seg);
    }

    if (bcCross.isValid ())
            return linearAreCrossed (bccaSeg, seg);

    return false;
}

bool flatAreCrossed( const Triangle& tr, const Point& P )
{
    const Point& A = tr[0];
    Line PA = Line {Segment {P, A}};
    if (!PA.isValid ())
        return true; // P == A

    return Segment {PA | tr.BC (), A}.linearContains (P);
}

bool linearAreCrossed( const Segment& ft, const Segment& sd )
{
    return ft.linearContains (sd.P1 ()) ||
           ft.linearContains (sd.P2 ()) ||
           sd.linearContains (ft.P1 ());
}

} // namespace

} // namespace geom3D