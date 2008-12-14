#include "shared.h"

int const n = 1024 * 1024;

void test(xtl::shared_ptr<int> const & pi)
{
    stl::vector< xtl::shared_ptr<int> > v;

    for(int i = 0; i < n; ++i)
    {
        v.push_back(pi);
    }
}

int const m = 16; // threads

int main()
{
    printf ("Results of shared_ptr_mt_test:\n");

    using namespace std; // printf, clock_t, clock

    printf("%s: %d threads, %d iterations: ", title, m, n);

    xtl::shared_ptr<int> pi(new int(42));

    clock_t t = clock();

    pthread_t a[m];

    for(int i = 0; i < m; ++i)
    {
        a[i] = createThread( xtl::bind(test, pi) );
    }

    for(int j = 0; j < m; ++j)
    {
        pthread_join(a[j], 0);
    }

    t = clock() - t;

    printf("\n\n%.3f seconds.\n", static_cast<double>(t) / CLOCKS_PER_SEC);

    return 0;
}
