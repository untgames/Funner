#include "scene_graph.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

template class xtl::declcast<controllers::AnimationController*, Controller*>;

namespace
{

/*
    Константы
*/

const char* SCENE_ANIMATION_LIBRARY            = "Scene.Controllers.Animation";
const char* SCENE_ANIMATION_CONTROLLER_LIBRARY = "Scene.Controllers.AnimationController";
const char* SCENE_ANIMATION_MANAGER_LIBRARY    = "Scene.Controllers.AnimationManager";
const char* SCENE_ANIMATION_EVENT_LIBRARY      = "Scene.Controllers.AnimationEvent";
const char* SCENE_ANIMATION_SEEK_MODE_LIBRARY  = "Scene.Controllers.AnimationSeekMode";

/*
    Вспомогательные функции
*/

void bind_animation_event_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_ANIMATION_EVENT_LIBRARY);
  
  lib.Register ("get_OnPlay", make_const (AnimationEvent_OnPlay));
  lib.Register ("get_OnStop", make_const (AnimationEvent_OnStop));
  lib.Register ("get_OnPause", make_const (AnimationEvent_OnPause));
  lib.Register ("get_OnFinish", make_const (AnimationEvent_OnFinish));
  lib.Register ("get_OnUpdate", make_const (AnimationEvent_OnUpdate));
  lib.Register ("get_OnDestroy", make_const (AnimationEvent_OnDestroy));
  
  environment.RegisterType<AnimationEvent> (SCENE_ANIMATION_EVENT_LIBRARY);
}

void bind_animation_seek_mode_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_ANIMATION_SEEK_MODE_LIBRARY);
  
  lib.Register ("get_Set", make_const (AnimationSeekMode_Set));
  lib.Register ("get_Current", make_const (AnimationSeekMode_Current));
  lib.Register ("get_End", make_const (AnimationSeekMode_End));
  
  environment.RegisterType<AnimationSeekMode> (SCENE_ANIMATION_SEEK_MODE_LIBRARY);  
}

void set_playing (Animation& animation, bool state)
{
  if (state) animation.Play ();
  else       animation.Pause ();
}

void bind_animation_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_ANIMATION_LIBRARY);

    //регистрация операций
    
  lib.Register ("get_Name", make_invoker (&Animation::Name));
  lib.Register ("Play", make_invoker (&Animation::Play));
  lib.Register ("Stop", make_invoker (&Animation::Stop));  
  lib.Register ("Pause", make_invoker (&Animation::Pause));
  lib.Register ("get_Playing", make_invoker (&Animation::IsPlaying));
  lib.Register ("set_Playing", make_invoker (&set_playing));
  lib.Register ("get_Duration", make_invoker (&Animation::Duration));
  lib.Register ("get_Tell", make_invoker (&Animation::Tell));
  lib.Register ("Seek", make_invoker (
    make_invoker<void (Animation&, float, AnimationSeekMode)> (xtl::bind (&Animation::Seek, _1, _2, _3)),
    make_invoker<void (Animation&, float)> (xtl::bind (&Animation::Seek, _1, _2, AnimationSeekMode_Default))
  ));
  lib.Register ("get_Looping", make_invoker (&Animation::IsLooping));
  lib.Register ("set_Looping", make_invoker (&Animation::SetLooping));
  lib.Register ("get_Weight", make_invoker (&Animation::Weight));
  lib.Register ("set_Weight", make_invoker (&Animation::SetWeight));
  lib.Register ("CreateEventHandler", make_callback_invoker<Animation::EventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler", make_invoker (&Animation::RegisterEventHandler));

    //регистрация типа данных

  environment.RegisterType<Animation> (SCENE_ANIMATION_LIBRARY);
}

void bind_animation_controller_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_ANIMATION_CONTROLLER_LIBRARY);
  
    //регистрация порождающей функции
    
  lib.Register ("Create", make_invoker (&AnimationController::Create));

    //регистрация операций  
    
  lib.Register ("get_AnimationsCount", make_invoker (&AnimationController::AnimationsCount));
  lib.Register ("Animation", make_invoker (&AnimationController::Animation));
  lib.Register ("PlayAnimation", make_invoker (
    make_invoker (xtl::implicit_cast<Animation (AnimationController::*)(const char*, const Animation::EventHandler&)> (&AnimationController::PlayAnimation)),
    make_invoker (xtl::implicit_cast<Animation (AnimationController::*)(const char*)> (&AnimationController::PlayAnimation))
  ));
  lib.Register ("PlayAnimation", make_invoker (xtl::implicit_cast<Animation (AnimationController::*)(const char*)> (&AnimationController::CreateAnimation)));
  lib.Register ("StopAllAnimations", make_invoker (&AnimationController::StopAllAnimations));
  lib.Register ("CreateEventHandler", make_callback_invoker<AnimationController::EventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler", make_invoker (&AnimationController::RegisterEventHandler));
    
    //регистрация типа данных

  environment.RegisterType<AnimationController> (SCENE_ANIMATION_CONTROLLER_LIBRARY);
}

AnimationManager create_animation_manager ()
{
  return AnimationManager ();
}

void bind_animation_manager_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_ANIMATION_MANAGER_LIBRARY);
  
    //регистрация порождающей функции
    
  lib.Register ("Create", make_invoker (&create_animation_manager));

    //регистрация операций  
    
  lib.Register ("CreateAnimation", make_invoker (&AnimationManager::CreateAnimation));
  lib.Register ("PlayAnimation", make_invoker (
    make_invoker (xtl::implicit_cast<Animation (AnimationManager::*)(const char*, Node&, const Animation::EventHandler&)> (&AnimationManager::PlayAnimation)),
    make_invoker (xtl::implicit_cast<Animation (AnimationManager::*)(const char*, Node&)> (&AnimationManager::PlayAnimation))
  ));
  lib.Register ("LoadResource", make_invoker (&AnimationManager::LoadResource));
  lib.Register ("UnloadResource", make_invoker (&AnimationManager::UnloadResource));

    //регистрация типа данных

  environment.RegisterType<AnimationManager> (SCENE_ANIMATION_MANAGER_LIBRARY);
}

}

namespace engine
{

namespace scene_graph_script_binds
{

void bind_controller_animation_library (Environment& environment)
{
  bind_animation_event_library      (environment);
  bind_animation_seek_mode_library  (environment);
  bind_animation_library            (environment);
  bind_animation_controller_library (environment);
  bind_animation_manager_library    (environment);
}

}

}
