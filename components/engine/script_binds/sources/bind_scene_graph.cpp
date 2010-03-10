#include "shared.h"

#include <math/angle.h>
#include <math/matrix.h>
#include <math/quat.h>

#include <sg/camera.h>
#include <sg/height_map.h>
#include <sg/helper.h>
#include <sg/light.h>
#include <sg/listener.h>
#include <sg/node_array.h>
#include <sg/scene.h>
#include <sg/sound_emitter.h>
#include <sg/sprite.h>
#include <sg/text_line.h>
#include <sg/visual_model.h>

using namespace script;
using namespace scene_graph;
using namespace math;
using namespace xtl;

/*
    Взаимодействие math::angle со стеком аргументов
*/

namespace script
{

namespace detail
{

template <class T>
inline T make_invoker_argument (const math::angle<T>& angle)
{
  return degree (angle);
}

template <class T>
inline T make_invoker_argument (math::angle<T>& angle)
{
  return degree (angle);
}

template <class T>
struct argument_selector<math::angle<T> >
{
  typedef float dump_type;

  static math::angle<T> get (IStack& stack, size_t index) { return degree (stack.GetFloat (index)); }
};

}

}

namespace
{

/*
    Константы (имена библиотек)
*/

const char* SCENE_STATIC_NODE_BIND_MODE_LIBRARY       = "Scene.NodeBindMode";
const char* SCENE_STATIC_NODE_TRANSFORM_SPACE_LIBRARY = "Scene.NodeTransformSpace";
const char* SCENE_STATIC_NODE_TRAVERSE_MODE_LIBRARY   = "Scene.NodeTraverseMode";
const char* SCENE_STATIC_NODE_SEARCH_MODE_LIBRARY     = "Scene.NodeSearchMode";
const char* SCENE_STATIC_NODE_ORT_LIBRARY             = "Scene.NodeOrt";
const char* SCENE_STATIC_NODE_EVENT_LIBRARY           = "Scene.NodeEvent";
const char* SCENE_STATIC_NODE_SUBTREE_EVENT_LIBRARY   = "Scene.NodeSubTreeEvent";
const char* SCENE_STATIC_NODE_ARRAY_LINK_MODE_LIBRARY = "Scene.NodeArrayLinkMode";
const char* SCENE_STATIC_TEXT_LINE_ALIGNMENT_LIBRARY  = "Scene.TextLineAlignment";
const char* SCENE_STATIC_NODE_PROPERTY_TYPE_LIBRARY   = "Scene.NodePropertyType";
const char* SCENE_SCENE_LIBRARY                       = "Scene.Scene";
const char* SCENE_NODE_LIBRARY                        = "Scene.Node";
const char* SCENE_NODE_PROPERTIES_LIBRARY             = "Scene.NodeProperties";
const char* SCENE_NODE_ARRAY_LIBRARY                  = "Scene.NodeArray";
const char* SCENE_ENTITY_LIBRARY                      = "Scene.Entity";
const char* SCENE_PERSPECTIVE_CAMERA_LIBRARY          = "Scene.PerspectiveCamera";
const char* SCENE_ORTHO_CAMERA_LIBRARY                = "Scene.OrthoCamera";
const char* SCENE_LIGHT_LIBRARY                       = "Scene.Light";
const char* SCENE_DIRECT_LIGHT_LIBRARY                = "Scene.DirectLight";
const char* SCENE_SPOT_LIGHT_LIBRARY                  = "Scene.SpotLight";
const char* SCENE_POINT_LIGHT_LIBRARY                 = "Scene.PointLight";
const char* SCENE_BOX_HELPER_LIBRARY                  = "Scene.Helpers.Box";
const char* SCENE_LISTENER_LIBRARY                    = "Scene.Listener";
const char* SCENE_SOUND_EMITTER_LIBRARY               = "Scene.SoundEmitter";
const char* SCENE_SPRITE_MODEL_LIBRARY                = "Scene.SpriteModel";
const char* SCENE_SPRITE_LIBRARY                      = "Scene.Sprite";
const char* SCENE_TEXT_LINE_LIBRARY                   = "Scene.TextLine";
const char* SCENE_VISUAL_MODEL_LIBRARY                = "Scene.VisualModel";
const char* SCENE_HEIGHT_MAP_LIBRARY                  = "Scene.HeightMap";
const char* BINDER_NAME                               = "SceneGraph";
const char* COMPONENT_NAME                            = "script.binds.SceneGraph";

/*
    Создание сцены
*/

shared_ptr<Scene> create_scene ()
{
  return shared_ptr<Scene> (new Scene ());
}

Node::Pointer get_scene_root (Scene& scene)
{
  return &scene.Root ();
}

/*
   Регистрация библиотеки работы со сценой
*/

void bind_scene_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SCENE_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_scene));

    //регистрация операций

  lib.Register ("set_Name", make_invoker (&Scene::SetName));
  lib.Register ("get_Name", make_invoker (&Scene::Name));

  lib.Register ("get_Root", make_invoker (&get_scene_root));

  lib.Register ("get_EntitiesCount", make_invoker (&Scene::EntitiesCount));

  environment.RegisterType<Scene> (SCENE_SCENE_LIBRARY);
}

/*
    Создание узлов сцены
*/

Node::Pointer create_node ()
{
  return Node::Create ();
}

}

namespace scene_graph
{

void push_argument (IStack& stack, Node& node)
{
  stack.Push (xtl::any (&node, true));
}

}

