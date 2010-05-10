#include "shared.h"

const char* file_name = "data/test5.wxf";

int main ()
{
  printf ("Results of parse_resolve3_test:\n");
  
  try
  {
    Parser parser (file_name);
    PropertyMap properties;
    
    properties.SetProperty ("A.D", "Hello");

    printf ("Parse successfull\n");
    
    ParseNode root = resolve_references (parser.Root (), properties);

    print (root, 0);

    parser.Log ().Print (&print_log);    
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
