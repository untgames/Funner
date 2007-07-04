#include "test.h"

const char* RESULTS_FILE_NAME = "media/tests/filesys/output_text_stream_results.txt";

int main ()
{
  printf ("Results of output_text_stream_test:\n");

  try
  {
    OutputTextStream os (RESULTS_FILE_NAME);

    os.SetBufferSize (16); //ignore

    printf ("Buffer size: %u\n",os.GetBufferSize ());
    
    os.Print ("This file generated for test OutputTextStream\n");

    for (size_t i=0;i<1000;i++)
      os.Printf ("Hello world #%u\n",i);

    os.UnbindFile ();
    
    PrintFileInfo (RESULTS_FILE_NAME);

    FileSystem::Remove (RESULTS_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
