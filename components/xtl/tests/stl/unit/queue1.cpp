#include <stl/queue>
#include <stl/list>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of queue1_test:\n");
  
  queue<int,list<int> > q;
  
  q.push (42);
  q.push (101);
  q.push (69);
  
  while(!q.empty())
  {
    printf ("%d\n",q.front ());
    q.pop ();
  }
  
  return 0;
}
