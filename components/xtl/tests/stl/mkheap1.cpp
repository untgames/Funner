#include <stl/algorithm>
#include <stl/functional>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of mkheap1_test:\n");
  
  int numbers [6] = {5,10,4,13,11,19};

  make_heap (numbers,numbers+6,greater<int> ());
  
  for (int i=6;i>=1;i--)
  {
    printf   ("%d\n",numbers [0]);
    pop_heap (numbers,numbers+i,greater<int> ());
  }
  
  return 0;
}
