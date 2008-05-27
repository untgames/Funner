//Тестирование алгоритма find_first_of (SGI sample)
#include <stdio.h>
#include <stl/algorithm>
#include "test.h"

using namespace stl;

int main ()
{
  printf ("Results of serg_alg_find03_3:\n");

  const char* WS = "\t\n ";
  const int n_WS = strlen (WS);

  const char* s1 = "This sentence contains five words.";
  const char* s2 = "OneWord";


  const char* end1 = find_first_of (s1, s1 + strlen(s1), WS, WS + n_WS); 
  const char* end2 = find_first_of (s2, s2 + strlen(s2), WS, WS + n_WS); 

  printf ("First word of s1: %.*s\n", end1 - s1, s1); 
  printf ("First word of s2: %.*s\n", end2 - s2, s2); 

  return 0;
}
 
