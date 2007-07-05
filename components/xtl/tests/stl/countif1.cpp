#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of countif1_test:\n");

  vector<int> numbers (100);
  
  for(int i=0;i<100;i++) numbers [i] = i % 3;
  
  int elements = count_if (numbers.begin (),numbers.end (),odd);
  
  printf ("Found %d odd elements.\n",elements);
  
  return 0;
}
