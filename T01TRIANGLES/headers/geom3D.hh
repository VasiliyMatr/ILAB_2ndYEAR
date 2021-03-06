
#include <array>
#include <cassert>
#include <cmath>
#include <compare>
#include <iostream>
#include <limits>
#include <vector>

#ifndef GEOM3D_HH_INCL
#define GEOM3D_HH_INCL

namespace geom3D
{

// Choosen floating point type for all geom3D functionallity.
using fp_t = float;

constexpr fp_t nan = std::numeric_limits<fp_t>::quiet_NaN();
constexpr fp_t inf = std::numeric_limits<fp_t>::infinity();

inline bool isValid(fp_t value) noexcept
{
    return !(std::isnan(value) || std::isinf(value));
}

// Used for comparsions with precision.
enum class valueOrder_t : size_t
{
    FIRST = 1,
    SECOND,
    THIRD
};

/* Wrapper for fp_t comparsions with precision.

   ORDER parameter is used to compare with different precision.
   valueOrder_t::SECOND and valueOrder_t::THIRD parameter values
   could be usefull to compare quantities with area or volume dimensions.
*/
template <valueOrder_t ORDER = valueOrder_t::FIRST> struct fpCmpW
{
    fp_t val_ = nan;

    // Cmp precision stuff.
    static constexpr fp_t pow(int power, fp_t val)
    {
        fp_t result = 1;

        for (int i = 0; i < std::abs(power); ++i)
            result *= val;
        return power >= 0 ? result : 1 / result;
    }
    static constexpr fp_t BASE_CMP_PRECISION = 0.001;
    static constexpr fp_t CMP_PRECISION = pow(size_t(ORDER), BASE_CMP_PRECISION);

    fpCmpW(fp_t val = 0) : val_(val)
    {
    }

    bool operator==(fpCmpW sd) const noexcept
    {
        return std::abs(val_ - sd.val_) <= CMP_PRECISION;
    }

    std::partial_ordering operator<=>(fpCmpW sd) const noexcept
    {
        if (*this == sd)
            return std::partial_ordering::equivalent;

        return val_ <=> sd.val_;
    }
};

/* Tag rule for all functions that works with IGeomPrimitive derives:
   If function could return valid result (or true in bool functions case)
   for invalid input it is considered WARNED.
*/
// Base for all geometrical primitives.
struct GeomPrimitive
{
};

// Number of dimensions.
constexpr size_t DNUM = 3;

using Coordinates = std::array<fp_t, DNUM>;

// Space coordinates ids for Coordinates alias.
enum coordId_t : size_t
{
    X,
    Y,
    Z
};

struct Point : GeomPrimitive
{
    Coordinates coord_{nan, nan, nan};

    bool isValid() const noexcept
    {
        return geom3D::isValid(coord_[X]) && geom3D::isValid(coord_[Y]) && geom3D::isValid(coord_[Z]);
    }

    bool isConsistent() const noexcept
    {
        return true;
    }

    Point(fp_t x, fp_t y, fp_t z) : coord_{x, y, z}
    {
    }

    // Default constructed point is invalid.
    Point()
    {
        assert(!isValid());
    }

    virtual ~Point()
    {
    }

    fp_t &operator[](size_t coordId) noexcept
    {
        return coord_[coordId % DNUM];
    }

    const fp_t &operator[](size_t coordId) const noexcept
    {
        return coord_[coordId % DNUM];
    }

    operator Coordinates() const
    {
        return coord_;
    }
};

// WARNED.
inline bool operator==(const Point &ft, const Point &sd)
{
    return fpCmpW{ft[X]} == sd[X] && fpCmpW{ft[Y]} == sd[Y] && fpCmpW{ft[Z]} == sd[Z];
}

inline fp_t sqDst(const Point &ft, const Point &sd)
{
    fp_t dst = 0;
    for (size_t i = 0; i < DNUM; ++i)
        dst += (ft[i] - sd[i]) * (ft[i] - sd[i]);

    return dst;
}

struct Vector final : GeomPrimitive
{
    Coordinates coord_{nan, nan, nan};

    bool isValid() const noexcept
    {
        return geom3D::isValid(coord_[X]) && geom3D::isValid(coord_[Y]) && geom3D::isValid(coord_[Z]);
    }

