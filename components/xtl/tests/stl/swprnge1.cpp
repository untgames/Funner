#include <stl/algorithm>
#include <string.h>
#include <stdio.h>

using namespace stl;

int main ()
{
  printf ("Results of swprnge1_test:\n");
  
  char word1 [] = "World", word2 [] = "Hello";
  
  printf      ("%s %s\n",word1,word2);  
  swap_ranges (word1,word1+strlen (word1),word2);
  printf      ("%s %s\n",word1,word2);  

  return 0;
}
