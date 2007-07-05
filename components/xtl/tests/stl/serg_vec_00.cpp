#include <stdio.h>
#include <stl/vector>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of serg_vec_00:\n");

  vector <int> v0;                                     // Create an empty vector v0
  vector <int> v1 (3);                                 // Create a vector v1 with 3 elements of default value 0
  vector <int> v2 (5, 2);                              // Create a vector v2 with 5 elements of value 2
  vector <int> v3 (3, 1, v2.get_allocator () );        // Create a vector v3 with 3 elements of value 1 and with the allocator of vector v2
  vector <int> v4 (v2);                                // Create a copy, vector v4, of vector v2
  vector <int> v5 (v4.begin () + 1, v4.begin () + 3);  // Create a vector v5 by copying the range v4[_First, _Last)

  print ("v0 =", v0.begin (), v0.end ());
  print ("v1 =", v1.begin (), v1.end ());
  print ("v2 =", v2.begin (), v2.end ());
  print ("v3 =", v3.begin (), v3.end ());
  print ("v4 =", v4.begin (), v4.end ());
  print ("v5 =", v5.begin (), v5.end ());
  
  return 0;
}
