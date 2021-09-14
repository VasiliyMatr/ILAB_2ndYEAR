
#include <cstdio>
#include <cstdlib>

#ifndef PAGE_HH_INCL
#define PAGE_HH_INCL

// something to store in cache
struct Page
{
    static const size_t PAGE_SIZE_ = 256;
    char placeHolder[PAGE_SIZE_] = "";
};

typedef __uint32_t pageId_t;

Page slowGetDataFunc (pageId_t pageId)
{
    return Page {};
}

#endif // PAGE_HH_INCL