namespace
{

/*
   Регистрация библиотеки работы с узлами сцены
*/

void bind_static_node_library (Environment& environment)
{
  InvokerRegistry& node_bind_mode_lib       = environment.CreateLibrary (SCENE_STATIC_NODE_BIND_MODE_LIBRARY);
  InvokerRegistry& node_transform_space_lib = environment.CreateLibrary (SCENE_STATIC_NODE_TRANSFORM_SPACE_LIBRARY);
  InvokerRegistry& node_traverse_mode_lib   = environment.CreateLibrary (SCENE_STATIC_NODE_TRAVERSE_MODE_LIBRARY);
  InvokerRegistry& node_search_mode_lib     = environment.CreateLibrary (SCENE_STATIC_NODE_SEARCH_MODE_LIBRARY);
  InvokerRegistry& node_ort_lib             = environment.CreateLibrary (SCENE_STATIC_NODE_ORT_LIBRARY);
  InvokerRegistry& node_event_lib           = environment.CreateLibrary (SCENE_STATIC_NODE_EVENT_LIBRARY);
  InvokerRegistry& node_subtree_event_lib   = environment.CreateLibrary (SCENE_STATIC_NODE_SUBTREE_EVENT_LIBRARY);
  InvokerRegistry& node_property_type_lib   = environment.CreateLibrary (SCENE_STATIC_NODE_PROPERTY_TYPE_LIBRARY);

  node_bind_mode_lib.Register       ("get_AddRef",            make_const (NodeBindMode_AddRef));
  node_bind_mode_lib.Register       ("get_WeakRef",           make_const (NodeBindMode_WeakRef));
  node_bind_mode_lib.Register       ("get_Default",           make_const (NodeBindMode_Default));
  node_transform_space_lib.Register ("get_Local",             make_const (NodeTransformSpace_Local));
  node_transform_space_lib.Register ("get_Parent",            make_const (NodeTransformSpace_Parent));
  node_transform_space_lib.Register ("get_World",             make_const (NodeTransformSpace_World));
  node_traverse_mode_lib.Register   ("get_TopToBottom",       make_const (NodeTraverseMode_TopToBottom));
  node_traverse_mode_lib.Register   ("get_BottomToTop",       make_const (NodeTraverseMode_BottomToTop));
  node_traverse_mode_lib.Register   ("get_Default",           make_const (NodeTraverseMode_Default));
  node_search_mode_lib.Register     ("get_OnNextSublevel",    make_const (NodeSearchMode_OnNextSublevel));
  node_search_mode_lib.Register     ("get_OnAllSublevels",    make_const (NodeSearchMode_OnAllSublevels));
  node_search_mode_lib.Register     ("get_Default",           make_const (NodeSearchMode_Default));
  node_ort_lib.Register             ("get_X",                 make_const (NodeOrt_X));
  node_ort_lib.Register             ("get_Y",                 make_const (NodeOrt_Y));
  node_ort_lib.Register             ("get_Z",                 make_const (NodeOrt_Z));
  node_event_lib.Register           ("get_AfterUpdate",       make_const (NodeEvent_AfterUpdate));
  node_event_lib.Register           ("get_BeforeDestroy",     make_const (NodeEvent_BeforeDestroy));
  node_event_lib.Register           ("get_AfterDestroy",      make_const (NodeEvent_AfterDestroy));
  node_event_lib.Register           ("get_AfterBind",         make_const (NodeEvent_AfterBind));
  node_event_lib.Register           ("get_BeforeUnbind",      make_const (NodeEvent_BeforeUnbind));
  node_event_lib.Register           ("get_AfterSceneAttach",  make_const (NodeEvent_AfterSceneAttach));
  node_event_lib.Register           ("get_BeforeSceneDetach", make_const (NodeEvent_BeforeSceneDetach));
  node_event_lib.Register           ("get_AfterSceneChange",  make_const (NodeEvent_AfterSceneChange));
  node_subtree_event_lib.Register   ("get_AfterBind",         make_const (NodeSubTreeEvent_AfterBind));
  node_subtree_event_lib.Register   ("get_BeforeUnbind",      make_const (NodeSubTreeEvent_BeforeUnbind));
  node_property_type_lib.Register   ("get_String",            make_const (NodePropertyType_String));
  node_property_type_lib.Register   ("get_Integer",           make_const (NodePropertyType_Integer));
  node_property_type_lib.Register   ("get_Float",             make_const (NodePropertyType_Float));
  node_property_type_lib.Register   ("get_Vector",            make_const (NodePropertyType_Vector));
  node_property_type_lib.Register   ("get_Matrix",            make_const (NodePropertyType_Matrix));
}

//получение уникального идентификатора узла
void* get_node_id (Node& node)
{
  return &node;
}

InvokerRegistry& bind_node_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_NODE_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_node));

    //регистрация статических переменных

  bind_static_node_library (environment);

    //регистрация операций

  lib.Register ("get_Id",                   make_invoker (&get_node_id));
  lib.Register ("set_Name",                 make_invoker (&Node::SetName));
  lib.Register ("set_Properties",           make_invoker (&Node::SetProperties));
  lib.Register ("get_Properties",           make_invoker (implicit_cast<NodeProperties::Pointer (Node::*) ()> (&Node::Properties)));
  lib.Register ("set_Position",             make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::SetPosition)));
  lib.Register ("set_WorldPosition",        make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::SetWorldPosition)));
  lib.Register ("SetPosition",              make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetPosition)));
  lib.Register ("SetWorldPosition",         make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetWorldPosition)));
  lib.Register ("ResetPosition",            make_invoker (&Node::ResetPosition));
  lib.Register ("ResetPivotPosition",       make_invoker (&Node::ResetPivotPosition));
  lib.Register ("SetPivotPosition",         make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetPivotPosition)));
  lib.Register ("set_PivotPosition",        make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::SetPivotPosition)));
  lib.Register ("get_PivotPosition",        make_invoker (&Node::PivotPosition));
  lib.Register ("set_OrientationPivotEnabled", make_invoker (&Node::SetOrientationPivotEnabled));
  lib.Register ("set_ScalePivotEnabled",       make_invoker (&Node::SetScalePivotEnabled));
  lib.Register ("get_OrientationPivotEnabled", make_invoker (&Node::OrientationPivotEnabled));
  lib.Register ("get_ScalePivotEnabled",       make_invoker (&Node::ScalePivotEnabled));
  lib.Register ("get_PivotEnabled",         make_invoker (&Node::PivotEnabled));
  lib.Register ("set_Orientation",          make_invoker (implicit_cast<void (Node::*) (const quatf&)> (&Node::SetOrientation)));
  lib.Register ("set_WorldOrientation",     make_invoker (implicit_cast<void (Node::*) (const quatf&)> (&Node::SetWorldOrientation)));
  lib.Register ("SetOrientation",           make_invoker<void (Node*, math::anglef, float, float, float)> (implicit_cast<void (Node::*) (const math::anglef&, float, float, float)> (&Node::SetOrientation)));
  lib.Register ("SetWorldOrientation",      make_invoker<void (Node*, math::anglef, float, float, float)> (implicit_cast<void (Node::*) (const math::anglef&, float, float, float)> (&Node::SetWorldOrientation)));
  lib.Register ("SetEulerOrientation",      make_invoker<void (Node*, math::anglef, math::anglef, math::anglef)> (implicit_cast<void (Node::*) (const math::anglef&, const math::anglef&, const math::anglef&)> (&Node::SetOrientation)));
  lib.Register ("SetWorldEulerOrientation", make_invoker<void (Node*, math::anglef, math::anglef, math::anglef)> (implicit_cast<void (Node::*) (const math::anglef&, const math::anglef&, const math::anglef&)> (&Node::SetWorldOrientation)));
  lib.Register ("ResetOrientation",         make_invoker (&Node::ResetOrientation));
  lib.Register ("set_Scale",                make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::SetScale)));
  lib.Register ("set_WorldScale",           make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::SetWorldScale)));
  lib.Register ("SetScale",                 make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetScale)));
  lib.Register ("SetWorldScale",            make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::SetWorldScale)));
  lib.Register ("ResetScale",               make_invoker (&Node::ResetScale));
  lib.Register ("set_OrientationInherit",   make_invoker (&Node::SetOrientationInherit));
  lib.Register ("set_ScaleInherit",         make_invoker (&Node::SetScaleInherit));

  lib.Register ("get_Name",                  make_invoker (&Node::Name));
  lib.Register ("get_Position",              make_invoker (&Node::Position));
  lib.Register ("get_WorldPosition",         make_invoker (&Node::WorldPosition));
  lib.Register ("get_Orientation",           make_invoker (&Node::Orientation));
  lib.Register ("get_WorldOrientation",      make_invoker (&Node::WorldOrientation));
  lib.Register ("get_Scale",                 make_invoker (implicit_cast<const vec3f& (Node::*) () const> (&Node::Scale)));
  lib.Register ("get_WorldScale",            make_invoker (&Node::WorldScale));
  lib.Register ("get_OrientationInherit",    make_invoker (&Node::IsOrientationInherited));
  lib.Register ("get_ScaleInherit",          make_invoker (&Node::IsScaleInherited));
  lib.Register ("get_LocalTM",               make_invoker (&Node::LocalTM));
  lib.Register ("get_WorldTM",               make_invoker (&Node::WorldTM));
  lib.Register ("get_ParentTM",              make_invoker (&Node::ParentTM));
  lib.Register ("get_IsInUpdateTransaction", make_invoker (&Node::IsInUpdateTransaction));
  lib.Register ("get_Parent",                make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::Parent)));
  lib.Register ("get_FirstChild",            make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::FirstChild)));
  lib.Register ("get_LastChild",             make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::LastChild)));
  lib.Register ("get_PrevChild",             make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::PrevChild)));
  lib.Register ("get_NextChild",             make_invoker (implicit_cast<Node::Pointer (Node::*) ()> (&Node::NextChild)));

  lib.Register ("BindToParent", make_invoker (make_invoker (&Node::BindToParent),
    make_invoker<void (Node&, Node&, NodeBindMode)> (xtl::bind (&Node::BindToParent, _1, _2, _3, NodeTransformSpace_Local)),
    make_invoker<void (Node&, Node&)> (xtl::bind (&Node::BindToParent, _1, _2, NodeBindMode_Default, NodeTransformSpace_Local))));

  lib.Register ("BindToScene", make_invoker (make_invoker (&Node::BindToScene),
    make_invoker<void (Node&, Scene&, NodeBindMode)> (xtl::bind (&Node::BindToScene, _1, _2, _3, NodeTransformSpace_Local)),
    make_invoker<void (Node&, Scene&)> (xtl::bind (&Node::BindToScene, _1, _2, NodeBindMode_Default, NodeTransformSpace_Local))));

  lib.Register ("Unbind", make_invoker (make_invoker (&Node::Unbind),
                          make_invoker<void (Node&)> (bind (&Node::Unbind, _1, NodeTransformSpace_Local))));

  lib.Register ("UnbindChild", make_invoker (make_invoker (implicit_cast<void (Node::*) (const char*, NodeTransformSpace)> (&Node::UnbindChild)),
                                             make_invoker (implicit_cast<void (Node::*) (const char*, NodeSearchMode, NodeTransformSpace)> (&Node::UnbindChild)),
                                             make_invoker<void (Node&, const char*)> (xtl::bind (implicit_cast<void (Node::*) (const char*, NodeTransformSpace)> (&Node::UnbindChild), _1, _2, NodeTransformSpace_Local)),
                                             make_invoker<void (Node&, const char*, NodeSearchMode)> (xtl::bind (implicit_cast<void (Node::*) (const char*, NodeSearchMode, NodeTransformSpace)> (&Node::UnbindChild), _1, _2, _3, NodeTransformSpace_Local))));
  lib.Register ("UnbindAllChildren", make_invoker (&Node::UnbindAllChildren));

  lib.Register ("FindChild", make_invoker (make_invoker<Node::Pointer (Node&, const char*)> (xtl::bind (implicit_cast<Node::Pointer (Node::*) (const char*, NodeSearchMode)> (&Node::FindChild), _1, _2, NodeSearchMode_Default)),
                                           make_invoker (implicit_cast<Node::Pointer (Node::*) (const char*, NodeSearchMode)> (&Node::FindChild))));

  lib.Register ("ObjectTM", make_invoker (&Node::ObjectTM));

  lib.Register ("Ort", make_invoker (&Node::Ort));

  lib.Register ("get_LocalOrtX", make_invoker (&Node::LocalOrtX));
  lib.Register ("get_LocalOrtY", make_invoker (&Node::LocalOrtY));
  lib.Register ("get_LocalOrtZ", make_invoker (&Node::LocalOrtZ));
  lib.Register ("get_WorldOrtX", make_invoker (&Node::WorldOrtX));
  lib.Register ("get_WorldOrtY", make_invoker (&Node::WorldOrtY));
  lib.Register ("get_WorldOrtZ", make_invoker (&Node::WorldOrtZ));
  lib.Register ("get_ParentOrtX", make_invoker (&Node::ParentOrtX));
  lib.Register ("get_ParentOrtY", make_invoker (&Node::ParentOrtY));
  lib.Register ("get_ParentOrtZ", make_invoker (&Node::ParentOrtZ));

  lib.Register ("Translate", make_invoker (make_invoker (implicit_cast<void (Node::*) (const vec3f&, NodeTransformSpace)> (&Node::Translate)),
                 make_invoker (implicit_cast<void (Node::*) (float, float, float, NodeTransformSpace)> (&Node::Translate)),
                 make_invoker<void (Node&, const vec3f&)> (xtl::bind (implicit_cast<void (Node::*) (const vec3f&, NodeTransformSpace)> (&Node::Translate), _1, _2, NodeTransformSpace_Parent)),
                 make_invoker<void (Node&, float, float, float)> (xtl::bind (implicit_cast<void (Node::*) (float, float, float, NodeTransformSpace)> (&Node::Translate), _1, _2, _3, _4, NodeTransformSpace_Parent))));
  lib.Register ("Rotate", make_invoker (make_invoker (implicit_cast<void (Node::*) (const quatf&, NodeTransformSpace)> (&Node::Rotate)),
                 make_invoker<void (Node*, math::anglef, math::anglef, math::anglef, NodeTransformSpace)> (implicit_cast<void (Node::*) (const math::anglef&, const math::anglef&, const math::anglef&, NodeTransformSpace)> (&Node::Rotate)),
                 make_invoker<void (Node*, math::anglef, float, float, float, NodeTransformSpace)> (implicit_cast<void (Node::*) (const math::anglef&, float, float, float, NodeTransformSpace)> (&Node::Rotate)),
                 make_invoker<void (Node&, const quatf&)> (xtl::bind (implicit_cast<void (Node::*) (const quatf&, NodeTransformSpace)> (&Node::Rotate), _1, _2, NodeTransformSpace_Parent)),
                 make_invoker<void (Node&, math::anglef, math::anglef, math::anglef)> (xtl::bind (implicit_cast<void (Node::*) (const math::anglef&, const math::anglef&, const math::anglef&, NodeTransformSpace)> (&Node::Rotate), _1, _2, _3, _4, NodeTransformSpace_Parent)),
                 make_invoker<void (Node&, math::anglef, float, float, float)> (xtl::bind (implicit_cast<void (Node::*) (const math::anglef&, float, float, float, NodeTransformSpace)> (&Node::Rotate), _1, _2, _3, _4, _5, NodeTransformSpace_Parent))));
  lib.Register ("Rescale", make_invoker (make_invoker (implicit_cast<void (Node::*) (const vec3f&)> (&Node::Scale)),
                                         make_invoker (implicit_cast<void (Node::*) (float, float, float)> (&Node::Scale))));

  lib.Register ("LookTo", make_invoker (
    make_invoker (implicit_cast<void (Node::*)(const vec3f&, NodeOrt, NodeOrt, NodeTransformSpace)> (&Node::LookTo)),
    make_invoker<void (Node&, const vec3f&, NodeOrt, NodeOrt)> (bind (implicit_cast<void (Node::*)(const vec3f&, NodeOrt, NodeOrt, NodeTransformSpace)> (&Node::LookTo), _1, _2, _3, _4, NodeTransformSpace_Local)),
    make_invoker (implicit_cast<void (Node::*)(const vec3f&, const vec3f&, NodeTransformSpace)> (&Node::LookTo)),
    make_invoker<void (Node&, const vec3f&, const vec3f&)> (bind (implicit_cast<void (Node::*)(const vec3f&, const vec3f&, NodeTransformSpace)> (&Node::LookTo), _1, _2, _3, NodeTransformSpace_Local)),
    make_invoker (implicit_cast<void (Node::*)(const vec3f&, NodeTransformSpace)> (&Node::LookTo)),
    make_invoker<void (Node&, const vec3f&)> (bind (implicit_cast<void (Node::*)(const vec3f&, NodeTransformSpace)> (&Node::LookTo), _1, _2, NodeTransformSpace_Local))
  ));

  lib.Register ("BeginUpdate", make_invoker (&Node::BeginUpdate));
  lib.Register ("EndUpdate",   make_invoker (&Node::EndUpdate));

  lib.Register ("CreateEventHandler",          make_callback_invoker<Node::EventHandler::signature_type> ());
  lib.Register ("CreateSubTreeEventHandler",   make_callback_invoker<Node::SubTreeEventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler",  make_invoker (xtl::implicit_cast<xtl::connection (Node::*)(NodeEvent, const Node::EventHandler&) const> (&Node::RegisterEventHandler)));
  lib.Register ("RegisterSubTreeEventHandler", make_invoker (xtl::implicit_cast<xtl::connection (Node::*)(NodeSubTreeEvent, const Node::SubTreeEventHandler&) const> (&Node::RegisterEventHandler)));

    //регистрация типов данных

  environment.RegisterType<Node> (SCENE_NODE_LIBRARY);

  return lib;
}

