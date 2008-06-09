#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/bind.h>

#include <syslib/window.h>
#include <syslib/timer.h>
#include <syslib/application.h>

#include <sg/node.h>
#include <sg/scene.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>

#include <sound/manager.h>
#include <sound/scene_player.h>
#include <sound/device.h>

using namespace sound;
using namespace sound::low_level;
using namespace syslib;
using namespace scene_graph;

const char* library_file = "data/test.snddecl";

const size_t TEST_WORK_TIME = 8000;  //����� ������ ����� (� ������������)

typedef scene_graph::Listener::Pointer     ListenerPtr;
typedef scene_graph::SoundEmitter::Pointer SoundEmitterPtr;

SoundEmitterPtr play_sound (const char* declaration_name, scene_graph::Scene& scene)
{
  SoundEmitterPtr sound_emitter = scene_graph::SoundEmitter::Create (declaration_name);

  sound_emitter->BindToParent (scene.Root (), NodeBindMode_Capture);

  sound_emitter->Play ();

  return sound_emitter;
}

int main ()
{
  printf ("EmitterAutoDelete test.\n");

  try
  {
    scene_graph::Scene scene;
    Window             window;
    SoundManager       manager (window, SoundSystem::FindConfiguration ("OpenAL", "*"));
    ListenerPtr        listener (scene_graph::Listener::Create ());
    
    listener->BindToParent (scene.Root ());

    manager.LoadSoundLibrary (library_file);

    ScenePlayer scene_player;

    scene_player.SetListener (listener.get ());

    scene_player.SetManager (&manager);

    ((Node*)play_sound ("declaration2", scene).get ())->RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&Application::Exit, 0));

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
