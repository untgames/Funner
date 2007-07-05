#include <stl/queue>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of pqueue1_test:\n");
  
  priority_queue<int> q;
  
  q.push (42);
  q.push (101);
  q.push (69);
  
  while(!q.empty ())
  {
    printf ("%d\n",q.top ());
    q.pop ();
  }
  
  return 0;
}
