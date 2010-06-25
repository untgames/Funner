#include "shared.h"

using namespace media::animation;

const char* COLLADA_FILE_NAME = "data/ape.dae";
const char* DST_FILE_NAME     = "/io/stdout/test.xanim";

void print (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  printf ("Results of convert_animation_test:\n");
  
  try
  {
    printf ("Load collada model '%s'\n", COLLADA_FILE_NAME);
    
    media::collada::Model collada_model (COLLADA_FILE_NAME, &print);
    
    printf ("Converting...\n");
    
    AnimationLibrary animation_library;
       
    convert (collada_model, animation_library);

    printf ("Save XAnim library '%s'\n", DST_FILE_NAME);
    
    animation_library.Save (DST_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
