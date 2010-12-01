#include "shared.h"

const char* file_name = "data/explode.wxf";

int main ()
{
  printf ("Results of parse_big_attributes_array_test:\n");

  try
  {
    Parser parser (file_name);

    printf ("Parse successfull\n");

    parser.Log ().Print (&print_log);    
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }
  
  return 0;  
}
