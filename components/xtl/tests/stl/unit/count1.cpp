#include <stl/algorithm>
#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of count1_test:\n");
  
  vector<int> numbers (100);
  
  for (int i=0;i<100;i++)
    numbers [i] = i % 3;
    
  int elements = count (numbers.begin (),numbers.end (),2);
  
  printf ("Found %d 2's.\n",elements);
  
  return 0;
}
