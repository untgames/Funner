#include <common/file.h>
#include <stdio.h>

using namespace common;

const char* MESSAGE = "Hello world!";

int main ()
{
  printf ("Results of anonymous_file_test:\n");
  
  try
  {
    char buffer [128];
    
    MemFile out_file (buffer, sizeof buffer);
    
    out_file.Write (MESSAGE, strlen (MESSAGE));
    
//    printf ("file_name: '%s'\n", out_file.Path ());
    
    stl::string path = out_file.Path ();

    stl::string text = FileSystem::LoadTextFile (path.c_str ());
    
    printf ("message: '%s'\n", text.c_str ());
    printf ("exist:   %s\n", FileSystem::IsFileExist (path.c_str ()) ? "true" : "false");
    
    printf ("rename\n");
    
    FileSystem::Rename (path.c_str (), "/anonymous/my_file");
 
    printf ("exist:   %s\n", FileSystem::IsFileExist ("/anonymous/my_file") ? "true" : "false");
    
    out_file.Close ();
    
    text = FileSystem::LoadTextFile ("/anonymous/my_file");
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;
}
