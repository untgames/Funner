#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of mkheap0_test:\n");
  
  int numbers [6] = {5,10,4,13,11,19};

  make_heap (numbers,numbers+6);
  
  for (int i=6;i;i--)
  {
    printf   ("%d\n",numbers [0]);
    pop_heap (numbers,numbers+i);
  }
  
  return 0;
}
