#include "scene_graph.h"

#include <media/rms/group.h>

using namespace scene_graph;

namespace
{

/*
    Константы
*/

const char* SCENE_MANAGER_LIBRARY = "Scene.SceneManager";

/*
    Вспомогательные функции
*/

SceneManager create_scene_manager ()
{
  return SceneManager ();
}

}

namespace engine
{

namespace scene_graph_script_binds
{

void bind_scene_manager_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_MANAGER_LIBRARY);
  
    //регистрация порождающей функции
    
  lib.Register ("Create", make_invoker (&create_scene_manager));

    //регистрация операций  
    
  lib.Register ("LoadScene", make_invoker (
    make_invoker<void (SceneManager&, const char*, const char*, const SceneManager::LogHandler&)> (xtl::bind (xtl::implicit_cast<void (SceneManager::*)(const char*, const char*, const SceneManager::LogHandler&)> (&SceneManager::LoadScene), _1, _2, _3, _4)),
    make_invoker<void (SceneManager&, const char*, const char*)> (xtl::bind (xtl::implicit_cast<void (SceneManager::*)(const char*, const char*)> (&SceneManager::LoadScene), _1, _2, _3)),
    make_invoker<void (SceneManager&, const char*)> (xtl::bind (xtl::implicit_cast<void (SceneManager::*)(const char*, const char*)> (&SceneManager::LoadScene), _1, _2, ""))
  ));
  lib.Register ("UnloadScene", make_invoker (&SceneManager::UnloadScene));
  lib.Register ("SaveScene", make_invoker (&SceneManager::SaveScene));  
  lib.Register ("HasScene", make_invoker (&SceneManager::HasScene));
  lib.Register ("SceneResources", make_invoker (&SceneManager::SceneResources));  
  lib.Register ("get_Resources", make_invoker (&SceneManager::Resources));    
  lib.Register ("CreateScene", make_invoker (xtl::implicit_cast<Node::Pointer (SceneManager::*)(const char*) const> (&SceneManager::CreateScene)));
  lib.Register ("CreateLogHandler", make_callback_invoker<SceneManager::LogHandler::signature_type> ());
    
    //регистрация типа данных

  environment.RegisterType<SceneManager> (SCENE_MANAGER_LIBRARY);
}

}

}
