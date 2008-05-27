#include <stl/algorithm>
#include <stdio.h>
#include <string.h>

using namespace stl;

inline bool string_less (const char* a,const char* b)
{
  return strcmp (a,b) < 0 ? 1 : 0;
}

int main ()
{
  printf ("Results of stblsrt2_test:\n");

  const char* letters [6] = {"bb","aa","ll","dd","qq","cc"};

  stable_sort (letters,letters+6,string_less);
  
  for (int i=0;i<6;i++) printf ("%s ",letters [i]);
  printf ("\n");  
  
  return 0;
}
