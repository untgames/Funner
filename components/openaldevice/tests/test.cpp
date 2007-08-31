#include <stdio.h>
#include <mathlib.h>
#include <common/exception.h>
#include <xtl/signal.h>
#include <syslib/application.h>
#include <syslib/timer.h>
#include <sound/openal_device.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/bind.h>
#include <xtl/connection.h>

using namespace math;
using namespace syslib;
using namespace sound::low_level;
using namespace xtl;

const char* file_name = "data/sound1.ogg";
const char* file_name2 = "data/sound2.ogg";

const size_t SOURCE_UPDATE_TIME = 100;   //период обновления параметров источника звука (в милисекундах)
const size_t TEST_WORK_TIME     = 4000;  //время работы теста (в милисекундах)

float        source_angle = 0;
Source       source;

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

void TimerHandler (ISoundDevice* sound_system, Timer&)
{
  source.position = vec3f (sin (deg2rad (source_angle)), 0, cos (deg2rad (source_angle)));
  source_angle++;
  sound_system->SetSource (1, source);
}

void idle ()
{
//  Application::Sleep (100);
}

int main ()
{
  try
  {
    register_openal_driver ();
    
    xtl::com_ptr<ISoundDevice> sound_system (SoundSystem::CreateDevice (SoundSystem::FindConfiguration ("openal", "*"), 0), false);

    Capabilities   info;
    Listener       listener;

    sound_system->GetCapabilities (info);

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
    source.position     = vec3f(0,0,1);
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

    source.gain         = 0.3f;
    sound_system->SetSource (1, source);
    sound_system->SetSample (1, file_name2);

    sound_system->SetSample (0, file_name);
    sound_system->Seek (0, 1.f);
    sound_system->Play (0, true);
    sound_system->Play (1, true);

    Timer timer1 (bind (&TimerHandler, get_pointer (sound_system), _1), SOURCE_UPDATE_TIME),
          timer2 (bind (&Application::Exit, 0), TEST_WORK_TIME);
          
//    xtl::auto_connection c = Application::RegisterEventHandler (ApplicationEvent_OnIdle, &idle);

    Application::Run ();
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
  }
  
  printf ("exit\n");

  return Application::GetExitCode ();
}
