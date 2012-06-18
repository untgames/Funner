#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
    Конструктор / деструктор
*/

InputSceneCollection::InputSceneCollection ()
{
}

InputSceneCollection::~InputSceneCollection ()
{
  scenes.clear ();
}

/*
    Поиск сцены
*/

InputScenePtr InputSceneCollection::GetScene (scene_graph::Scene& scene)
{
  try
  {
    InputSceneMap::iterator iter = scenes.find (&scene);
    
    if (iter != scenes.end ())
      return iter->second.scene;
      
    SceneDesc& desc = scenes [&scene];
    
    try
    {
      desc.scene              = InputScenePtr (new InputScene (scene), false);
      desc.on_scene_destroyed = scene.Root ().RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&InputSceneCollection::OnSceneDestroyed, this, &scene));
      
      return desc.scene;
    }
    catch (...)
    {
      scenes.erase (&scene);
      throw;
    }      
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::InputSceneCollection::GetScene");
    throw;
  }
}

/*
    Оповещение об удалении сцены
*/

void InputSceneCollection::OnSceneDestroyed (scene_graph::Scene* scene)
{
  scenes.erase (scene);
}
