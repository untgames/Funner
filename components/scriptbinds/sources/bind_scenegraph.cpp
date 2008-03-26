#include "shared.h"

#include <xtl/implicit_cast.h>

#include <sg/camera.h>
#include <sg/light.h>
#include <sg/helper.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>
#include <sg/visual_model.h>

using namespace script;
using namespace scene_graph;
using namespace math;
using namespace xtl;

namespace
{

/*
    Константы (имена библиотек)
*/

const char* SCENE_STATIC_NODE_BIND_MODE_LIBRARY       = "static.Scene_NodeBindMode";
const char* SCENE_STATIC_NODE_TRANSFORM_SPACE_LIBRARY = "static.Scene_NodeTransformSpace";
const char* SCENE_STATIC_NODE_TRAVERSE_MODE_LIBRARY   = "static.Scene_NodeTraverseMode";
const char* SCENE_NODE_LIBRARY                        = "Scene.Node";
const char* SCENE_ENTITY_LIBRARY                      = "Scene.Entity";
const char* SCENE_PERSPECTIVE_CAMERA_LIBRARY          = "Scene.PerspectiveCamera";
const char* SCENE_ORTHO_CAMERA_LIBRARY                = "Scene.OrthoCamera";
const char* SCENE_LIGHT_LIBRARY                       = "Scene.Light";
const char* SCENE_DIRECT_LIGHT_LIBRARY                = "Scene.DirectLight";
const char* SCENE_SPOT_LIGHT_LIBRARY                  = "Scene.SpotLight";
const char* SCENE_POINT_LIGHT_LIBRARY                 = "Scene.PointLight";
const char* SCENE_BOX_HELPER_LIBRARY                  = "Scene.BoxHelper";
const char* SCENE_LISTENER_LIBRARY                    = "Scene.Listener";
const char* SCENE_SOUND_EMITTER_LIBRARY               = "Scene.SoundEmitter";
const char* SCENE_VISUAL_MODEL_LIBRARY                = "Scene.VisualModel";

/*
    Создание узлов сцены
*/

Node::Pointer create_node ()
{
  return Node::Create ();
}

/*
   Регистрация библиотеки работы с узлами сцены
*/

void bind_static_node_library (Environment& environment)
{
  InvokerRegistry& node_bind_mode_lib       = environment.CreateLibrary (SCENE_STATIC_NODE_BIND_MODE_LIBRARY);
  InvokerRegistry& node_transform_space_lib = environment.CreateLibrary (SCENE_STATIC_NODE_TRANSFORM_SPACE_LIBRARY);
  InvokerRegistry& node_traverse_mode_lib   = environment.CreateLibrary (SCENE_STATIC_NODE_TRAVERSE_MODE_LIBRARY);

  node_bind_mode_lib.Register       ("get_AddRef",      make_const (NodeBindMode_AddRef));
  node_bind_mode_lib.Register       ("get_Capture",     make_const (NodeBindMode_Capture));
  node_transform_space_lib.Register ("get_Local",       make_const (NodeTransformSpace_Local));
  node_transform_space_lib.Register ("get_Parent",      make_const (NodeTransformSpace_Parent));
  node_transform_space_lib.Register ("get_World",       make_const (NodeTransformSpace_World));
  node_traverse_mode_lib.Register   ("get_TopToBottom", make_const (NodeTraverseMode_TopToBottom));
  node_traverse_mode_lib.Register   ("get_BottomToTop", make_const (NodeTraverseMode_BottomToTop));
}
  
InvokerRegistry& bind_node_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_NODE_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_node));

    //регистрация статических переменных

  bind_static_node_library (environment);

    //регистрация операций

  lib.Register ("set_Name",               make_invoker (&Node::SetName));  
  lib.Register ("set_Position",           make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::SetPosition)));
  lib.Register ("SetPosition",            make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetPosition)));
  lib.Register ("ResetPosition",          make_invoker (&Node::ResetPosition));
  lib.Register ("set_Orientation",        make_invoker (implicit_cast<void (Node::*) (const quatf&)> (&Node::SetOrientation)));
  lib.Register ("SetOrientation",         make_invoker (implicit_cast<void (Node::*) (float, float, float, float)> (&Node::SetOrientation)));
  lib.Register ("SetEulerOrientation",    make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetOrientation)));
  lib.Register ("ResetOrientation",       make_invoker (&Node::ResetOrientation));
  lib.Register ("set_Scale",              make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::SetScale)));
  lib.Register ("SetScale",               make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetScale)));
  lib.Register ("ResetScale",             make_invoker (&Node::ResetScale));
  lib.Register ("set_OrientationInherit", make_invoker (&Node::SetOrientationInherit));
  lib.Register ("set_ScaleInherit",       make_invoker (&Node::SetScaleInherit));

  lib.Register ("get_Name",               make_invoker (&Node::Name));
  lib.Register ("get_Position",           make_invoker (&Node::Position));
  lib.Register ("get_WorldPosition",      make_invoker (&Node::WorldPosition));
  lib.Register ("get_Orientation",        make_invoker (&Node::Orientation));
  lib.Register ("get_WorldOrientation",   make_invoker (&Node::WorldOrientation));
  lib.Register ("get_Scale",              make_invoker (implicit_cast<const vec3f& (Node::*) () const> (&Node::Scale)));
  lib.Register ("get_WorldScale",         make_invoker (&Node::WorldScale));
  lib.Register ("get_OrientationInherit", make_invoker (&Node::IsOrientationInherited));
  lib.Register ("get_ScaleInherit",       make_invoker (&Node::IsScaleInherited));
  lib.Register ("get_LocalTM",            make_invoker (&Node::LocalTM));
  lib.Register ("get_WorldTM",            make_invoker (&Node::WorldTM));
  lib.Register ("get_ParentTM",           make_invoker (&Node::ParentTM));

  lib.Register ("Parent",     make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::Parent)));
  lib.Register ("FirstChild", make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::FirstChild)));
  lib.Register ("LastChild",  make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::LastChild)));
  lib.Register ("PrevChild",  make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::PrevChild)));
  lib.Register ("NextChild",  make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::NextChild)));

  lib.Register ("BindToParent",      make_invoker (&Node::BindToParent));
  lib.Register ("Unbind",            make_invoker (&Node::Unbind));
