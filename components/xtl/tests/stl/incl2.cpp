#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>
#include <string.h>

using namespace stl;

inline bool compare_strings (const char* s1,const char* s2)
{
  return strcmp (s1,s2) < 0 ? 1 : 0;
}

int main ()
{
  printf ("Results of incl2_test:\n");

  const char* names [] = {"Todd","Mike","Graham","Jack","Brett"};

  const unsigned nameSize = sizeof (names) / sizeof (names[0]);
  
  vector<const char*> v1 (nameSize), v2 (2);
  
  for (int i=0;i<(int)v1.size();i++) v1 [i] = names [i]; 

  v2 [0] = "foo";
  v2 [1] = "bar";
  
  sort (v1.begin (),v1.end (),compare_strings);
  sort (v2.begin (),v2.end (),compare_strings);

  if (includes (v1.begin (),v1.end (),v2.begin (),v2.end (),compare_strings))
    printf ("v1 includes v2\n");
  else
    printf ("v1 does not include v2\n");
    
  v2 [0] = "Brett";
  v2 [1] = "Todd";
  
  if (includes (v1.begin (),v1.end (),v2.begin (),v2.end (),compare_strings))
    printf ("v1 includes v2\n");
  else
    printf ("v1 does not include v2\n");
    
  return 0;
}
