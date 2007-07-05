#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

bool test (int* numbers,int count,int pivot)
{
  int i=0;

  for (;i<pivot;i++)
    if (numbers [i] >= numbers [pivot])
      return false;      
      
  while (++i<count)
    if (numbers [i] < numbers [pivot])
      return false;      
      
  return true;      
}

int main ()
{
  printf ("Results of nthelem0_test:\n");
  
  int numbers [6] = {5,2,4,1,0,3};  

  nth_element (numbers,numbers+3,numbers+6);
  
  if (!test (numbers,6,3))
  {
    printf ("fail\n");
    for (int i=0;i<6;i++)
      printf ("%d ",numbers [i]);
      
    printf ("\n");    
  }
  else printf ("successfull\n");
    
  return 0;
}