/*
   Регистрация библиотеки работы со свойствами узла
*/

template <class Ret> struct result_value
{
  Ret value;
  
  result_value () {}
};

template <class Ret> Ret get_node_property (NodeProperties& properties, const char* name)
{
  result_value<Ret> result;
  
  properties.GetProperty (name, result.value);
  
  return result.value;
}

void bind_node_properties_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_NODE_PROPERTIES_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&NodeProperties::Create));
  
    //регистрация методов
    
  lib.Register ("get_Size",        make_invoker (&NodeProperties::Size));
  lib.Register ("SetPropertyName", make_invoker (implicit_cast<void (NodeProperties::*)(const char*, const char*)> (&NodeProperties::SetPropertyName)));
  lib.Register ("GetPropertyName", make_invoker (&NodeProperties::PropertyName));
  lib.Register ("Remove",          make_invoker (implicit_cast<void (NodeProperties::*)(const char*)> (&NodeProperties::Remove)));
  lib.Register ("Clear",           make_invoker (&NodeProperties::Clear));  
  lib.Register ("Clone",           make_invoker (&NodeProperties::Clone));
  lib.Register ("GetPropertyType", make_invoker (
    make_invoker (implicit_cast<NodePropertyType (NodeProperties::*)(const char*) const> (&NodeProperties::PropertyType)),
    make_invoker (implicit_cast<NodePropertyType (NodeProperties::*)(size_t) const> (&NodeProperties::PropertyType))
  ));  
  lib.Register ("SetPropertyType", make_invoker (implicit_cast<void (NodeProperties::*)(const char*, NodePropertyType)> (&NodeProperties::SetPropertyType)));  
  lib.Register ("IsPresent",       make_invoker (&NodeProperties::IsPresent));
  lib.Register ("GetString",       make_invoker (&get_node_property<stl::string>));
  lib.Register ("GetInteger",      make_invoker (&get_node_property<int>));
  lib.Register ("GetFloat",        make_invoker (&get_node_property<float>));
  lib.Register ("GetVector",       make_invoker (&get_node_property<math::vec4f>));
  lib.Register ("GetMatrix",       make_invoker (&get_node_property<math::mat4f>));  
  lib.Register ("SetString",       make_invoker (implicit_cast<void (NodeProperties::*)(const char*, const char*)> (&NodeProperties::SetProperty)));
  lib.Register ("SetInteger",      make_invoker (implicit_cast<void (NodeProperties::*)(const char*, int)> (&NodeProperties::SetProperty)));
  lib.Register ("SetFloat",        make_invoker (implicit_cast<void (NodeProperties::*)(const char*, float)> (&NodeProperties::SetProperty)));
  lib.Register ("SetVector",       make_invoker (implicit_cast<void (NodeProperties::*)(const char*, const math::vec4f&)> (&NodeProperties::SetProperty)));
  lib.Register ("SetMatrix",       make_invoker (implicit_cast<void (NodeProperties::*)(const char*, const math::mat4f&)> (&NodeProperties::SetProperty)));

    //регистрация типов данных

  environment.RegisterType<NodeProperties> (SCENE_NODE_PROPERTIES_LIBRARY);
}

