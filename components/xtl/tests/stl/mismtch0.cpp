#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of mismtch0_test:\n");
  
  int n1 [5] = {1,2,3,4,5}, n2 [5] = {1,2,3,4,5}, n3 [5] = {1,2,3,2,1};

  pair<int*,int*> result = mismatch (n1,n1+5,n2);
  
  if (result.first == (n1+5) && result.second == (n2+5))
    printf ("n1 and n2 are the same\n");
  else
    printf ("Mismatch at offset: %d\n",result.first-n1);
    
  result = mismatch (n1,n1+5,n3);
  
  if (result.first == (n1+5) && result.second == (n3+5))
    printf ("n1 and n3 are the same\n");
  else
    printf ("Mismatch at offset: %d\n",result.first-n1);
    
  return 0;
}
