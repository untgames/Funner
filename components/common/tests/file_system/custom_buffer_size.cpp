#include "test.h"

int main ()
{
  printf ("Results of custom_buffer_size_test:\n");

  try
  {
    FileSystem::AddSearchPath ("data");
    FileSystem::SetFileBufferSize ("speed-test.txt", 1024*1024);
    
    InputFile file ("speed-test.txt");

    printf ("File buffer size: %u\n", file.BufferSize ());
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
