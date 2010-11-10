#include <stdio.h>

#include <math/angle.h>

#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/common_exceptions.h>

#include <common/action_queue.h>
#include <common/log.h>

#include <syslib/application.h>

#include <sound/device.h>
#include <sound/driver.h>

#include <media/sound.h>

using namespace math;
using namespace common;
using namespace syslib;
using namespace sound::low_level;
using namespace sound;
using namespace xtl;
using namespace media;

const char* file_name = "data/sound1.ogg";
//const char* file_name2 = "data/sound1.wav";
const char* file_name2 = "data/sound2.ogg";

const char* LOG_FILTER_MASK = "sound::low_level::openal";

const size_t SOURCE_UPDATE_TIME = 100;   //период обновления параметров источника звука (в милисекундах)
const size_t TEST_WORK_TIME     = 8000;  //время работы теста (в милисекундах)

float        source_angle = 0;
Source       source;

void log_print (const char* log_name, const char* message)
{
  printf ("Log '%s': '%s'\n", log_name, message);
}

//печать числа с плавающей точкой
void print (float value)
{
  printf ("%+.3f", value);
}

//печать вектора
template <class T, unsigned int N>
void print (const math::vector<T, N>& v)
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

void TimerHandler (IDevice* sound_system)
{
  source.position = vec3f (sin (degree (source_angle)), 0, cos (degree (source_angle)));
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
    common::LogFilter log_filter (LOG_FILTER_MASK, &log_print);

    xtl::com_ptr<IDevice> sound_system (DriverManager::CreateDevice ("OpenAL", "*", "frequency=192000 min_channels_count=32 max_channels_count=192"), false);

    Capabilities   info;
    Listener       listener;

    sound_system->GetCapabilities (info);

//    sound_system->SetIntegerParam ("al_debug_log", 1);

    printf ("Using device %s.\n", sound_system->Name ());
    printf ("Total channels available: %u.\n", info.channels_count);
//    printf ("Supported EAX %u.%u.\n", info.eax_major_version, info.eax_minor_version);
//    printf ("Supported EFX %u.%u.\n", info.efx_major_version, info.efx_minor_version);
//    printf ("Maximum auxilary sends %u.\n", info.max_aux_sends);

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
    
    printf ("!!! %f\n", SOURCE_UPDATE_TIME / 1000.0f);

    Action timer1 = ActionQueue::PushAction (bind (&TimerHandler, get_pointer (sound_system)), ActionThread_Current, 0, SOURCE_UPDATE_TIME / 1000.0f),
           timer2 = ActionQueue::PushAction (bind (&Application::Exit, 0), ActionThread_Current, TEST_WORK_TIME / 1000.0f);

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
