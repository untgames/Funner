#include "test.h"

const char* SEARCH_PATH = "data";
const char* FILE_NAME   = "search/test_zip.txt";

void PrintLog (const char* message)
{
  printf ("\t%s\n",message);
}

void ReadFileContent (File& file)
{
  char buf [128];
  
  printf ("File content: '");
  
  while (!file.Eof ())
  {
    size_t read_size = file.Read (buf,sizeof (buf)-1);
    
    buf [read_size] = 0;
    
    printf ("%s",buf);
  }
  
  printf ("'\n");
}

int main ()
{
  printf ("Results of zip_read_file_test:\n");
  
  printf ("Add search path '%s':\n",SEARCH_PATH);

  try
  {
    FileSystem::AddSearchPath (SEARCH_PATH);
    
    InputFile file (FILE_NAME);
       
    printf ("File name: '%s'\n",FILE_NAME);
    printf ("File size: %u\n",file.Size ());

    printf ("Test #1. Read file content\n");
    
    ReadFileContent (file);
    
    printf ("Test #2. Rewind and read file content\n");
    
    file.Rewind ();
    
    ReadFileContent (file);
    
    printf ("Test #3. Seek and read file content\n");
    
    file.Seek (8,FILE_SEEK_END);
    
    ReadFileContent (file);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
