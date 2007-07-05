#include <common/strlib.h>
#include <stdio.h>

using namespace stl;
using namespace common;

struct Test
{
  const char* str;
  const char* delimiters;
  const char* spaces;
};

int main ()
{
  printf ("Results of split_test:\n");
  
  Test test [] = {
      {"hello world"," "," \t"},
      {"hello, world",","," \t"},
      {"hello,world",","," \t"},
      {",world",","," \t"},
      {"hello,",","," \t"},
      {",,hello,world,",","," \t"},
      {"hello world "," "," \t"},
      {"","",""},
      {"hello world",""," \t"}
  };
  
  const int tests_count = 9;
  
  for (int i=0;i<tests_count;i++)
  {
    vector<string> tokens = split (test [i].str,test [i].delimiters,test [i].spaces);
    
    printf ("split '%s' (delimiters='%s', spaces='%s') on %u tokens:\n",
            test [i].str,decompress (test [i].delimiters).c_str (),decompress (test [i].spaces).c_str (),tokens.size ());
    
    for (size_t i=0;i<tokens.size ();i++)
      printf ("    '%s'\n",tokens [i].c_str ());    
  }
  
  return 0;
}
