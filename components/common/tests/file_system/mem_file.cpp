#include <stl/string>
#include <common/file.h>
#include <stdio.h>

using namespace common;

const char* MESSAGE = "Hello world!";

int main ()
{
  printf ("Results of mem_file_test:\n");
  
  try
  {
    MemFile out_file;

    out_file.Write (MESSAGE, strlen (MESSAGE));

    stl::string path = out_file.Path ();

    stl::string text = FileSystem::LoadTextFile (path.c_str ());

    printf ("message: '%s'\n", text.c_str ());
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;
}
