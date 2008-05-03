#include <stdio.h>
#include <exception>
#include <xtl/intrusive_ptr.h>
#include <sound/device.h>

using namespace sound::low_level;

int main ()
{
  printf ("Results of configurations_test:\n");

  try
  {
    printf ("configurations:\n");
    
    for (size_t i=0; i<SoundSystem::GetConfigurationsCount (); i++)
    {
      printf ("  %s:\n", SoundSystem::GetConfiguration (i));
      
      xtl::com_ptr<ISoundDevice> sound_system (SoundSystem::CreateDevice (SoundSystem::GetConfiguration (i), 0), false);

      Capabilities   info;

      sound_system->GetCapabilities (info);
      
      printf ("    Total channels available: %u.\n", info.channels_count);
      printf ("    Supported EAX %u.%u.\n", info.eax_major_version, info.eax_minor_version);
      printf ("    Supported EFX %u.%u.\n", info.efx_major_version, info.efx_minor_version);
      printf ("    Maximum auxilary sends %u.\n", info.max_aux_sends);
    }
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
