#include "shared.h"

template <class T>
void print_elements (const T& coll, const char* optcstr="");

int main ()
{
    printf ("Results of tr1_array2_test:\n");

  array <int,10> a = { { 1, 2, 3, 4, 5 } };

  print_elements (a);

  // modify elements directly
  for (unsigned i=0; i<a.size(); ++i) {
      ++a[i];
  }
  print_elements(a);

  // change order using an STL algorithm
  stl::reverse (a.begin (), a.end ());
  print_elements (a);

  // negate elements using STL framework
  stl::transform (a.begin(),a.end(),    // source
                  a.begin(),            // destination
                  stl::negate<int>());  // operation
  print_elements(a);

  return 0;
}

template <class T>
void print_elements (const T& coll, const char* optcstr)
{
  typename T::const_iterator pos;

  printf ("%s", optcstr);

  for (pos=coll.begin(); pos!=coll.end(); ++pos)
     printf ("%d ", *pos);

  printf ("\n");
}
