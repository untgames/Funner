#include <stdio.h>
#include <exception>
#include <media/sound_declaration.h>

using namespace media;

const char* SOURCE_FILE_NAME = "data/test.snddecl";
const char* RESULT_FILE_NAME = "/io/stdout/test.snddecl";

int main ()
{
  printf ("Results of snddecl_loader_test:\n");
  
  try
  {
    SoundDeclarationLibrary library;
    
    library.Load (SOURCE_FILE_NAME);

    library.Save (RESULT_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