//  lib.Register ("UnbindChild",       make_invoker (&Node::UnbindChild)));
  lib.Register ("UnbindAllChildren", make_invoker (&Node::UnbindAllChildren));
  
  lib.Register ("ObjectTM", make_invoker (&Node::ObjectTM));

  lib.Register ("Rescale", make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::Scale)));

    //регистрация типов данных

  environment.RegisterType<Node> (SCENE_NODE_LIBRARY);

  return lib;
}

/*
   Регистрация библиотеки работы с объектами сцены
*/

InvokerRegistry& bind_entity_library (Environment& environment, InvokerRegistry& node_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_ENTITY_LIBRARY);

    //наследование

  lib.Register (node_lib);

    //регистрация операций

  lib.Register ("set_Color",  make_invoker (implicit_cast<void (Entity::*) (const vec3f&)> (&Entity::SetColor)));
  lib.Register ("get_Color",  make_invoker (&Entity::Color));

    //регистрация типов данных

  environment.RegisterType<Entity> (SCENE_ENTITY_LIBRARY);

  return lib;
}

/*
    Создание камер
*/

PerspectiveCamera::Pointer create_perspective_camera ()
{
  return PerspectiveCamera::Create ();
}

OrthoCamera::Pointer create_ortho_camera ()
{
  return OrthoCamera::Create ();
}

/*
    Регистрация библиотек работы с камерами
*/

