
#include <gtest/gtest.h>

#include "geom3D.hh"
#include "geom3D-gen.hh"
#include "geom3D-split.hh"

namespace geom3D
{

bool SplittedTrsGroup::testSplitting()
{
    IndexedTrsGroup gr = genSmallTrsGroup (1000);

    bool testResult = true;
    PointSplitter splitter {gr};
    SubGroup splGr {nullptr, {}, SpaceDomain {gr}, {}, gr };

    splitGroup (&splGr);

    // Test sub groups space domains borders correctness.
    for (size_t childId = 0; childId < SUB_GROUPS_NUM; ++childId)
    {
        SubGroup* child = splGr.children_[childId];
        size_t childTrsNum = child->internalTrs_.size ();
        SpaceDomain childSD = child->spaceDomain_;

        for (size_t trId = 0; trId < childTrsNum; ++trId)
        {
            Triangle tr = child->internalTrs_[trId].first;
            for (size_t coordId = 0; coordId < DNUM; ++coordId)
            for (size_t pointId = 0; pointId < TR_POINT_NUM; ++pointId)
                if (tr[pointId][coordId] > childSD.upper ()[coordId] ||
                    tr[pointId][coordId] < childSD.lower ()[coordId])
                testResult = false;
        }
    }

    // Test border trs. 
    size_t borderSize = splGr.borderTrs_.size ();
    std::vector<bool> mask (borderSize, false);

    for (size_t i = 0; i < borderSize; ++i)
    {
        Triangle borderTr = splGr.borderTrs_[i].first;
        for (size_t childId = 0; childId < SUB_GROUPS_NUM; ++childId)
        {
            SubGroup* child = splGr.children_[childId];
            if (child->spaceDomain_.crosses (borderTr))
                mask[i] = true;
        }
    }

    for (size_t i = 0; i < borderSize; ++i)
        if (!mask[i]) testResult = false;

    for (size_t i = 0; i < SUB_GROUPS_NUM; ++i)
        delete splGr.children_[i];

    return testResult;
}

TEST( SplittingTests, GroupSplitTest )
{
    ASSERT_TRUE (SplittedTrsGroup::testSplitting ());
}

TEST( SplittingTests, SplittingEquivTest )
{
    IndexedTrsGroup gr = genSmallTrsGroup (1000);

    TrsIndexes idsWithNoSplit = cross (gr);
    SplittedTrsGroup spltGr (gr, 20);
    TrsIndexes idsWithSplit = spltGr.cross ();
    fp_t ratio = idsWithNoSplit.size () /
        static_cast<fp_t> (idsWithSplit.size ());

    ASSERT_TRUE ( fpCmpW {1} == ratio );
}

} // namespace geom3D