#include <stl/vector>
#include <stl/algorithm>

using namespace stl;

int main ()
{
  printf ("Results of equal1_test:\n");
  
  vector<int> v1 (10);
  
  for (int i=0;i<(int)v1.size ();i++) v1 [i] = i;
  
  vector<int> v2 (10);
  
  if (equal (v1.begin (),v1.end (),v2.begin ())) printf ("v1 is equal to v2\n");
  else                                           printf ("v1 is not equal to v2\n");
  
  copy (v1.begin (),v1.end (),v2.begin ());
  
  if (equal (v1.begin (),v1.end (),v2.begin ())) printf ("v1 is equal to v2\n");
  else                                           printf ("v1 is not equal to v2\n");
  
  return 0;
}
