#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/bind.h>
#include <xtl/ref.h>
#include <syslib/timer.h>
#include <syslib/application.h>
#include <sg/scene.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>
#include <sound/manager.h>
#include <sound/scene_player.h>

using namespace sound;
using namespace xtl;
using namespace syslib;
using namespace scene_graph;
using namespace math;

const size_t TEST_WORK_TIME = 8000;  //время работы теста (в милисекундах)
const size_t ACTION_TIME    = 20;
const char* library_file    = "data/test.snddecl";

float volume_angle = 0;

typedef scene_graph::Listener::Pointer     ListenerPtr;
typedef scene_graph::SoundEmitter::Pointer SoundEmitterPtr;

void TimerHandler (SoundEmitterPtr sound_emitter)
{
  volume_angle += 0.02f;

  sound_emitter->SetGain (abs (sin ((float)volume_angle)));
}

int main ()
{
  printf ("ScenePlayer2 test.\n");

  try
  {
    Scene           scene;
    SoundManager    manager ("OpenAL", "*");
    ListenerPtr     listener (scene_graph::Listener::Create ());
    SoundEmitterPtr sound_emitter  = SoundEmitter::Create ("declaration1");

    listener->BindToParent (scene.Root ());
    sound_emitter->BindToParent (scene.Root ());

    manager.LoadSoundLibrary (library_file);

    ScenePlayer scene_player;

    scene_player.SetListener (listener.get ());

    scene_player.SetManager (&manager);

    sound_emitter->Play ();

    Timer timer (bind (&Application::Exit, 0), TEST_WORK_TIME);
    Timer timer2 (bind (&TimerHandler, sound_emitter), ACTION_TIME);
  
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