void bind_perspective_camera_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_PERSPECTIVE_CAMERA_LIBRARY);

    //наследование

  lib.Register (entity_lib);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_perspective_camera));

    //регистрация операций

  lib.Register ("set_FovX",  make_invoker (&PerspectiveCamera::SetFovX));
  lib.Register ("set_FovY",  make_invoker (&PerspectiveCamera::SetFovY));
  lib.Register ("set_ZNear", make_invoker (&PerspectiveCamera::SetZNear));
  lib.Register ("set_ZFar",  make_invoker (&PerspectiveCamera::SetZFar));
  lib.Register ("get_FovX",  make_invoker (&PerspectiveCamera::FovX));
  lib.Register ("get_FovY",  make_invoker (&PerspectiveCamera::FovY));
  lib.Register ("get_ZNear", make_invoker (&PerspectiveCamera::ZNear));
  lib.Register ("get_ZFar",  make_invoker (&PerspectiveCamera::ZFar));

    //регистрация типов данных

  environment.RegisterType<PerspectiveCamera> (SCENE_PERSPECTIVE_CAMERA_LIBRARY);
}

void bind_ortho_camera_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_ORTHO_CAMERA_LIBRARY);

    //наследование

  lib.Register (entity_lib);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_ortho_camera));

    //регистрация операций

  lib.Register ("set_Left",   make_invoker (&OrthoCamera::SetLeft));
  lib.Register ("set_Right",  make_invoker (&OrthoCamera::SetRight));
  lib.Register ("set_Top",    make_invoker (&OrthoCamera::SetTop));
  lib.Register ("set_Bottom", make_invoker (&OrthoCamera::SetBottom));
  lib.Register ("set_ZNear",  make_invoker (&OrthoCamera::SetZNear));
  lib.Register ("set_ZFar",   make_invoker (&OrthoCamera::SetZFar));
  lib.Register ("get_Left",   make_invoker (&OrthoCamera::Left));
  lib.Register ("get_Right",  make_invoker (&OrthoCamera::Right));
  lib.Register ("get_Top",    make_invoker (&OrthoCamera::Top));
  lib.Register ("get_Bottom", make_invoker (&OrthoCamera::Bottom));
  lib.Register ("get_ZNear",  make_invoker (&OrthoCamera::ZNear));
  lib.Register ("get_ZFar",   make_invoker (&OrthoCamera::ZFar));

    //регистрация типов данных

  environment.RegisterType<OrthoCamera> (SCENE_ORTHO_CAMERA_LIBRARY);
}

/*
    Создание источников света
*/

DirectLight::Pointer create_direct_light ()
{
  return DirectLight::Create ();
}

SpotLight::Pointer create_spot_light ()
{
  return SpotLight::Create ();
}

PointLight::Pointer create_point_light ()
{
  return PointLight::Create ();
}

/*
    Регистрация библиотек работы с источниками света
*/

void bind_direct_light_library (Environment& environment, InvokerRegistry& light_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_DIRECT_LIGHT_LIBRARY);

    //наследование

  lib.Register (light_lib);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_direct_light));

    //регистрация операций

  lib.Register ("set_Radius", make_invoker (&DirectLight::SetRadius));
  lib.Register ("get_Radius", make_invoker (&DirectLight::Radius));

    //регистрация типов данных

  environment.RegisterType<DirectLight> (SCENE_DIRECT_LIGHT_LIBRARY);
}

void bind_spot_light_library (Environment& environment, InvokerRegistry& light_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SPOT_LIGHT_LIBRARY);

    //наследование

  lib.Register (light_lib);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_spot_light));

    //регистрация операций

  lib.Register ("set_Angle",    make_invoker (&SpotLight::SetAngle));
  lib.Register ("set_Exponent", make_invoker (&SpotLight::SetExponent));
  lib.Register ("get_Angle",    make_invoker (&SpotLight::Angle));
  lib.Register ("get_Exponent", make_invoker (&SpotLight::Exponent));

    //регистрация типов данных

  environment.RegisterType<SpotLight> (SCENE_SPOT_LIGHT_LIBRARY);
}

void bind_point_light_library (Environment& environment, InvokerRegistry& light_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_POINT_LIGHT_LIBRARY);

    //наследование

  lib.Register (light_lib);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_point_light));

    //регистрация типов данных

  environment.RegisterType<PointLight> (SCENE_POINT_LIGHT_LIBRARY);
}

