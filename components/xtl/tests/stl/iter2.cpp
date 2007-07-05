#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

void print (const vector<const char*>& v)
{
  for (vector<const char*>::const_iterator i=v.begin ();i!=v.end ();i++)
    printf ("%s\n",*i);
}

int main ()
{
  printf ("Results of iter2_test:\n");
  
  vector<const char*> v;
  
  v.push_back ("zippy");
  v.push_back ("motorboy");
  
  print (v);
  
  return 0;
}