/*
   Регистрация библиотеки работы с массивами узлов сцены
*/

void bind_node_array_link_mode_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_STATIC_NODE_ARRAY_LINK_MODE_LIBRARY);

    //регистрация операций

  lib.Register ("get_AddRef",  make_const (NodeArrayLinkMode_AddRef));
  lib.Register ("get_WeakRef", make_const (NodeArrayLinkMode_WeakRef));
}

NodeArray create_node_array (NodeArrayLinkMode link_mode)
{
  return NodeArray (link_mode);
}

Node::Pointer get_node_array_item (NodeArray& array, size_t index)
{
  return &array.Item (index);
}

void bind_node_array_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_NODE_ARRAY_LIBRARY);

    //регистрация статических библиотек

  bind_node_array_link_mode_library (environment);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (make_invoker (&create_node_array),
                                        make_invoker<NodeArray ()> (xtl::bind (&create_node_array, NodeArrayLinkMode_AddRef))));

    //регистрация операций

  lib.Register ("get_Size",     make_invoker (&NodeArray::Size));
  lib.Register ("get_Empty",    make_invoker (&NodeArray::IsEmpty));
  lib.Register ("get_Capacity", make_invoker (&NodeArray::Capacity));
  lib.Register ("get_LinkMode", make_invoker (&NodeArray::LinkMode));
  lib.Register ("Item",         make_invoker (&get_node_array_item));
  lib.Register ("Add",          make_invoker (&NodeArray::Add));
  lib.Register ("Remove",       make_invoker (make_invoker ((void (NodeArray::*)(size_t))&NodeArray::Remove),
                                              make_invoker ((void (NodeArray::*)(Node&))&NodeArray::Remove)));
  lib.Register ("Clear",        make_invoker (&NodeArray::Clear));
  lib.Register ("Reserve",      make_invoker (&NodeArray::Reserve));

    //регистрация типов данных

  environment.RegisterType<NodeArray> (SCENE_NODE_ARRAY_LIBRARY);
}

