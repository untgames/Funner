#include <stl/algorithm>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of lexcmp1_test:\n");
  
  const unsigned size = 6;
  char n1 [size] = "shoe", n2 [size] = "shine";

  bool before = lexicographical_compare (n1,n1+size,n2,n2+size);
  
  if (before) printf ("%s is before %s\n",n1,n2);
  else        printf ("%s is before %s\n",n2,n1);

  return 0;
}
