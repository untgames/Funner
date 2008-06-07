#include <stdio.h>
#include <xtl/token_parser.h>

using namespace xtl::io;

void checked_get (const char* string)
{
  printf ("try get int from '%s': ", string);

  try
  {
    int x = get<int> (string);

    printf ("%d\n", x);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}

void default_get (const char* string, int default_value)
{
  int value = get (string, default_value);
  
  printf ("get int from '%s': %d\n", string, value);
}

int main ()
{
  printf ("Results of token_parser_test:\n");
  
  checked_get ("123");
  checked_get ("");
  checked_get ("h123");
  checked_get ("123h");
  default_get ("123", -1);
  default_get ("", -1);
  default_get ("h123", -1);
  default_get ("123h", -1);

  return 0;
}
