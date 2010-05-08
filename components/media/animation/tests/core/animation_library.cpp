#include "shared.h"

using namespace media;
using namespace media::animation;

void dump (const AnimationLibrary& library)
{
  printf ("library (%u meshes)\n", library.Size ());

  for (AnimationLibrary::ConstIterator i=library.CreateIterator (); i; ++i)
    printf ("  animation '%s'\n", i->Name ());
}

int main ()
{
  printf ("Results of animation_library_test:\n");
  
  try
  {
    AnimationLibrary library;
    
    const char* animation_name [] = {"animation1", "animation2", "animation3", "animation4"};
    
    printf ("attach animations\n");
    
    for (size_t i=0; i<4; i++)
    {
      Animation animation;

      animation.Rename (animation_name [i]);

      library.Attach (animation_name [i], animation);
    }  
    
    dump (library);
    
    printf ("detach animation\n");
    
    library.Detach ("animation3");
    
    dump (library);
    
    printf ("clear library\n");
    
    library.DetachAll ();
    
    dump (library);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
