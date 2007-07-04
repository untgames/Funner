#include "test.h"

const char* FILE_NAME = "data/test.txt";

int main ()
{
  printf ("Results of read_file_test:\n");

  try
  {
    InputFile file (FILE_NAME);

    char buf [128];

    printf ("File name: '%s'\n",FILE_NAME);
    printf ("File content:\n");
    
    while (!file.Eof ())
    {
      size_t read_size = file.Read (buf,sizeof (buf)-1);
      
      buf [read_size] = 0;
      
      printf ("%s",buf);
    }
    
    printf ("\nEnd file content\n");
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