    bool isConsistent() const noexcept
    {
        return true;
    }

    Vector(fp_t x, fp_t y, fp_t z) : coord_{x, y, z}
    {
    }

    Vector(const Point &P1, const Point &P2) : coord_{P2[X] - P1[X], P2[Y] - P1[Y], P2[Z] - P1[Z]}
    {
    }

    // Default constructed vector is invalid.
    Vector()
    {
        assert(!isValid());
    }

    fp_t &operator[](size_t coordId) noexcept
    {
        return coord_[coordId % DNUM];
    }

    const fp_t &operator[](size_t coordId) const noexcept
    {
        return coord_[coordId % DNUM];
    }

    Vector operator-() const
    {
        return {-coord_[X], -coord_[Y], -coord_[Z]};
    }

    Vector operator+=(const Vector &sd)
    {
        for (size_t i = 0; i < DNUM; ++i)
            coord_[i] += sd[i];

        return *this;
    }

    Vector operator-=(const Vector &sd)
    {
        return *this += (-sd);
    }

    Vector operator*=(fp_t num)
    {
        for (size_t i = 0; i < DNUM; ++i)
            coord_[i] *= num;

        return *this;
    }

    // WARNED
    Vector operator/=(fp_t num)
    {
        return *this *= 1 / num;
    }

    fp_t sqLen() const noexcept
    {
        return coord_[X] * coord_[X] + coord_[Y] * coord_[Y] + coord_[Z] * coord_[Z];
    }

    fp_t len() const
    {
        return std::sqrt(sqLen());
    }

    // Cheap scaling is used to fix precision problems.
    fp_t scale()
    {
        fp_t divider = 0;
        for (size_t i = 0; i < DNUM; ++i)
            divider = std::max(divider, std::abs(coord_[i]));

        fp_t factor = 1 / divider;
        *this *= factor;
        return factor;
    }

    static fp_t scalarProduct(const Vector &ft, const Vector &sd) noexcept
    {
        return ft[X] * sd[X] + ft[Y] * sd[Y] + ft[Z] * sd[Z];
    }

    static Vector crossProduct(const Vector &ft, const Vector &sd)
    {
        return {ft[Y] * sd[Z] - ft[Z] * sd[Y], -ft[X] * sd[Z] + ft[Z] * sd[X], ft[X] * sd[Y] - ft[Y] * sd[X]};
    }

    // Few useful Vector constants.
    static Vector e1()
    {
        return {1, 0, 0};
    }
    static Vector e2()
    {
        return {0, 1, 0};
    }
    static Vector e3()
    {
        return {0, 0, 1};
    }
    static Vector zero()
    {
        return {0, 0, 0};
    }
};

inline Point operator+(const Point &P, const Vector &vec)
{
    Point toRet{P};
    for (size_t i = 0; i < DNUM; ++i)
        toRet[i] += vec[i];
    return toRet;
}

inline Vector operator+(const Vector &ft, const Vector &sd)
{
    Vector copy{ft};
    return copy += sd;
}

inline Vector operator-(const Vector &ft, const Vector &sd)
{
    return ft + (-sd);
}

inline Vector operator*(const Vector &vec, fp_t num)
{
    Vector copy{vec};
    return copy *= num;
}

inline Vector operator*(fp_t num, const Vector &vec)
{
    return vec * num;
}

// WARNED.
inline Vector operator/(const Vector &vec, fp_t num)
{
    return vec * (1 / num);
}

// WARNED.
inline bool operator==(const Vector &ft, const Vector &sd)
{
    return fpCmpW{ft[X]} == sd[X] && fpCmpW{ft[Y]} == sd[Y] && fpCmpW{ft[Z]} == sd[Z];
}

// Calculates 3x3 determinant.
inline fp_t det(const Vector &a, const Vector &b, const Vector &c) noexcept
{
    return a[X] * (b[Y] * c[Z] - b[Z] * c[Y]) - a[Y] * (b[X] * c[Z] - b[Z] * c[X]) + a[Z] * (b[X] * c[Y] - b[Y] * c[X]);
}

class Segment;
class Plane;

// Lines are stored as point + direction vector
class Line final : public GeomPrimitive
{
    Vector dir_;
    Point P_;

