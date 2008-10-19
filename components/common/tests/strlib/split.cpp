#include <common/strlib.h>
#include <stdio.h>

using namespace stl;
using namespace common;

struct Test
{
  const char* str;
  const char* delimiters;
  const char* spaces;
  const char* brackets;
};

int main ()
{
  printf ("Results of split_test:\n");

  static Test test [] = {
      {"hello world"," "," \t",""},
      {"hello, world",","," \t",""},
      {"hello,world",","," \t",""},
      {",world",","," \t",""},
      {"hello,",","," \t",""},
      {",,hello,world,",","," \t",""},
      {"hello world "," "," \t",""},
      {"","","",""},
      {"hello world",""," \t", ""},
      {"hello 'my world' world"," "," \t","''"},
      {"hello (my world}111,world"," ,"," \t","(}"},
      {"hello (my world}world"," }"," \t","(}"},
  };

  static const size_t tests_count = sizeof (test) / sizeof (*test);

  for (size_t i=0;i<tests_count;i++)
  {
    StringArray tokens = split (test [i].str,test [i].delimiters,test [i].spaces, test [i].brackets);

    printf ("split '%s' (delimiters='%s', spaces='%s', brackets='%s') on %lu tokens:\n",
            test [i].str,decompress (test [i].delimiters).c_str (),decompress (test [i].spaces).c_str (),
            decompress (test [i].brackets).c_str (),tokens.Size ());

    for (size_t i=0;i<tokens.Size ();i++)
      printf ("    '%s'\n",tokens [i]);
  }

  return 0;
}
