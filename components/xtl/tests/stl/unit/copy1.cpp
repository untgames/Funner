#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of copy1_test:\n");
  
  char string [23] = "A string to be copied.", result[23];
  
  copy (string,string+23,result);

  printf (" Src: %s\nDest: %s\n",string,result);  
  
  return 0;
}
