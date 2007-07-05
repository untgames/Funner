#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

inline void print_sqr (int x)
{
  printf ("%d ",x*x);
}

int main ()
{
  printf ("Results of foreach1_test:\n");

  vector <int> v1(10);
  
  for (int i=0;i<(int)v1.size();v1[i]=i++);

  for_each (v1.begin (),v1.end (),ptr_fun (print_sqr));
  printf ("\n");
  
  return 0;
}
