
#include <cstdio>
#include <cstdlib>

#ifndef PAGE_HH_INCL
#define PAGE_HH_INCL

typedef __uint32_t pageId_t;

// something to store in cache
struct Page
{
    static const size_t PAGE_SIZE_ = 256;
    char placeHolder[PAGE_SIZE_] = "";

    pageId_t id_ = 0;
};

Page slowGetDataFunc (pageId_t pageId)
{
    Page page {};
    page.id_ = pageId;

    return page;
}

#endif // PAGE_HH_INCL