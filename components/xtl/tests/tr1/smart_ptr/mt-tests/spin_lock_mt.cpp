#include "shared.h"

int const n = 1024 * 1024;

xtl::spin_lock lock;

void test(int& value)
{
    for(int i = 0; i < n; ++i)
    {
        lock.lock ();

        value++;

        lock.unlock ();
    }
}

int const m = 16; // threads

int main()
{
    printf ("Results of spin_lock_mt_test:\n");

    using namespace std; // printf, clock_t, clock

    printf("%s: %d threads, %d iterations: ", title, m, n);

    int value = 0;

    clock_t t = clock();

    pthread_t a[m];

    for(int i = 0; i < m; ++i)
    {
        a[i] = createThread( xtl::bind(test, xtl::ref (value)) );
    }

    for(int j = 0; j < m; ++j)
    {
        pthread_join(a[j], 0);
    }

    t = clock() - t;

    printf("\n\n%.3f seconds.\n", static_cast<double>(t) / CLOCKS_PER_SEC);

    printf ("value is %u required is %u\n", value, n * m);

    return 0;
}
