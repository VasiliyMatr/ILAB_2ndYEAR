#include "cache-tests.hh"

namespace caches
{

size_t TestPage::missCounter_ = 0;

TestPage::TestPage( testPageId_t )
{
    ++missCounter_;
}

void TestPage::resetMissNum()
{
    missCounter_ = 0;
}

size_t TestPage::getMissNum()
{
    return missCounter_;
}

void test2QEfficiency( const char * filename )
{
    assert (filename != nullptr);

    std::ifstream in(filename);
    if (!in.is_open ())
    {
        std::cout << "Cannot open file " << filename <<std::endl;
        return;
    }
    std::streambuf * cinbuf = std::cin.rdbuf ();
    std::cin.rdbuf (in.rdbuf ());

    size_t hitsNum = caches::test2QEfficiency ();
    size_t expectedHitsNum = 0;
    std::cin >> expectedHitsNum;

    std::cin.rdbuf (cinbuf);

    static const char SET_FAIL_COLOR[] = "\033[0;31m";
    static const char SET_PASS_COLOR[] = "\033[0;32m";
    static const char RESET_COLOR[] = "\033[0m";

    std::cout << "Testing with input file " << '\"' << filename << '\"' << std::endl;
    if (expectedHitsNum != hitsNum)
    {
        std::cout << SET_FAIL_COLOR;
        std::cout << "Failed: ";
        std::cout << "expeced " << expectedHitsNum << " hits, ";
        std::cout << "but got " << hitsNum << " hits" << std::endl;
        std::cout << RESET_COLOR;
        return;
    }
    std::cout << SET_PASS_COLOR;
    std::cout << "Passed: " <<"Hits number = " << hitsNum << std::endl;
    std::cout << RESET_COLOR;
}

size_t test2QEfficiency()
{
    size_t cacheSize = 0;
    size_t inputSize = 0;

    std::cin >> cacheSize >> inputSize;

    testPageId_t * pIds = new testPageId_t[inputSize];
    for (size_t i = 0; i < inputSize; ++i)
        std::cin >> pIds[i];

    caches::Cache2Q<TestPage, testPageId_t> cache { cacheSize };
    TestPage::resetMissNum ();

    for (size_t i = 0; i < inputSize; ++i)
        cache.getPage (pIds [i]);

    delete [] pIds;

    return inputSize - TestPage::getMissNum ();
}

} // namespace caches