/*
   Регистрация библиотеки работы с объектами сцены
*/

InvokerRegistry& bind_entity_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_ENTITY_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_NODE_LIBRARY);

    //регистрация операций

  lib.Register ("set_WireColor",  make_invoker (implicit_cast<void (Entity::*) (const vec3f&)> (&Entity::SetWireColor)));
  lib.Register ("get_WireColor",  make_invoker (&Entity::WireColor));

  lib.Register ("SetWireColor", make_invoker (implicit_cast<void (Entity::*) (float, float, float)> (&Entity::SetWireColor)));

  lib.Register ("get_BoundBox",      make_invoker (&Entity::BoundBox));
  lib.Register ("get_WorldBoundBox", make_invoker (&Entity::WorldBoundBox));

  lib.Register ("get_InfiniteBounds", make_invoker (&Entity::IsInfiniteBounds));

  lib.Register ("get_ChildrenBoundBox",      make_invoker (&Entity::ChildrenBoundBox));
  lib.Register ("get_FullBoundBox",          make_invoker (&Entity::FullBoundBox));
  lib.Register ("get_WorldChildrenBoundBox", make_invoker (&Entity::WorldChildrenBoundBox));
  lib.Register ("get_WorldFullBoundBox",     make_invoker (&Entity::WorldFullBoundBox));
  lib.Register ("GetIntersections",          make_invoker (make_invoker ((void (Entity::*)(NodeArray&) const)&Entity::GetIntersections),
                                                           make_invoker ((NodeArray (Entity::*)() const)&Entity::GetIntersections)));

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

