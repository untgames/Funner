#include <stdio.h>
#include <exception>
#include <sound/openal_device.h>

using namespace sound::low_level;

int main ()
{
  printf ("Results of configurations_test:\n");

  try
  {
    register_openal_driver ();
    
    printf ("configurations:\n");
    
    for (size_t i=0; i<SoundSystem::GetConfigurationsCount (); i++)
      printf ("  %s\n", SoundSystem::GetConfiguration (i));    
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
  } 

  return 0;
}
