#include "shared.h"

const char* file_name = "data/test1.wxf";

int main ()
{
  printf ("Results of wxf1_test:\n");
  
  try
  {
    Parser parser (file_name);

    printf ("Parse successfull\n");

    print (parser.Root (), 0);    

    parser.Log ().Print (&print_log);    
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }
  
  return 0;  
}
