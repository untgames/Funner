#include <time.h>
#include <stdlib.h>
#include <stl/vector>

#include "test.h"

using namespace stl;

int const n = 29000;
int const k = 2048;

void test (vector<shared_ptr<int> > & v )
{
    vector<weak_ptr<int> > w ( v.begin(), v.end() );

    int s = 0, r = 0;

    for( int i = 0; i < n; ++i )
    {
        // randomly kill a pointer

        v[ rand() % k ].reset();

        for( int j = 0; j < k; ++j )
        {
            if(shared_ptr<int> px = w[ j ].lock() )
            {
                ++s;
            }
            else
            {
                ++r;
                w[ j ] = v[ rand() % k ];
            }
        }
    }

    printf( "\n%d locks, %d rebinds.", s, r );
}

int main()
{
    printf ("Results of weak_ptr_timing_test:\n");

    vector<shared_ptr<int> > v( k );

    for( int i = 0; i < k; ++i )
    {
        v [i].reset (new int (0));
    }

    clock_t t = clock();

    test( v );

    t = clock() - t;

    printf ("\n\n%.3f seconds.\n", static_cast<double>( t ) / CLOCKS_PER_SEC );

    return 0;
}