void bind_perspective_camera_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_PERSPECTIVE_CAMERA_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_perspective_camera));

    //регистрация операций

  lib.Register ("set_FovX",  make_invoker<void (PerspectiveCamera&, math::anglef)> (&PerspectiveCamera::SetFovX));
  lib.Register ("set_FovY",  make_invoker<void (PerspectiveCamera&, math::anglef)> (&PerspectiveCamera::SetFovY));
  lib.Register ("set_ZNear", make_invoker (&PerspectiveCamera::SetZNear));
  lib.Register ("set_ZFar",  make_invoker (&PerspectiveCamera::SetZFar));
  lib.Register ("get_FovX",  make_invoker<math::anglef (PerspectiveCamera&)> (&PerspectiveCamera::FovX));
  lib.Register ("get_FovY",  make_invoker<math::anglef (PerspectiveCamera&)> (&PerspectiveCamera::FovY));
  lib.Register ("get_ZNear", make_invoker (&PerspectiveCamera::ZNear));
  lib.Register ("get_ZFar",  make_invoker (&PerspectiveCamera::ZFar));

    //регистрация типов данных

  environment.RegisterType<PerspectiveCamera> (SCENE_PERSPECTIVE_CAMERA_LIBRARY);
}

void bind_ortho_camera_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_ORTHO_CAMERA_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

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

void bind_direct_light_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_DIRECT_LIGHT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_LIGHT_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_direct_light));

    //регистрация операций

  lib.Register ("set_Radius", make_invoker (&DirectLight::SetRadius));
  lib.Register ("get_Radius", make_invoker (&DirectLight::Radius));

    //регистрация типов данных

  environment.RegisterType<DirectLight> (SCENE_DIRECT_LIGHT_LIBRARY);
}

void bind_spot_light_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SPOT_LIGHT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_LIGHT_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_spot_light));

    //регистрация операций

  lib.Register ("set_Angle",    make_invoker<void (SpotLight&, math::anglef)> (&SpotLight::SetAngle));
  lib.Register ("set_Exponent", make_invoker (&SpotLight::SetExponent));
  lib.Register ("get_Angle",    make_invoker<math::anglef (SpotLight&)> (&SpotLight::Angle));
  lib.Register ("get_Exponent", make_invoker (&SpotLight::Exponent));

    //регистрация типов данных

  environment.RegisterType<SpotLight> (SCENE_SPOT_LIGHT_LIBRARY);
}

