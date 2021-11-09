
#include "geom3D.hh"
#include "geom3D-split.hh"
#include "geom3D-tests.hh"

namespace geom3D
{

TEST( SplittingTest, SplittingTest )
{
    static constexpr size_t trNum = 2000;
    IndexedTrsGroup gr{};

    for (size_t i = 0; i < trNum; ++i)
    {
        Point P = genP ();
        Triangle newTr {P, P + genSmallVec (), P + genSmallVec ()};
        gr.push_back ({newTr, i});
    }

    TrsIndexes idsWithNoSplit = cross (gr);
    SplittedTrsGroup spltGr (gr, 20);
    TrsIndexes idsWithSplit = spltGr.cross ();

    ASSERT_TRUE (idsWithNoSplit.size () == idsWithSplit.size ());
}

} // namespace geom3D