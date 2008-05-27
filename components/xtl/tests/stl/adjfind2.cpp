#include <stl/string>
#include <stl/vector>
#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

inline int equal_length(const char* v1,const char* v2)
{
  return ::strlen (v1) == ::strlen(v2);
}

int main ()
{
  printf ("Results of adjfind2_test:\n");
  
  typedef vector<const char*> CStrVector;

  const char* names [] = {"Brett","Graham","Jack","Mike","Todd"};
  const int nameCount = sizeof(names)/sizeof(names[0]);
  
  CStrVector v (nameCount);
  
  for (int i=0;i<nameCount;i++) v [i] = names [i];
  
  CStrVector::iterator location = adjacent_find (v.begin (),v.end (),equal_length);
  
  if (location != v.end ())
    printf ("Found two adjacent strings of equal length: %s -and- %s\n",location [0],location [1]);
  else
    printf ("Didn't find two adjacent strings of equal length.\n");
    
  return 0;
}
