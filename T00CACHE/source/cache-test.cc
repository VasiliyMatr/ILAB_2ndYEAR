
#include "cache.hh"

int main( int argc, char ** argv )
{
    for (int i = 1; i < argc; i++)
        caches::Cache2Q<Page, pageId_t>::test (argv[i]);

    return 0;
}