void bind_point_light_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_POINT_LIGHT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_LIGHT_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_point_light));

    //регистрация типов данных

  environment.RegisterType<PointLight> (SCENE_POINT_LIGHT_LIBRARY);
}

void bind_light_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_LIGHT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация операций

  lib.Register ("set_LightColor",  make_invoker (&Light::SetLightColor));
  lib.Register ("set_Attenuation", make_invoker (&Light::SetAttenuation));
  lib.Register ("set_Range",       make_invoker (&Light::SetRange));
  lib.Register ("get_LightColor",  make_invoker (&Light::LightColor));
  lib.Register ("get_Attenuation", make_invoker (&Light::Attenuation));
  lib.Register ("get_Range",       make_invoker (&Light::Range));

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

void set_bound_box (helpers::Box& entity, const bound_volumes::aaboxf& box)
{
  entity.SetBoundBox (box);
}

void bind_box_helper_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_BOX_HELPER_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_box_helper));

    //регистрация операций

  lib.Register ("set_BoundBox", make_invoker (&set_bound_box));

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

void bind_listener_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_LISTENER_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

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

void bind_sound_emitter_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SOUND_EMITTER_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_sound_emitter));

    //регистрация операций

  lib.Register ("set_Gain", make_invoker (&SoundEmitter::SetGain));
  lib.Register ("get_Gain", make_invoker (&SoundEmitter::Gain));

  lib.Register ("get_SoundDeclarationName", make_invoker (&SoundEmitter::SoundDeclarationName));

  lib.Register ("Play", make_invoker (&SoundEmitter::Play));
  lib.Register ("Stop", make_invoker (&SoundEmitter::Stop));

    //регистрация типов данных

  environment.RegisterType<SoundEmitter> (SCENE_SOUND_EMITTER_LIBRARY);
}

/*
    Создание спрайта
*/

Sprite::Pointer create_sprite ()
{
  return Sprite::Create ();
}

SpriteList::Pointer create_sprite_list ()
{
  return SpriteList::Create ();
}

/*
     Регистрация библиотеки работы с спрайтовыми моделями
*/

void bind_sprite_model_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SPRITE_MODEL_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация операций

  lib.Register ("set_Material",       make_invoker (&SpriteModel::SetMaterial));
  lib.Register ("get_Material",       make_invoker (&SpriteModel::Material));
  lib.Register ("set_AlphaReference", make_invoker (&SpriteModel::SetAlphaReference));
  lib.Register ("get_AlphaReference", make_invoker (&SpriteModel::AlphaReference));

    //регистрация типов данных

  environment.RegisterType<SpriteModel> (SCENE_SPRITE_MODEL_LIBRARY);
}

/*
     Регистрация библиотеки работы с спрайтами
*/

void bind_sprite_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SPRITE_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_SPRITE_MODEL_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_sprite));

    //регистрация операций

  lib.Register ("set_Color",    make_invoker (implicit_cast<void (Sprite::*) (const vec4f&)> (&Sprite::SetColor)));
  lib.Register ("set_Alpha",    make_invoker (&Sprite::SetAlpha));
  lib.Register ("set_Frame",    make_invoker (&Sprite::SetFrame));
  lib.Register ("get_Color",    make_invoker (&Sprite::Color));
  lib.Register ("get_Alpha",    make_invoker (&Sprite::Alpha));
  lib.Register ("get_Frame",    make_invoker <int (Sprite&)> (&Sprite::Frame));

  lib.Register ("SetColor", make_invoker (make_invoker (implicit_cast<void (Sprite::*) (float, float, float, float)> (&Sprite::SetColor)),
                                          make_invoker (implicit_cast<void (Sprite::*) (float, float, float)>        (&Sprite::SetColor))));

    //регистрация типов данных

  environment.RegisterType<Sprite> (SCENE_SPRITE_LIBRARY);
}

/*
    Создание линии текста
*/

TextLine::Pointer create_text_line ()
{
  return TextLine::Create ();
}

/*
   Регистрация библиотеки работы с текстом
*/

void bind_static_text_line_library (Environment& environment)
{
  InvokerRegistry& text_line_alignment_lib = environment.CreateLibrary (SCENE_STATIC_TEXT_LINE_ALIGNMENT_LIBRARY);

  text_line_alignment_lib.Register ("get_Center",   make_const (TextLineAlignment_Center));
  text_line_alignment_lib.Register ("get_Left",     make_const (TextLineAlignment_Left));
  text_line_alignment_lib.Register ("get_Right",    make_const (TextLineAlignment_Right));
  text_line_alignment_lib.Register ("get_Top",      make_const (TextLineAlignment_Top));
  text_line_alignment_lib.Register ("get_Bottom",   make_const (TextLineAlignment_Bottom));
  text_line_alignment_lib.Register ("get_BaseLine", make_const (TextLineAlignment_BaseLine));
}

