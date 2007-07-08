#include <time.h>
#include <stl/vector>
#include "test.h"

using namespace stl;

int const n = 8 * 1024 * 1024;

int main()
{
    printf ("Results of shared_ptr_timing_test:\n");

    vector<shared_ptr<int> > v;
    shared_ptr<int> pi(new int);

    clock_t t = clock();

    for(int i = 0; i < n; ++i)
        v.push_back(pi);

    t = clock() - t;

    printf ("%.3f seconds\n", static_cast<double>(t) / CLOCKS_PER_SEC);

    return 0;
}
