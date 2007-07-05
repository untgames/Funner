#include <stl/algorithm>
#include <stl/vector>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of lwrbnd1_test:\n");
  
  vector<int> v1 (20);
  
  for (int i=0;i<(int)v1.size ();i++)
  {
    v1 [i] = i/4;
    printf ("%d ",v1 [i]);
  }
  
  vector<int>::iterator location = lower_bound (v1.begin (),v1.end (),3);
  
  printf ("\n3 can be inserted at index: %d\n",location-v1.begin ());

  return 0;
}
