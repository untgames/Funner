#include "shared.h"

int const n = 16384;
int const k = 512; // vector size
int const m = 16; // threads

void test( stl::vector< xtl::shared_ptr<A> > & v )
{
    using namespace std; // printf, rand

    stl::vector< xtl::weak_ptr<A> > w( v.begin(), v.end() );

    int s = 0, f = 0, r = 0;

    for( int i = 0; i < n; ++i )
    {
        // randomly kill a pointer

        v[ rand() % k ].reset();
        ++s;

        for( int j = 0; j < k; ++j )
        {
            if( xtl::shared_ptr<A> px = w[ j ].lock() )
            {
                ++s;

                if( rand() & 4 )
                {
                    continue;
                }

                // rebind anyway with prob. 50% for add_ref_lock() against weak_release() contention
                ++f;
            }
            else
            {
                ++r;
            }

            w[ j ] = v[ rand() % k ];
        }
    }
}

int main()
{
    printf ("Results of weak_ptr_mt_test:\n");

    using namespace std; // printf, clock_t, clock

    stl::vector< xtl::shared_ptr<A> > v( k );

    for( int i = 0; i < k; ++i )
    {
        v[ i ].reset( new A);
    }

    pthread_t a[m];

    for(int i = 0; i < m; ++i)
    {
        a[i] = createThread( xtl::bind( test, v ) );
    }

    v.resize( 0 ); // kill original copies

    for(int j = 0; j < m; ++j)
    {
        pthread_join( a[j], 0 );
    }

    return 0;
}