void bind_text_line_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_TEXT_LINE_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация статических переменных

  bind_static_text_line_library (environment);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_text_line));

    //регистрация операций

  lib.Register ("set_Text",                make_invoker (implicit_cast<void (TextLine::*) (const char*)> (&TextLine::SetText)));
  lib.Register ("get_Text",                make_invoker (&TextLine::Text));
  lib.Register ("get_TextLength",          make_invoker (&TextLine::TextLength));
  lib.Register ("set_TextUnicode",         make_invoker (implicit_cast<void (TextLine::*) (const wchar_t*)> (&TextLine::SetText)));
  lib.Register ("get_TextUnicode",         make_invoker (&TextLine::TextUnicode));
  lib.Register ("set_Font",                make_invoker (&TextLine::SetFont));
  lib.Register ("get_Font",                make_invoker (&TextLine::Font));
  lib.Register ("set_Color",               make_invoker (implicit_cast<void (TextLine::*) (const vec4f&)> (&TextLine::SetColor)));
  lib.Register ("get_Color",               make_invoker (&TextLine::Color));
  lib.Register ("set_HorizontalAlignment", make_invoker (&TextLine::SetHorizontalAlignment));
  lib.Register ("get_HorizontalAlignment", make_invoker (&TextLine::HorizontalAlignment));
  lib.Register ("set_VerticalAlignment",   make_invoker (&TextLine::SetVerticalAlignment));
  lib.Register ("get_VerticalAlignment",   make_invoker (&TextLine::VerticalAlignment));

  lib.Register ("SetColor", make_invoker (make_invoker (implicit_cast<void (TextLine::*) (float, float, float, float)> (&TextLine::SetColor)),
                                          make_invoker (implicit_cast<void (TextLine::*) (float, float, float)>        (&TextLine::SetColor))));
  lib.Register ("SetCharsColorFactors", make_invoker (&TextLine::SetCharsColorFactors));
  lib.Register ("CharColor", make_invoker (&TextLine::CharColor));

  lib.Register ("SetAlignment", make_invoker (&TextLine::SetAlignment));

  environment.RegisterType<TextLine> (SCENE_TEXT_LINE_LIBRARY);
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

void bind_visual_model_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_VISUAL_MODEL_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_visual_model));

    //регистрация операций

  lib.Register ("set_MeshName", make_invoker (&VisualModel::SetMeshName));
  lib.Register ("get_MeshName", make_invoker (&VisualModel::MeshName));

  environment.RegisterType<VisualModel> (SCENE_VISUAL_MODEL_LIBRARY);
}

/*
    Создание карты высот
*/

HeightMap::Pointer create_height_map ()
{
  return HeightMap::Create ();
}

void set_height_map_cell_height (HeightMap& map, size_t row, size_t column, float height)
{
  map.Vertex (row, column).height = height;
}

float get_height_map_cell_height (HeightMap& map, size_t row, size_t column)
{
  return map.Vertex (row, column).height;
}

void set_height_map_cell_normal (HeightMap& map, size_t row, size_t column, const math::vec3f& normal)
{
  map.Vertex (row, column).normal = normal;
}

const math::vec3f& get_height_map_cell_normal (HeightMap& map, size_t row, size_t column)
{
  return map.Vertex (row, column).normal;
}

void set_height_map_cell_color (HeightMap& map, size_t row, size_t column, const math::vec4f& color)
{
  map.Vertex (row, column).color = color;
}

math::vec4f& get_height_map_cell_color (HeightMap& map, size_t row, size_t column)
{
  return map.Vertex (row, column).color;
}

/*
   Регистрация библиотеки работы с картами высот
*/

void bind_height_map_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_HEIGHT_MAP_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_height_map));

    //регистрация операций

  lib.Register ("set_RowsCount",    make_invoker (&HeightMap::SetRowsCount));
  lib.Register ("set_ColumnsCount", make_invoker (&HeightMap::SetColumnsCount));
  lib.Register ("get_RowsCount",    make_invoker (&HeightMap::RowsCount));
  lib.Register ("get_ColumnsCount", make_invoker (&HeightMap::ColumnsCount));

  lib.Register ("SetCellsCount",     make_invoker (&HeightMap::SetCellsCount));
  lib.Register ("SetVerticesHeight", make_invoker (&HeightMap::SetVerticesHeight));
  lib.Register ("SetVerticesNormal", make_invoker (&HeightMap::SetVerticesNormal));
  lib.Register ("SetVerticesColor",  make_invoker (&HeightMap::SetVerticesColor));

  lib.Register ("SetVertexHeight", make_invoker (&set_height_map_cell_height));
  lib.Register ("GetVertexHeight", make_invoker (&get_height_map_cell_height));
  lib.Register ("SetVertexNormal", make_invoker (&set_height_map_cell_normal));
  lib.Register ("GetVertexNormal", make_invoker (&get_height_map_cell_normal));
  lib.Register ("SetVertexColor",  make_invoker (&set_height_map_cell_color));
  lib.Register ("GetVertexColor",  make_invoker (&get_height_map_cell_color));

  environment.RegisterType<HeightMap> (SCENE_HEIGHT_MAP_LIBRARY);
}

/*
    Регистрация библиотеки работы со сценой
*/

void bind_scene_graph_library (Environment& environment)
{
  bind_scene_library              (environment);
  bind_node_library               (environment);
  bind_node_properties_library    (environment);
  bind_node_array_library         (environment);
  bind_entity_library             (environment);
  bind_perspective_camera_library (environment);
  bind_ortho_camera_library       (environment);
  bind_light_library              (environment);
  bind_direct_light_library       (environment);
  bind_spot_light_library         (environment);
  bind_point_light_library        (environment);
  bind_box_helper_library         (environment);
  bind_listener_library           (environment);
  bind_sound_emitter_library      (environment);
  bind_sprite_model_library       (environment);
  bind_sprite_library             (environment);
  bind_text_line_library          (environment);
  bind_visual_model_library       (environment);
  bind_height_map_library         (environment);
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_scene_graph_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> SceneGraphScriptBind (COMPONENT_NAME);

}

}
