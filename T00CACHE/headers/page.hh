
#include <cstdio>
#include <cstdlib>

#ifndef PAGE_HH_INCL
#define PAGE_HH_INCL

typedef __uint32_t pageId_t;

// something to store in cache
struct Page
{
    static const size_t PAGE_SIZE_ = 64;
    char placeHolder[PAGE_SIZE_] = "";
};

Page slowGetDataFunc (pageId_t pageId)
{
    return Page{};
}

#endif // PAGE_HH_INCL