#include <common/file.h>
#include <stdio.h>
#include <time.h>

using namespace common;

const char*  SEARCH_PATH = "data";
const char*  FILE_NAME   = "speed-test.txt";
const size_t BUF_SIZE    = 128;
const size_t TESTS_COUNT = 10;

void PrintLog (const char* message)
{
  printf ("\t%s\n",message);
}

static char buf [BUF_SIZE];

int main ()
{
  printf ("Results of zip-read-speed_test:\n");
  
  printf ("Add search path '%s':\n",SEARCH_PATH);
  
  FileSystem::AddSearchPath (SEARCH_PATH);

  try
  {
    InputFile file (FILE_NAME);

    printf ("File size:   %u\n",file.Size ());
    printf ("Tests count: %u\n",TESTS_COUNT);

    clock_t start = clock ();        

    for (size_t i=0;i<TESTS_COUNT;i++)
    {
      while (!file.Eof ()) file.Read (buf,sizeof (buf));
      
      file.Rewind ();
    }

    clock_t end = clock ();
    
    printf ("Time:        %.2f seconds\n",float (end-start)/float (CLOCKS_PER_SEC));
    printf ("Speed:       %.2f M/sec\n",float (file.Size ())/float (end-start)*float (CLOCKS_PER_SEC)/1.0e6f*float (TESTS_COUNT));
  }
  catch (FileException& exception)
  {
    printf ("FileException: %s\n",exception.Message ());
  }

  return 0;
}
