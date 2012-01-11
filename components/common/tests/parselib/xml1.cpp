#include "shared.h"

using namespace common;

const char* file_name [] = { "data/test1.xml", "data/error_test1.xml", "data/error_test2.xml",
                             "data/character_reference_test.xml", "data/error_test3.xml",
                             "data/test-utf8.xml", "data/test-cp1251.xml", "data/test-utf16.xml",
                             "data/test-utf16be.xml", "data/test-utf16le.xml", "data/test-utf32.xml",
                             "data/test-utf32be.xml", "data/test-utf32le.xml", "data/test-utf8-bom.xml" };
const size_t elements = sizeof file_name / sizeof *file_name;

int main ()
{
  printf ("Results of xml1_test:\n");

  for (size_t i=0;i<elements;i++)
  {
    Parser parser (file_name [i]);          
    
    print (parser.Root (), 0);
    
    parser.Log ().Print (&print_log);
  }

  return 0;  
}
