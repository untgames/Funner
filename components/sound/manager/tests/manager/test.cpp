#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mathlib.h>
#include <common/exception.h>
#include <syslib/window.h>
#include <syslib/timer.h>
#include <syslib/application.h>
#include <xtl/function.h>
#include <xtl/bind.h>
#include <xtl/ref.h>
#include <sound/device.h>
#include <sound/manager.h>

using namespace syslib;
using namespace sound;
using namespace sound::low_level;
using namespace xtl;
using namespace math;

const size_t ACTION_TIME = 1000;
const size_t TEST_WORK_TIME = 15000;  //время работы теста (в милисекундах)
const char* library_file = "data/test.snddecl";

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

void print (const Listener& listener)
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

void print (sound::WindowMinimizeAction minimize_action)
{
  switch (minimize_action)
  {
    case WindowMinimizeAction_Mute:    printf ("Mute\n");   break;
    case WindowMinimizeAction_Pause:   printf ("Pause\n");  break;
    case WindowMinimizeAction_Ignore:  printf ("Ignore\n"); break;
    default:                           printf ("Unknown minimize action\n");
  }
}

void print (bool arg)
{
  if (arg)
    printf ("true\n");
  else 
    printf ("false\n");
}

void TimerHandler (Emitter& emitter)
{
  emitter.SetPosition (emitter.Position () + vec3f (5.f, 0.f, 0.f));
}

int main ()
{
  printf ("Results of test 'test':\n");

  try
  {
    Timer timer (bind (&Application::Exit, 0), TEST_WORK_TIME);

    srand ((unsigned int)time (NULL));

    Window       window;
    SoundManager manager (window, SoundSystem::FindConfiguration ("OpenAL", "*"));
    Listener     listener;
    Emitter      emitter;

    emitter.SetSource ("declaration1");
    emitter.SetSampleIndex (rand ());

/*
   Testing basic manager properties
*/

    printf ("OpenAL configuration:\n%s\n", manager.FindConfiguration ("OpenAL", "*"));

    printf ("Minimize action: ");
    print  (manager.WindowMinimizeAction ());
    manager.SetWindowMinimizeAction (WindowMinimizeAction_Pause);
    printf ("New minimize action: ");
    print  (manager.WindowMinimizeAction ());

    printf ("Initial volume = %f,", manager.Volume ());
    manager.SetVolume (0.7f);
    printf (" new volume = %f.\n", manager.Volume ());

    printf ("Initial mute: "); print (manager.IsMuted ());
    manager.SetMute (true);
    printf ("Mute: "); print (manager.IsMuted ());
    manager.SetMute (false);
    printf ("Mute: "); print (manager.IsMuted ());

    printf ("Initial listener status: "); print (manager.Listener ());
    listener.position  = vec3f (0.f, 1.f, 0.f);
    listener.direction = vec3f (0.f, 0.f, 1.f);
    listener.up        = vec3f (0.f, -1.f, 0.f);
    listener.velocity  = vec3f (1.f, 0.f, 0.f);
    manager.SetListener (listener);
    printf ("New listener status: "); print (manager.Listener ());

    manager.LoadSoundLibrary (library_file);
/*
   Testing basic emitter properties
*/

    printf ("Emitter source - %s\n", emitter.Source ());
    printf ("Initial emitter volume = %f,", emitter.Volume ());
    emitter.SetVolume (0.9f);
    printf (" new emitter volume = %f.\n", emitter.Volume ());

    printf ("Initial position: ");  print (emitter.Position ());
    printf ("\nInitial direction: "); print (emitter.Direction ());
    printf ("\nInitial velocity: ");  print (emitter.Velocity ());
    emitter.SetPosition  (vec3f (-50.f, 2.f, 4.f));
    emitter.SetDirection (vec3f (-2.f, -2.f, -4.f));
    emitter.SetVelocity  (vec3f (0.f, 0.f, 0.1f));
    printf ("\nNew position: ");  print (emitter.Position ());
    printf ("\nNew direction: "); print (emitter.Direction ());
    printf ("\nNew velocity: ");  print (emitter.Velocity ());  printf ("\n");

/*
   Testing manager work
*/
    manager.PlaySound (emitter, 0.3f);

    printf ("Current offset in seconds = %f\n", manager.Tell (emitter));
    printf ("Duration in seconds = %f\n", manager.Duration (emitter));
    printf ("Is Playing = %c\n", manager.IsPlaying (emitter) ? 'y' : 'n');

    Timer timer2 (bind (&TimerHandler, ref (emitter)), ACTION_TIME);

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
