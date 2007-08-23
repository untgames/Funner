#include <stdio.h>
#include <mathlib.h>
#include <common/exception.h>
#include <sound/openal_device.h>

using namespace math;
using namespace sound;
using namespace sound::openal_device;

const char* file_name = "data/sound2.ogg";

//печать числа с плавающей точкой
void print (float value)
{
  printf ("%+.3f", value);
}

//печать вектора
template <class T, size_t N>
void print (const math::vec<T, N>& v)
{
  printf ("[");
  
  for (size_t i=0; i<N; i++)
  {
    printf (" ");
    print  (v [i]);
  }
  
  printf (" ]");
}

void dump (Listener& listener)
{
  printf ("position = ");
  print  (listener.position);
  printf (" direction = ");
  print  (listener.direction);
  printf (" up = ");
  print  (listener.up);
  printf (" velocity = ");
  print  (listener.velocity);
}

void dump (Source& source)
{
  printf ("position = ");
  print  (source.position);
  printf (" direction = ");
  print  (source.direction);
  printf (" velocity = ");
  print  (source.velocity);
  printf (" gain = %.3f min_gain = %.3f max_gain = %.3f inner_angle = %.3f outer_angle = %.3f outer_gain = %.3f reference_distance = %.3f",
            source.gain, source.minimum_gain, source.maximum_gain, source.inner_angle, source.outer_angle, source.outer_gain, source.reference_distance);
}

int main ()
{
  try
  {
    ICustomSoundSystem *sound_system = new OpenALSoundSystem;
    SystemInfo info;
    Listener   listener;
    Source     source;

    sound_system->GetInfo (info);
    printf ("Using device %s.\n", sound_system->Name ());
    printf ("Total channels available: %u\n", info.channels_count);
    printf ("Supported EAX %u.%u.\n", info.eax_major_version, info.eax_minor_version);

    sound_system->GetListener (listener);
    dump (listener);
    printf ("\n");
    listener.position  = vec3f(0,0,0);
    listener.direction = vec3f(0,0,-1);
    listener.up        = vec3f(0,1,0);
    listener.velocity  = vec3f(0,0,0);
    sound_system->SetListener (listener);
    sound_system->GetListener (listener);
    dump (listener);
    printf ("\n");
  
    sound_system->GetSource (0, source);
    dump (source);
    printf ("\n");
    source.position     = vec3f(-1,1,1);
    source.direction    = vec3f(0,0,0);
    source.velocity     = vec3f(0,0,0);
    source.gain         = 1.f;
    source.minimum_gain = 0.1f;
    source.maximum_gain = 1.f;
    source.inner_angle  = 360.f;
    source.outer_angle  = 360.f;
    source.outer_gain   = 1.f;
    source.reference_distance = 1.f;
    sound_system->SetSource (0, source);
    sound_system->GetSource (0, source);
    dump (source);
    printf ("\n");

    sound_system->SetSample (0, file_name);
    sound_system->Play (0, true);

    for(;;);

    delete (OpenALSoundSystem*)sound_system;
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
