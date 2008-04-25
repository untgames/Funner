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

const size_t MAX_WORDS = 4;

int main ()
{
  printf ("Results of str_word_test:\n");
  
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
    const Test& this_test = test [i];
    
    printf ("split '%s' (delimiters='%s', spaces='%s'): {",
            this_test.str, decompress (this_test.delimiters).c_str (), decompress (this_test.spaces).c_str ());

    for (int j=0; j<MAX_WORDS; j++)
      printf ("'%s' ", word (this_test.str, j, this_test.delimiters, this_test.spaces).c_str ());

    printf ("}\n");
  }
 
  return 0;
}
