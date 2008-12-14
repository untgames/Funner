#include "shared.h"

int const n = 1024 * 1024;

void test(xtl::intrusive_ptr<B> const & pi)
{
    stl::vector< xtl::intrusive_ptr<B> > v;

    for(int i = 0; i < n; ++i)
    {
        v.push_back(pi);
    }
}

int const m = 16; // threads

int main()
{
    printf ("Results of unit_reference_counter_mt_test:\n");

    using namespace std;

    xtl::intrusive_ptr<B> pi(new B, false);

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
