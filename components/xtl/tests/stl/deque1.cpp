#include <stl/deque>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of deque1_test:\n");
  
  deque<int> d;
  
  d.push_back(4); // Add after end.
  d.push_back(9);
  d.push_back(16);
  d.push_front(1); // Insert at beginning.   
  
  for (int i=0;i<(int)d.size();i++) printf ("d[%d] = %d\n",i,d [i]);
  printf ("\n");

  d.pop_front(); // Erase first element.

  d [2] = 25; // Replace last element.
  
  for (int i=0;i<(int)d.size();i++) printf ("d[%d] = %d\n",i,d [i]);
  
  return 0;
}