  public:
    bool isValid() const noexcept
    {
        return dir_.isValid() && P_.isValid();
    }

    bool isConsistent() const noexcept
    {
        return !isValid() || dir_ != Vector::zero();
    }

    Vector dir() const
    {
        return dir_;
    }

    Point P() const
    {
        return P_;
    }

    // Line is invalid if dir == Vector::zero ()
    Line(const Vector &dir, const Point &P) : dir_(dir), P_(P)
    {
        if (dir_ == Vector::zero())
        {
            dir_ = Vector{};
        }
        else
        {
            dir_.scale();
        }

        assert(isConsistent());
    }

    // Default constructed Line is invalid.
    Line()
    {
        assert(!isValid());
    }

    // Does this line contains the point?
    // WARNED.
    bool contains(const Point &toCheck) const
    {
        return fpCmpW{(P_[X] - toCheck[X]) * dir_[Y]} == (P_[Y] - toCheck[Y]) * dir_[X] &&
               fpCmpW{(P_[Y] - toCheck[Y]) * dir_[Z]} == (P_[Z] - toCheck[Z]) * dir_[Y];
    }

    // Is this line parallel to the second line?
    bool parallelTo(const Line &sd) const
    {
        assert(!isValid() || dir_ != Vector::zero());
        return Vector::crossProduct(dir_, sd.dir_) == Vector::zero();
    }

    // Returns lines cross.
    // If lines are equal or parallel or skew - returns invalid point.
    Point operator|(const Line &) const;

    // Returns lines and planes cross.
    // If there is infinite number of solutions or no solutions - returns invalid
    // point. WARNED.
    Point operator|(const Plane &) const;

    // Returns lines and segments cross.
    // If there is infinite number of solutions, or no solutions,
    // or Segment length is zero - returns invalid point.
    // WARNED.
    Point operator|(const Segment &) const;

    // WARNED.
    bool operator==(const Line &sd) const
    {
        assert(!isValid() || dir_ != Vector::zero());
        return sd.contains(P_) && sd.contains(P_ + dir_);
    }
};

// Segments are stored as 2 Points + segment stores
// self square length (square length is used frequently).
class Segment final : public GeomPrimitive
{
    // sqLen_ == sqDst(P1_, P2_) is a protected invariant.
    Point P1_;
    Point P2_;
    fp_t sqLen_ = nan;

  public:
    bool isValid() const noexcept
    {
        return P1_.isValid() && P2_.isValid() && geom3D::isValid(sqLen_);
    }

    bool isConsistent() const noexcept
    {
        return !isValid() || fpCmpW<valueOrder_t::SECOND>{sqLen_} == sqDst(P1_, P2_);
    }

    Point P1() const
    {
        return P1_;
    }

    Point P2() const
    {
        return P2_;
    }

    fp_t sqLen() const noexcept
    {
        return sqLen_;
    }

    Segment(const Point &P1, const Point &P2) : P1_(P1), P2_(P2), sqLen_(sqDst(P1, P2))
    {
        assert(isConsistent());
    }

    // Default constructed Segment is invalid.
    Segment()
    {
        assert(!isValid());
    }

    operator Line() const
    {
        Line toRet{Vector{P1_, P2_}, P1_};
        assert(toRet.isConsistent());
        return toRet;
    }

    // Does this segment contains the point?
    // Works properly only for point & segment on one line.
    // WARNED.
    bool linearContains(const Point &P) const
    {
        return (P == P1_ || P == P2_) || (sqLen_ > sqDst(P, P1_) && sqLen_ > sqDst(P, P2_));
    }

    // WARNED.
    Point operator|(const Line &line) const
    {
        return line | *this;
    }
};

// n_[X]*x + n_[Y]*y + n_[Z]*z + D = 0
class Plane final : public GeomPrimitive
{
    // n_ != Vector::zero () is a protected invariant.
    Vector n_;
    fp_t D_ = nan;

  public:
    bool isValid() const noexcept
    {
        return n_.isValid() && geom3D::isValid(D_);
    }

    bool isConsistent() const noexcept
    {
        return !isValid() || n_ != Vector::zero();
    }

    Vector n() const
    {
        return n_;
    }

    fp_t D() const noexcept
    {
        return D_;
    }

