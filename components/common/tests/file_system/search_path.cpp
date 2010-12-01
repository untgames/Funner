#include "test.h"
#include <xtl/function.h>

const char* SEARCH_PATH = "data";
const char* FILE_NAME   = "test.txt";

void PrintLog (const char* message)
{
  printf ("\t%s\n",message);
}

int main ()
{
  printf ("Results of search_path_test:\n");

  printf ("Add search path '%s':\n",SEARCH_PATH);

  try
  {
    FileSystem::AddSearchPath (SEARCH_PATH, &PrintLog);

    InputFile file (FILE_NAME);

    printf ("File name: '%s'\n",FILE_NAME);
    printf ("File size: %lu bytes\n",file.Size ());
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
