#include <stl/algorithm>
#include <string.h>
#include <stdio.h>

using namespace stl;

bool char_str_less (const char* a,const char* b)
{
  return strcmp (a,b) < 0 ? 1 : 0;
}

int main ()
{
  printf ("Results of lwrbnd2_test:\n");

  const char* str [] = {"a","a","b","b","q","w","z"};

  const unsigned strCt = sizeof (str)/sizeof (str[0]);

  printf ("d can be inserted at index: %d\n",lower_bound (str,str+strCt,"d",char_str_less)-str);

  return 0;
}
