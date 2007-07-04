#include <common/file.h>
#include <stdio.h>
#include <memory.h>

using namespace common;

const size_t READ_BLOCK_SIZE   = 4;
const char*  MESSAGE1          = "0123456789\n";
const char*  MESSAGE2          = "9876543210\n";
const char*  MESSAGE3          = "---ABCDEFGHIJKLMNOPQRSTUVWXYZ---\n";
const char*  RESULTS_FILE_NAME = "media/tests/filesys/buffered_file_results.txt";

static char buffer [1024*16];

int main ()
{
  printf ("Results of buffered_file_test:\n");
  
  try
  {
    StdFile file (RESULTS_FILE_NAME,FILE_MODE_READ_WRITE|FILE_MODE_CREATE,strlen (MESSAGE1) * 2 + 1);

    printf ("Is file buffered: %s\n",file.IsBuffered () ? "true" : "false");

    int message_len = strlen (MESSAGE1);

    for (size_t i=0;i<10;i++)
      file.Write (MESSAGE1,message_len);

    message_len = strlen (MESSAGE2);

    file.Seek (message_len*9,FILE_SEEK_CUR);

    for (size_t i=0;i<10;i++)
    {
      file.Write (MESSAGE2,message_len);
      file.Seek  (-message_len*2,FILE_SEEK_CUR);
    }

    file.Seek  (-message_len,FILE_SEEK_CUR);
    file.Write (MESSAGE3,strlen (MESSAGE3));
    file.Write (MESSAGE3,strlen (MESSAGE3));
    
    printf ("File content (forward read):\n");

    file.Rewind ();

    while (!file.Eof ())
    {
      char s [READ_BLOCK_SIZE+1] = {0};
      
      file.Read (s,sizeof (s)-1);
      
      printf ("%s",s);
    }
    
    printf ("End file content\n");
    
    printf ("File content (backward read):\n");
    
    if (sizeof (buffer) - 1 < file.Size ())
    {
      printf ("Read buffer too small\n");
      return 0;
    }

    file.Seek (0,FILE_SEEK_END);

    while (file.Tell ())
    {
      file.Read (buffer+file.Seek (-(int)READ_BLOCK_SIZE,FILE_SEEK_CUR),READ_BLOCK_SIZE);
      file.Seek (-(int)READ_BLOCK_SIZE,FILE_SEEK_CUR);
    }

    buffer [file.Size ()] = 0;

    printf ("%s",buffer);

    printf ("End file content\n");

    printf ("File content (direct read test):\n");

    memset (buffer,0,sizeof (buffer));

    file.Read (buffer,file.Size ());

    printf ("%s",buffer);

    printf ("End file content\n");

    file.Close ();

    FileSystem::Remove (RESULTS_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
