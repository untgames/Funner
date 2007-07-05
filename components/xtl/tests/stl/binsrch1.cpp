#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of binsrch1_test:\n");
  
  int vector [100];  
  
  for (int i=0;i<100;i++) vector [i] = i;
  
  if (binary_search (vector,vector+100,42))
    printf ("found 42\n");
  else
    printf ("did not find 42\n");

  return 0;
}
