#include <stl/pair.h>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of pair0_test:\n");
  
  pair<int,int> p = make_pair (1,10);
  
  printf ("p.first = %d, p.second = %d\n",p.first,p.second);
  
  return 0;
}
