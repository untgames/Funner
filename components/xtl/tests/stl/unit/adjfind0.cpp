#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of adjfind0_test:\n");
  
  int numbers1 [5] = {1,2,4,8,16}, numbers2 [5] = {5,3,2,1,1};

  int* location = adjacent_find ((int*)numbers1,(int*)numbers1+5);  

  if (location != numbers1+5)
    printf ("Found adjacent pair of: %d at offset %d\n",*location,location-numbers1);
  else
    printf ("No adjacent pairs\n");
    
  location = adjacent_find ((int*)numbers2,(int*)numbers2+5);
  
  if (location != numbers2+5)
    printf ("Found adjacent pair of: %d at offset %d\n",*location,location - numbers2);
  else
    printf ("No adjacent pairs\n");
    
  return 0;
}
