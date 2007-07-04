#include <common/file.h>
#include <stdio.h>

using namespace common;

const char* MESSAGE = "Hello world!";

int main ()
{
  printf ("Results of console-output_test:\n");
  
  try
  {
    OutputFile file ("con");
    
    file.Write (MESSAGE,strlen (MESSAGE));
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;
}
