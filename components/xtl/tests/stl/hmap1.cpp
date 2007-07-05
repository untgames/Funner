#include <stl/hash_map>
#include <stl/string>
#include <stdio.h>

using namespace stl;

typedef hash_map<int,string> Dict;

const int DICT_SIZE = 1000;

int main ()
{
  printf ("Results of hmap1_test:\n");

  Dict dict;
  
  int i; 

  for (i=0;i<DICT_SIZE;i++)
  {
    char s [128];
    
    sprintf (s,"Test%d",i);
    
    dict [i] = s;
  }
  
  bool errors = false;    
  
  for (i=0;i<DICT_SIZE;i++)
  {
    char s [128];
    
    sprintf (s,"Test%d",i);    
    
    pair<Dict::iterator,Dict::iterator> p = dict.equal_range (i);
    
    if (p.first == p.second)
      printf ("element with key %d not found\n",i);
    else if (p.first->second != (const char*)s)
      printf ("wrong value dict [%d]='%s'\n",i,p.first->second.c_str ());
    else
      continue;

    errors = true;
  }  
  
  if (!errors)
    printf ("successfull\n");

  return 0;
}
