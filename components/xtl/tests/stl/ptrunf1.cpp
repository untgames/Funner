#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

inline bool even (int n)
{
  return (n % 2) == 0;
}

int main ()
{
  printf ("Results of ptrunf1_test:\n");

  int array [3] = {1,2,3};

  int* p = find_if ((int*)array,(int*)array+3,pointer_to_unary_function<int,bool> (even));
  
  if (p != array + 3)
    printf ("%d is even\n",*p);
    
  return 0;
}
