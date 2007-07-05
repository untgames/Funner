#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of uprbnd1_test:\n");
  
  int array [20];
  
  for (int i=0;i<20;i++)
  {
    array [i] = i/4;
    printf ("%d ",array [i]);
  }
  
  printf ("\n3 can be inserted at index: %d\n",upper_bound (array,array+20,3)-array);

  return 0;
}
