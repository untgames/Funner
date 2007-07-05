#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of find1_test:\n");
  
  int years [] = {1942,1952,1962,1972,1982,1992};

  const unsigned yearCount = sizeof (years) / sizeof (years[0]);
   
  printf ("Found 1972 at offset %d\n",find (years,years+yearCount,1972)-years);
  
  return 0;
}
