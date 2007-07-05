#include <stl/list>
#include <stl/stack>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of stack2_test:\n");
  
  stack<int,list<int> > s;
  
  s.push (42);
  s.push (101);
  s.push (69);
  
  while(!s.empty ())
  {
    printf ("%d\n",s.top ());
    s.pop ();
  }
  
  return 0;
}
