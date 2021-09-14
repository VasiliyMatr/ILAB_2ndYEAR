
#include "../headers/cache.hh"

int main()
{
    size_t cacheSize = 0;
    size_t inputSize = 0;

    std::cin >> cacheSize;
    std::cin >> inputSize;

    pageId_t * pIds = new pageId_t[inputSize];
    for (size_t i = 0; i < inputSize; ++i)
        std::cin >> pIds[i];

    Cache<Page, pageId_t> cache {cacheSize};

    for (size_t i = 0; i < inputSize; ++i)
        cache.getPage (pIds [i]);

    delete pIds;
    return 0;
}