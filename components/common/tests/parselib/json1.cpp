#include "shared.h"

using namespace common;

const char* file_name [] = { "data/test1.json", "data/test2.json", "data/test3.json", "data/error_test1.json", "data/error_test2.json" };
const size_t elements = sizeof file_name / sizeof *file_name;

int main ()
{
  printf ("Results of json1_test:\n");

  for (size_t i=0;i<elements;i++)
  {
    Parser parser (file_name [i], "json");
    
    print (parser.Root (), 0);
    
    parser.Log ().Print (&print_log);
  }

  return 0;  
}
