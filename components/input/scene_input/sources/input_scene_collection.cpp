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
      return iter->second;
      
    InputScenePtr input_scene (new InputScene (scene), false);
    
    connect_tracker (scene.Root ().RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&InputSceneCollection::OnSceneDestroyed, this, &scene)));
    
    scenes.insert_pair (&scene, input_scene);
    
    return input_scene;
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