void bind_light_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_LIGHT_LIBRARY);

    //наследование

  lib.Register (entity_lib);

    //регистрация операций

  lib.Register ("set_LightColor",  make_invoker (&Light::SetLightColor));
  lib.Register ("set_Attenuation", make_invoker (&Light::SetAttenuation));
  lib.Register ("set_Range",       make_invoker (&Light::SetRange));
  lib.Register ("get_LightColor",  make_invoker (&Light::LightColor));
  lib.Register ("get_Attenuation", make_invoker (&Light::Attenuation));
  lib.Register ("get_Range",       make_invoker (&Light::Range));

    //регистрация библиотек

  bind_direct_light_library (environment, lib);
  bind_spot_light_library   (environment, lib);
  bind_point_light_library  (environment, lib);

    //регистрация типов данных

  environment.RegisterType<Light> (SCENE_LIGHT_LIBRARY);
}

/*
    Создание хелперов
*/

helpers::Box::Pointer create_box_helper ()
{
  return helpers::Box::Create ();
}

/*
   Регистрация библиотеки работы с хелперами
*/

void bind_box_helper_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_BOX_HELPER_LIBRARY);

    //наследование

  lib.Register (entity_lib);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_box_helper));

    //регистрация типов данных

  environment.RegisterType<helpers::Box> (SCENE_BOX_HELPER_LIBRARY);
}

/*
    Создание слушателя
*/

Listener::Pointer create_listener ()
{
  return Listener::Create ();
}

/*
   Регистрация библиотеки работы с слушателями
*/

void bind_listener_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_LISTENER_LIBRARY);

    //наследование

  lib.Register (entity_lib);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_listener));

    //регистрация операций

  lib.Register ("set_Gain", make_invoker (&Listener::SetGain));
  lib.Register ("get_Gain", make_invoker (&Listener::Gain));

    //регистрация типов данных

  environment.RegisterType<Listener> (SCENE_LISTENER_LIBRARY);
}

/*
    Создание источника звука
*/

SoundEmitter::Pointer create_sound_emitter (const char* sound_declaration_name)
{
  return SoundEmitter::Create (sound_declaration_name);
}

/*
   Регистрация библиотеки работы с источниками звука
*/

void bind_sound_emitter_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SOUND_EMITTER_LIBRARY);

    //наследование

  lib.Register (entity_lib);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_sound_emitter));

    //регистрация операций

  lib.Register ("Play", make_invoker (&SoundEmitter::Play));
  lib.Register ("Stop", make_invoker (&SoundEmitter::Stop));

    //регистрация типов данных

  environment.RegisterType<SoundEmitter> (SCENE_SOUND_EMITTER_LIBRARY);
}

/*
    Создание модели
*/

VisualModel::Pointer create_visual_model ()
{
  return VisualModel::Create ();
}

/*
   Регистрация библиотеки работы с моделями
*/

void bind_visual_model_library (Environment& environment, InvokerRegistry& entity_lib)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_VISUAL_MODEL_LIBRARY);

    //наследование

  lib.Register (entity_lib);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_visual_model));

    //регистрация операций

  lib.Register ("set_MeshName", make_invoker (&VisualModel::SetMeshName));
  lib.Register ("get_MeshName", make_invoker (&VisualModel::MeshName));

    //регистрация типов данных

  environment.RegisterType<VisualModel> (SCENE_VISUAL_MODEL_LIBRARY);
}

}

namespace script
{

/*
    Регистрация библиотеки работы со сценой
*/

void bind_scene_graph_library (Environment& environment)
{
    //регистрация библиотек
  
  InvokerRegistry& node_class_lib   = bind_node_library (environment);
  InvokerRegistry& entity_class_lib = bind_entity_library (environment, node_class_lib);

  bind_perspective_camera_library (environment, entity_class_lib);
  bind_ortho_camera_library       (environment, entity_class_lib);
  bind_light_library              (environment, entity_class_lib);
  bind_box_helper_library         (environment, entity_class_lib);
  bind_listener_library           (environment, entity_class_lib);
  bind_sound_emitter_library      (environment, entity_class_lib);
  bind_visual_model_library       (environment, entity_class_lib);
}

}
