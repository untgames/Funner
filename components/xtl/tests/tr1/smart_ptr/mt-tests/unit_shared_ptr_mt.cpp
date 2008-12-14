#include "shared.h"

int const n = 1024 * 1024;

void test(xtl::shared_ptr<A> const & pi)
{
    stl::vector< xtl::shared_ptr<A> > v;

    for(int i = 0; i < n; ++i)
    {
        v.push_back(pi);
    }
}

int const m = 16; // threads

int main()
{
    printf ("Results of unit_shared_ptr_mt_test:\n");

    using namespace std;

    xtl::shared_ptr<A> pi(new A);

    pthread_t a[m];

    for(int i = 0; i < m; ++i)
    {
        a[i] = createThread( xtl::bind(test, pi) );
    }

    for(int j = 0; j < m; ++j)
    {
        pthread_join(a[j], 0);
    }

    return 0;
}
