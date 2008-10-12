#include "shared.h"

template <typename T>
void test_static_size (const T& cont)
{
    int tmp[T::static_size];
    for (unsigned i=0; i<T::static_size; ++i) {
        tmp[i] = int(cont[i]);
    }
    for (unsigned j=0; j<T::static_size; ++j) {
        printf ("%d ", tmp[j]);
    }
    printf ("\n");
}

int main ()
{
    printf ("Results of tr1_array5_test:\n");

    // define special type name
    typedef array<float,6> Array;

    // create and initialize an array
    const Array a = { { 42.42f } };

    // use some common STL container operations
    printf ("static_size: %lu\n", a.size ());
    printf ("size:        %lu\n", a.size ());
    // Can't use std::boolalpha because it isn't portable
    printf ("empty:       %s\n", (a.empty()? "true" : "false"));
    printf ("max_size:    %lu\n", a.max_size ());
    printf ("front:       %f\n", a.front ());
    printf ("back:        %f\n", a.back ());
    printf ("[0]:         %f\n", a[0]);
    printf ("elems:       ");

    // iterate through all elements
    for (Array::const_iterator pos=a.begin(); pos<a.end(); ++pos)
        printf ("%f ", *pos);

    printf ("\n");

    test_static_size(a);

    // check copy constructor and assignment operator
    Array b(a);
    Array c;
    c = a;
    if (a==b && a==c)
      printf ("copy construction and copy assignment are OK\n");
    else
      printf ("copy construction and copy assignment are BROKEN\n");

    typedef array<double,6> DArray;
    typedef array<int,6> IArray;
    IArray ia = { { 1, 2, 3, 4, 5, 6 } } ; // extra braces silence GCC warning
    DArray da;
    da = ia;
    da.assign (42);

    return 0;
}

