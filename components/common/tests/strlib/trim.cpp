#include <common/strlib.h>
#include <stdio.h>

using namespace stl;
using namespace common;

struct Test
{
  const char* str;
  const char* spaces;
};

const size_t MAX_WORDS = 4;

int main ()
{
  printf ("Results of trim_test:\n");
  
  static Test test [] = {
      {" hello world"," \t"},
      {"hello world "," \t"},
      {" hello world "," \t"},
      {"\thello world"," \t"},
      {"hello world\t"," \t"},
      {"\t hello world \t"," \t"},
      {"",""},
      {" ",""},
      {"","\t "},      
  };
  
  const size_t tests_count = sizeof (test)/sizeof (*test);
  
  for (size_t i=0;i<tests_count;i++)
  {
    const Test& this_test = test [i];
    
    printf ("strip('%s','%s')='%s' trim('%s','%s')='%s'\n", this_test.str, this_test.spaces, strip (this_test.str, this_test.spaces).c_str (),
      this_test.str, this_test.spaces, trim (this_test.str, this_test.spaces).c_str ());
  }
 
  return 0;
}
