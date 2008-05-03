#include "test.h"

const char* MESSAGE = "Hello world!\n";

int main ()
{
  printf ("Results of stdout_test:\n");  
  fflush (stdout);

  try
  {
    OutputFile ("/io/stdout").Write (MESSAGE,strlen (MESSAGE));
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
    fflush (stdout);
  }    

  return 0;
}
