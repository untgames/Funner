#include <stl/vector>
#include <stl/algorithm>

using namespace stl;

int main ()
{
  printf ("Results of iter3_test:\n");
  
  typedef vector<const char*> Vec;
  
  Vec v;
  
  v.push_back ("zippy");
  v.push_back ("motorboy");
  
  for (Vec::reverse_iterator it=v.rbegin ();it!=v.rend ();it++)
    printf ("%s\n",*it);

  return 0;
}
