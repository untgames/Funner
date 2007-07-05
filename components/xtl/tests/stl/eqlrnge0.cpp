#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of eqlrnge0_test:\n");
  
  int numbers [10] = {0,0,1,1,2,2,2,2,3,3};

  pair<int*,int*> range = equal_range (numbers,numbers+10,2);
  
  printf ("2 can be inserted from before index %d to before index %d\n",range.first-numbers,range.second-numbers);

  return 0;
}
