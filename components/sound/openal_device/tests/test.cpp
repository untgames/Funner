#include <stdio.h>
#include <mathlib.h>

#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/bind.h>
#include <xtl/connection.h>

#include <common/exception.h>

#include <syslib/application.h>
#include <syslib/timer.h>

#include <sound/device.h>

#include <media/sound.h>

using namespace math;
using namespace syslib;
using namespace sound::low_level;
using namespace sound;
using namespace xtl;
using namespace media;

const char* file_name = "data/sound1.ogg";
//const char* file_name2 = "data/sound1.wav";
const char* file_name2 = "data/sound2.ogg";

const size_t SOURCE_UPDATE_TIME = 100;   //период обновления параметров источника звука (в милисекундах)
const size_t TEST_WORK_TIME     = 80000;  //время работы теста (в милисекундах)

float        source_angle = 0;
Source       source;

void log_print (const char* message)
{
  printf ("DeviceLog: %s\n", message);
}

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
  printf ("listener:\n");
  printf ("  position = ");
  print  (listener.position);
  printf ("\n  direction = ");
  print  (listener.direction);
  printf ("\n  up = ");
  print  (listener.up);
  printf ("\n  velocity = ");
  print  (listener.velocity);
  printf ("\n");
}

void dump (Source& source)
{
  printf ("source:\n");
  printf ("  position = ");
  print  (source.position);
  printf ("\n  direction = ");
  print  (source.direction);
  printf ("\n  velocity = ");
  print  (source.velocity);
  printf ("\n  gain = %.3f\n  min_gain = %.3f\n  max_gain = %.3f\n  inner_angle = %.3f\n"
          "  outer_angle = %.3f\n  outer_gain = %.3f\n  reference_distance = %.3f",
          source.gain, source.minimum_gain, source.maximum_gain, source.inner_angle,
          source.outer_angle, source.outer_gain, source.reference_distance);
  printf ("\n");
}

void TimerHandler (ISoundDevice* sound_system, Timer&)
{
  source.position = vec3f (sin (deg2rad (source_angle)), 0, cos (deg2rad (source_angle)));
  source_angle++;
  sound_system->SetSource (1, source);
}

void idle ()
{
  Application::Sleep (100);
}

int main ()
{
  try
  {
    xtl::com_ptr<ISoundDevice> sound_system (SoundSystem::CreateDevice (SoundSystem::FindConfiguration ("OpenAL", "Generic*"), 0, "frequency=192000 min_channels_count=32 max_channels_count=192"), false);

    Capabilities   info;
    Listener       listener;

    sound_system->SetDebugLog (&log_print);
    sound_system->GetCapabilities (info);
    
//    sound_system->SetIntegerParam ("al_debug_log", 1);

    printf ("Using device %s.\n", sound_system->Name ());
    printf ("Total channels available: %u.\n", info.channels_count);
    printf ("Supported EAX %u.%u.\n", info.eax_major_version, info.eax_minor_version);
    printf ("Supported EFX %u.%u.\n", info.efx_major_version, info.efx_minor_version);
    printf ("Maximum auxilary sends %u.\n", info.max_aux_sends);

    printf ("Distance model is '%s'\n", sound_system->GetStringParam ("al_distance_model"));

    sound_system->GetListener (listener);
    dump (listener);
    listener.position  = vec3f(0,0,0);
    listener.direction = vec3f(0,0,-1);
    listener.up        = vec3f(0,1,0);
    listener.velocity  = vec3f(0,0,0);
    sound_system->SetListener (listener);
    sound_system->GetListener (listener);
    dump (listener);
  
    sound_system->GetSource (0, source);
    dump (source);
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
    source.maximum_distance   = 2.f;
    sound_system->SetSource (0, source);
    sound_system->GetSource (0, source);
    dump (source);

//    source.gain         = 0.3f;
    sound_system->SetSource (1, source);
    
    SoundSample sample1 (file_name), sample2 (file_name2);
    
    sound_system->SetSample (1, sample2);

//    sound_system->SetSample (0, sample1);
    sound_system->Seek (1, 40.f, SeekMode_Clamp);
//    sound_system->Play (0, true);
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
