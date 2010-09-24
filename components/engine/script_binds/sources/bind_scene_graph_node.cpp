#include "scene_graph.h"

namespace engine
{

namespace scene_graph_script_binds
{

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

void bind_node_library (Environment& environment)
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

  lib.Register ("UnbindChild", make_invoker (make_invoker (implicit_cast<void (Node::*) (const char*, NodeSearchMode, NodeTransformSpace)> (&Node::UnbindChild)),
                                             make_invoker<void (Node&, const char*, NodeSearchMode)> (xtl::bind (implicit_cast<void (Node::*) (const char*, NodeSearchMode, NodeTransformSpace)> (&Node::UnbindChild), _1, _2, _3, NodeTransformSpace_Local)),
                                             make_invoker<void (Node&, const char*)> (xtl::bind (implicit_cast<void (Node::*) (const char*, NodeTransformSpace)> (&Node::UnbindChild), _1, _2, NodeTransformSpace_Local))));
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
  
  lib.Register ("AttachController",     make_invoker (&Node::AttachController));
  lib.Register ("DetachAllControllers", make_invoker (&Node::DetachAllControllers));
  lib.Register ("Update",               make_invoker (make_invoker (&Node::Update), make_invoker<void (Node&, float)> (xtl::bind (&Node::Update, _1, _2, NodeTraverseMode_Default))));

  lib.Register ("CreateEventHandler",          make_callback_invoker<Node::EventHandler::signature_type> ());
  lib.Register ("CreateSubTreeEventHandler",   make_callback_invoker<Node::SubTreeEventHandler::signature_type> ());
  lib.Register ("RegisterEventHandler",  make_invoker (xtl::implicit_cast<xtl::connection (Node::*)(NodeEvent, const Node::EventHandler&) const> (&Node::RegisterEventHandler)));
  lib.Register ("RegisterSubTreeEventHandler", make_invoker (xtl::implicit_cast<xtl::connection (Node::*)(NodeSubTreeEvent, const Node::SubTreeEventHandler&) const> (&Node::RegisterEventHandler)));

    //регистрация типов данных

  environment.RegisterType<Node> (SCENE_NODE_LIBRARY);
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
    Регистрация библиотеки работы с контроллером
*/

void bind_controller_owner_mode_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_CONTROLLER_OWNER_MODE_LIBRARY);

  lib.Register ("get_None",               make_const (ControllerOwnerMode_None));
  lib.Register ("get_ControllerOwnsNode", make_const (ControllerOwnerMode_ControllerOwnsNode));  
  lib.Register ("get_NodeOwnsController", make_const (ControllerOwnerMode_NodeOwnsController));
}

void bind_controller_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_CONTROLLER_LIBRARY);

    //регистрация операций

  lib.Register ("get_AttachedNode",   make_invoker ((Node* (Controller::*)())&Controller::AttachedNode));
  lib.Register ("get_OwnerMode",      make_invoker (&Controller::OwnerMode));
  lib.Register ("set_OwnerMode",      make_invoker (&Controller::SetOwnerMode));
  lib.Register ("ControllerOwnsNode", make_invoker (&Controller::ControllerOwnsNode));
  lib.Register ("NodeOwnsController", make_invoker (&Controller::NodeOwnsController));
  lib.Register ("Detach",             make_invoker (&Controller::Detach));

    //регистрация типов данных

  environment.RegisterType<Controller> (SCENE_CONTROLLER_LIBRARY);
}

}

}
