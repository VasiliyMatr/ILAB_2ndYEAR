
#include "../headers/cache.hh"

int main()
{
    size_t AM_Size = 0;
    size_t ALin_Size = 0;
    size_t ALout_Size = 0;
    size_t inputSize = 0;

    std::cin >> AM_Size;
    std::cin >> ALin_Size;
    std::cin >> ALout_Size;
    std::cin >> inputSize;

    pageId_t * pIds = new pageId_t[inputSize];
    for (size_t i = 0; i < inputSize; ++i)
        std::cin >> pIds[i];

    caches::Cache2Q<Page, pageId_t> cache { AM_Size, ALin_Size, ALout_Size };

    for (size_t i = 0; i < inputSize; ++i)
        cache.getPage (pIds [i]);

    delete pIds;
    return 0;
}