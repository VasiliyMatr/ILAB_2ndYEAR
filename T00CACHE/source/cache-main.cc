
#include "cache.hh"

int main()
{
    const pageId_t pIds[] = { 1, 2, 3, 4, 1, 2, 5, 1, 2, 4, 3, 4 };
    Cache<Page, pageId_t> cache {4};

    for (size_t i = 0; i < sizeof (pIds) / sizeof (pageId_t); ++i)
        cache.getPage (pIds [i]);

    return 0;
}