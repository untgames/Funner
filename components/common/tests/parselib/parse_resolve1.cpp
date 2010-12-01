#include "shared.h"

const char* file_name = "data/test4.wxf";

void resolver (const char* name, stl::string& replacement)
{
  replacement = common::format ("replacement.%s", name);
}

int main ()
{
  printf ("Results of parse_resolve1_test:\n");
  
  try
  {
    Parser parser (file_name);

    printf ("Parse successfull\n");
    
    ParseNode root = resolve_references (parser.Root (), resolver);

    print (root, 0);

    parser.Log ().Print (&print_log);    
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
