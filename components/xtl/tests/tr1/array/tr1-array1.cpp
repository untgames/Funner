#include "shared.h"

int main()
{
    printf ("Results of tr1_array1_test:\n");

    // define special type name
    typedef array<float,6> Array;

    // create and initialize an array
    Array a = { { 42 } };

    // access elements
    for (unsigned i=1; i<a.size(); ++i) {
        a[i] = a[i-1]+1;
    }

    // use some common STL container operations
    printf ("size:     %lu\n", a.size());
    printf ("empty:    %s\n", a.empty() ? "true" : "false");
    printf ("max_size: %lu\n", a.max_size());
    printf ("front:    %g\n", a.front());
    printf ("back:     %g\n", a.back());
    printf ("elems:    ");

    // iterate through all elements
    for (Array::const_iterator pos=a.begin(); pos<a.end(); ++pos) {
        printf ("%g ", *pos);
    }

    printf ("\n");

    // check copy constructor and assignment operator
    Array b(a);
    Array c;
    c = a;
    if (a==b && a==c) {
        printf ("copy construction and copy assignment are OK\n");
    }
    else {
        printf ("copy construction and copy assignment FAILED\n");
    }

    return 0;
}
