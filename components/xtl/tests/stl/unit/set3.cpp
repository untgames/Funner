#include <stl/set>
#include <stdio.h>

using namespace stl;

void init_set3 (set<int>& s)
{
  set<int> tmp;
  
  for (int i=0;i<10;i++) tmp.insert (i);   
  
  swap (tmp,s);
}

int main ()
{
  printf ("Results of set3_test:\n");
  
  set<int> s;
  
  s.insert (123);
  
  init_set3 (s);

  for (set<int>::iterator i=s.begin ();i!=s.end ();i++)
    printf ("%d ",*i);
  printf ("\n");
  
  return 0;
}
