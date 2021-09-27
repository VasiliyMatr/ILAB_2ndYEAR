
#include "cache-tests.hh"

int main( int argc, char ** argv )
{
    for (int i = 1; i < argc; i++)
        caches::test2QEfficiency (argv[i]);

    return 0;
}