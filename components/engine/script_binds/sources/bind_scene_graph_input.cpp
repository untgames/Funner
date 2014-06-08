#include "scene_graph.h"

using namespace scene_graph;

template class engine::decl_sg_cast<InputZoneModel, Node>;
template class engine::decl_sg_cast<InputZoneModel, Entity>;
template class engine::decl_sg_cast<InputZoneModel, VisualModel>;
template class engine::decl_sg_cast<InputZone,      InputZoneModel>;
template class engine::decl_sg_cast<InputZone,      Node>;
template class engine::decl_sg_cast<InputZone,      Entity>;
template class engine::decl_sg_cast<InputZone,      VisualModel>;
template class engine::decl_sg_cast<InputZoneList,  InputZoneModel>;
template class engine::decl_sg_cast<InputZoneList,  Node>;
template class engine::decl_sg_cast<InputZoneList,  Entity>;
template class engine::decl_sg_cast<InputZoneList,  VisualModel>;

namespace engine
{

namespace scene_graph_script_binds
{

/*
    Регистрация библиотеки InputZoneNotification
*/

void bind_input_zone_notification_library (script::Environment& env)
{
  InvokerRegistry lib = env.CreateLibrary (SCENE_STATIC_INPUT_ZONE_NOTIFICATION_LIBRARY);
  
  lib.Register ("get_OnPress",            make_const (InputZoneNotification_OnPress));
  lib.Register ("get_OnPressEnter",       make_const (InputZoneNotification_OnPressEnter));
  lib.Register ("get_OnPressMove",        make_const (InputZoneNotification_OnPressMove));  
  lib.Register ("get_OnPressLeave",       make_const (InputZoneNotification_OnPressLeave));
  lib.Register ("get_OnClick",            make_const (InputZoneNotification_OnClick));
  lib.Register ("get_OnTouchEnter",       make_const (InputZoneNotification_OnTouchEnter));
  lib.Register ("get_OnTouchLeave",       make_const (InputZoneNotification_OnTouchLeave));
  lib.Register ("get_OnTouchMove",        make_const (InputZoneNotification_OnTouchMove));
  lib.Register ("get_OnTouchDown",        make_const (InputZoneNotification_OnTouchDown));
  lib.Register ("get_OnTouchUpInside",    make_const (InputZoneNotification_OnTouchUpInside));
  lib.Register ("get_OnTouchUpOutside",   make_const (InputZoneNotification_OnTouchUpOutside));
  lib.Register ("get_OnTouchClick",       make_const (InputZoneNotification_OnTouchUpOutside));
  lib.Register ("get_OnScreenTouchDown",  make_const (InputZoneNotification_OnScreenTouchDown));
  lib.Register ("get_OnScreenTouchUp",    make_const (InputZoneNotification_OnScreenTouchUp));
  lib.Register ("get_OnScreenTouchMove",  make_const (InputZoneNotification_OnScreenTouchMove));
}

/*
    Регистрация библиотеки InputZoneNotificationContext
*/

namespace
{

size_t              get_touch_id             (const InputZoneNotificationContext& context) { return context.touch_id; }
int                 get_button               (const InputZoneNotificationContext& context) { return context.button; }
const math::vec3f&  get_touch_world_position (const InputZoneNotificationContext& context) { return context.touch_world_position; }
const math::vec2f&  get_touch_local_position (const InputZoneNotificationContext& context) { return context.touch_local_position; }

}

void bind_input_zone_notification_context_library (script::Environment& env)
{
  InvokerRegistry lib = env.CreateLibrary (SCENE_INPUT_ZONE_NOTIFICATION_CONTEXT_LIBRARY);
  
    //регистрация функций

  lib.Register ("get_TouchId",            make_invoker (&get_touch_id));
  lib.Register ("get_Button",             make_invoker (&get_button));
  lib.Register ("get_TouchWorldPosition", make_invoker (&get_touch_world_position));
  lib.Register ("get_TouchLocalPosition", make_invoker (&get_touch_local_position));

    //регистрация типов данных

  env.RegisterType<InputZoneNotificationContext> (SCENE_INPUT_ZONE_NOTIFICATION_CONTEXT_LIBRARY);
}

/*
    Регистрация библиотеки InputZoneModel
*/

namespace
{

xtl::connection register_notification_handler (InputZoneModel& zone, InputZoneNotification notification, const xtl::function<void (const Viewport& viewport, InputZoneNotification notification, const InputZoneNotificationContext&)>& handler)
{
  return zone.RegisterNotificationHandler (notification, xtl::bind (handler, _2, _3, _4));
}

xtl::connection register_default_notification_handler (InputZoneModel& zone, const xtl::function<void (const Viewport& viewport, InputZoneNotification notification, const InputZoneNotificationContext&)>& handler)
{
  return zone.RegisterNotificationHandler (xtl::bind (handler, _2, _3, _4));
}

}

void bind_input_zone_model_library (script::Environment& env)
{
  InvokerRegistry lib = env.CreateLibrary (SCENE_INPUT_ZONE_MODEL_LIBRARY);
  
    //наследование

  lib.Register (env, SCENE_VISUAL_MODEL_LIBRARY);  

    //регистрация операций
    
  lib.Register ("get_ZonesCount",              make_invoker (&InputZoneModel::ZoneDescsCount));
  lib.Register ("get_BoundsAutoUpdate",        make_invoker (&InputZoneModel::BoundsAutoUpdate));
  lib.Register ("set_BoundsAutoUpdate",        make_invoker (&InputZoneModel::SetBoundsAutoUpdate));
  lib.Register ("get_Active",                  make_invoker (&InputZoneModel::IsActive));
  lib.Register ("set_Active",                  make_invoker (&InputZoneModel::SetActive));
  lib.Register ("IsIntersected",               make_invoker (&InputZoneModel::IsIntersected));
  lib.Register ("HasNotificationHandler",      make_invoker (&InputZoneModel::HasNotificationHandler));
  lib.Register ("RegisterNotificationHandler", make_invoker (
    make_invoker (&register_notification_handler),
    make_invoker (&register_default_notification_handler)
  ));
  
  lib.Register ("CreateNotificationHandler", make_callback_invoker<void (const Viewport& viewport, InputZoneNotification notification, const InputZoneNotificationContext&)> ());
  
  lib.Register ("GetNotificationName", make_invoker (xtl::implicit_cast<const char* (*)(InputZoneNotification)> (&get_name)));

    //регистрация типов данных

  env.RegisterType<InputZoneModel> (SCENE_INPUT_ZONE_MODEL_LIBRARY);
}

/*
    Регистрация библиотеки InputZone
*/

namespace
{

InputZone::Pointer create_input_zone ()
{
  return InputZone::Create ();
}

}

void bind_input_zone_library (script::Environment& env)
{
  InvokerRegistry lib = env.CreateLibrary (SCENE_INPUT_ZONE_LIBRARY);
  
    //наследование

  lib.Register (env, SCENE_INPUT_ZONE_MODEL_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_input_zone));

    //регистрация операций
    
  lib.Register ("SetPlaneState",  make_invoker (&InputZone::SetPlaneState));
  lib.Register ("IsPlaneEnabled", make_invoker (&InputZone::IsPlaneEnabled));
  lib.Register ("EnablePlane",    make_invoker (&InputZone::EnablePlane));
  lib.Register ("DisablePlane",   make_invoker (&InputZone::DisablePlane));
    
    //регистрация типов данных

  env.RegisterType<InputZone> (SCENE_INPUT_ZONE_LIBRARY);
}

/*
    Регистрация библиотек взаимодействия с зонами ввода
*/

void bind_input_library (script::Environment& env)
{
  bind_input_zone_notification_library         (env);
  bind_input_zone_notification_context_library (env);  
  bind_input_zone_model_library                (env);
  bind_input_zone_library                      (env);
}

}

}
