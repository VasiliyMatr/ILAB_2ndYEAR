
#ifndef GEOM3D_TESTS_HH_INCL
#define GEOM3D_TESTS_HH_INCL

#include "geom3D.hh"

namespace geom3D
{

// Bounds for gen values.
constexpr fp_t MAX_TEST_FP_VAL = 12000;
constexpr fp_t MIN_TEST_FP_VAL = -12000;

// Generates fp_t value in range:
// [MIN_TEST_FP_VAL; MAX_TEST_FP_VAL] \ {0}
inline fp_t genFP()
{
    fp_t positivePart = MAX_TEST_FP_VAL * (static_cast<fp_t>(std::rand()) / RAND_MAX);
    fp_t negativePart = MIN_TEST_FP_VAL * (static_cast<fp_t>(std::rand()) / RAND_MAX);

    fp_t sum = positivePart + negativePart;

    return fpCmpW{} == sum ? genFP() : sum;
}

inline Point genP()
{
    return Point{genFP(), genFP(), genFP()};
}

inline Vector genVec()
{
    return Vector{genFP(), genFP(), genFP()};
}

// To gen point near (0,0,0).
constexpr fp_t SMALL_FACTOR = 80 / (MAX_TEST_FP_VAL - MIN_TEST_FP_VAL);
inline Point genCloseP()
{
    return Point{0, 0, 0} + genVec() * SMALL_FACTOR;
}

inline Triangle genSmallTr()
{
    return Triangle{genCloseP(), genCloseP(), genCloseP()};
}

inline IndexedTrsGroup genSmallTrsGroup(size_t trNum)
{
    IndexedTrsGroup gr{};
    for (size_t i = 0; i < trNum; ++i)
        gr.push_back({genSmallTr(), i});

    return gr;
}

} // namespace geom3D

#endif // #ifndef GEOM3D_TESTS_HH_INCL
