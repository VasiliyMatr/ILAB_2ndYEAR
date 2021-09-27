
#include <cstdio>
#include <cstdlib>

#ifndef CACHE_TESTS_HH_INCL
#define CACHE_TESTS_HH_INCL

#include "cache.hh"

namespace caches
{

using testPageId_t = __uint32_t;

// Something to store in cache.
class TestPage
{
    static const size_t PAGE_SIZE_ = 64;
    char placeHolder[PAGE_SIZE_] = "";
    static size_t missCounter_;

public:

    // Func to get page from remote storage. Can be wery slow.
    TestPage( testPageId_t );
    static void resetMissNum();
    static size_t getMissNum();

};

// 2Q Efficiency tests stuff.
    // To test with data from stdin. Format for data:
    //     <CACHE CAPACITY> <SEQUENCE SIZE> <SEQUENCE>
    //
    //     Silently returns number of hits for given inputs.
    size_t test2QEfficiency();

    // To test with data from file. Format for test data:
    //     <CACHE CAPACITY> <SEQUENCE SIZE> <SEQUENCE> <EXPECTED NUM OF HITS>
    //
    //     Prints test result in stdin.
    void test2QEfficiency( const char * filename );

} // namespace caches

#endif // #ifndef CACHE_TESTS_HH_INCL