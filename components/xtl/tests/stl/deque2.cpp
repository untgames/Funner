#include <stl/deque>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of deque2_test:\n");
  
  deque<int> d;
  
  d.push_back(4); // Add after end.
  d.push_front(9);
  d.push_front(16);
  
  printf ("end () - begin () = %d\n",d.end ()-d.begin ());
  printf ("begin () - end () = %d\n",d.begin ()-d.end ());  
  
  return 0;
}
