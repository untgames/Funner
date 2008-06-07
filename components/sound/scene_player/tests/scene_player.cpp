#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/bind.h>
#include <xtl/ref.h>
#include <syslib/window.h>
#include <syslib/timer.h>
#include <syslib/application.h>
#include <sg/scene.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>
#include <sound/manager.h>
#include <sound/scene_player.h>
#include <sound/device.h>

using namespace sound;
using namespace sound::low_level;
using namespace xtl;
using namespace syslib;
using namespace scene_graph;
using namespace math;

const size_t TEST_WORK_TIME = 8000;  //время работы теста (в милисекундах)
const size_t ACTION_TIME    = 5000;
const size_t ACTION2_TIME   = 250;
const char* library_file    = "data/test.snddecl";

typedef scene_graph::Listener::Pointer     ListenerPtr;
typedef scene_graph::SoundEmitter::Pointer SoundEmitterPtr;

void TimerHandler (SoundEmitterPtr sound_emitter, SoundEmitterPtr sound_emitter2)
{
  sound_emitter->Stop ();
  sound_emitter2->Play ();
}

void TimerHandler2 (SoundEmitterPtr sound_emitter, SoundManager& manager, ScenePlayer& scene_player, bool& set_manager)
{
  sound_emitter->SetPosition (sound_emitter->Position () * -1.f);

  if (set_manager)
    scene_player.SetManager (&manager);
  else
    scene_player.SetManager (0);
  
  set_manager = !set_manager;
}

int main ()
{
  printf ("ScenePlayer test.\n");

  try
  {
    Scene           scene;
    Window          window;
    SoundManager    manager (window, SoundSystem::FindConfiguration ("OpenAL", "SB*"));
    SoundManager    manager2 (window, SoundSystem::FindConfiguration ("OpenAL", "Generic*Real*"));
    ListenerPtr     listener (scene_graph::Listener::Create ());
    SoundEmitterPtr sound_emitter  = SoundEmitter::Create ("declaration1"),
                    sound_emitter2 = SoundEmitter::Create ("declaration2");
    bool            set_manager = false;

    listener->BindToParent (scene.Root ());
    sound_emitter->BindToParent (scene.Root ());
    sound_emitter->SetPosition (0.1f, 0.1f, 0.1f);

    manager.LoadSoundLibrary (library_file);
    manager2.LoadSoundLibrary (library_file);

    ScenePlayer scene_player;
    ScenePlayer scene_player2;

    scene_player.SetListener (listener.get ());
    scene_player2.SetListener (listener.get ());

    scene_player.SetManager (&manager);
    scene_player2.SetManager (&manager2);

    sound_emitter2->BindToParent (scene.Root ());

    srand (clock ());

    sound_emitter->Play ();

    Timer timer (bind (&Application::Exit, 0), TEST_WORK_TIME);
//    Timer timer2 (bind (&TimerHandler, sound_emitter, sound_emitter2), ACTION_TIME);
    Timer timer3 (bind (&TimerHandler2, sound_emitter, ref (manager), ref (scene_player), ref (set_manager)), ACTION2_TIME);
  
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