    fp_t eVal(const Point &P) const noexcept
    {
        return n_[X] * P[X] + n_[Y] * P[Y] + n_[Z] * P[Z] + D_;
    }

    // SAFE.
    // Plane is invalid if n_ == Vector::zero ().
    Plane(const Vector &n, fp_t D) : n_(n), D_(D)
    {
        if (n_ == Vector::zero())
        {
            n_ = Vector{};
        }
        else
        {
            fp_t factor = n_.scale();
            D_ *= factor;
        }

        assert(isConsistent());
    }

    // Plane is invalid if points lies on one line.
    Plane(const Point &A, const Point &B, const Point &C) : n_(Vector::crossProduct({A, B}, {B, C})), D_(0)
    {
        if (n_ == Vector::zero())
        {
            n_ = Vector{};
        }
        else
        {
            n_.scale();
            D_ = -eVal(A);
        }

        assert(isConsistent());
    }

    // Default constructed Plane is invalid.
    Plane()
    {
        assert(!isValid());
    }

    bool contains(const Point &P) const noexcept
    {
        return fpCmpW{} == eVal(P);
    }

    // WARNED.
    Point operator|(const Line &line) const
    {
        return line | *this;
    }
};

// Returns segment and plane cross.
// If there is infinite number of solutions, or no solutions,
// or Segment length is zero - returns invalid point.
// WARNED.
inline Point operator|(const Plane &plane, const Segment &seg)
{
    Point P1 = seg.P1();
    Point P2 = seg.P2();
    fp_t P1EVal = plane.eVal(P1);
    fp_t P2EVal = plane.eVal(P2);

    if (fpCmpW{} == P1EVal)
    {
        if (fpCmpW{} == P2EVal && P1 != P2)
            return Point{};
        return P1;
    }

    if (fpCmpW{} == P2EVal)
        return P2;

    if (P1EVal * P2EVal > 0)
        return Point{};

    fp_t dstSum = std::abs(P1EVal) + std::abs(P2EVal);
    return P1 + Vector{P1, P2} * std::abs(P1EVal) / dstSum;
}

inline Point operator|(const Segment &seg, const Plane &plane)
{
    return plane | seg;
}

// Expected, right?
constexpr size_t TR_POINT_NUM = 3;

// Stored triangle info - not a geometrical primitive.
class Triangle final : public GeomPrimitive
{
    // Many invariants to protect.
    Plane plane_;
    bool isDegen_ = true;
    // Max segment is stored in AB_ for degenerate triangles.
    // Other segments can contain any Point equal to ctor args.
    Segment AB_; // ft + sd point (in this order) - for not degen.
    Segment BC_; // sd + tr point (in this order) - for not degen.
    Segment CA_; // tr + ft point (in this order) - for not degen.

  public:
    bool isValid() const noexcept
    {
        return AB_.isValid() && BC_.isValid() && CA_.isValid();
    }

    bool isConsistent() const noexcept;

    Plane plane() const
    {
        return plane_;
    }

    bool isDegen() const
    {
        return isDegen_;
    }

    Segment AB() const
    {
        return AB_;
    }

    Segment BC() const
    {
        return BC_;
    }

    Segment CA() const
    {
        return CA_;
    }

    Triangle(const Point &, const Point &, const Point &);

    Triangle()
    {
        assert(isConsistent());
    }

    // Any return value for invalid or half-invalid points.
    bool crosses(const Triangle &) const;

    Point operator[](size_t pointId) const
    {
        pointId %= TR_POINT_NUM;

        if (pointId == 0)
            return AB_.P1();
        if (pointId == 1)
            return AB_.P2();
        return BC_.P2();
    }

    // Returns mass center coords.
    operator Coordinates() const
    {
        Coordinates toRet{nan, nan, nan};

        for (size_t i = 0; i < DNUM; ++i)
            toRet[i] = (AB_.P1()[i] + AB_.P2()[i] + BC_.P2()[i]) / TR_POINT_NUM;

        return toRet;
    }
};

// Triangles will be splitted in smaller groups for
// asymptotic computational complexity reduction.
using IndexedTrsGroup = std::vector<std::pair<Triangle, size_t>>;
using TrsIndexes = std::vector<size_t>;

} // namespace geom3D

#endif // #ifndef GEOM3D_HH_INCL
