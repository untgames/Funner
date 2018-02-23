#include "scene_graph.h"

using namespace scene_graph;

namespace
{

const char* SCENE_CONTROLLER_ALIGN_WITH_NODE_LIBRARY                         = "Scene.Controllers.AlignWithNode";
const char* SCENE_CONTROLLER_LOOK_TO_NODE_POINT_LIBRARY                      = "Scene.Controllers.LookToNodePoint";
const char* SCENE_CONTROLLER_MOVE_TO_NODE_POINT_LIBRARY                      = "Scene.Controllers.MoveToNodePoint";
const char* SCENE_CONTROLLER_SPINE_ANIMATION_LIBRARY                         = "Scene.Controllers.SpineAnimation";
const char* SCENE_CONTROLLER_SPINE_SKELETON_VISUAL_STRUCTURE_BUILDER_LIBRARY = "Scene.Controllers.SpineSkeletonVisualStructureBuilder";
const char* SCENE_CONTROLLER_SYNC_PHYSICS_TO_NODE_LIBRARY                    = "Scene.Controllers.SyncPhysicsToNode";
const char* SCENE_CONTROLLER_WATER_LIBRARY                                   = "Scene.Controllers.Water";

}

template class engine::decl_sg_cast<controllers::AlignWithNode,                       Controller>;
template class engine::decl_sg_cast<controllers::LookToNodePoint,                     Controller>;
template class engine::decl_sg_cast<controllers::MoveToNodePoint,                     Controller>;
template class engine::decl_sg_cast<controllers::SpineAnimation,                      Controller>;
template class engine::decl_sg_cast<controllers::SpineSkeletonVisualStructureBuilder, Controller>;
template class engine::decl_sg_cast<controllers::SyncPhysicsToNode,                   Controller>;
template class engine::decl_sg_cast<controllers::Water,                               Controller>;

namespace engine
{

namespace scene_graph_script_binds
{

/*
    Регистрация библиотеки работы с контроллером воды
*/

void bind_controller_water_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_WATER_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&Water::Create));

    //регистрация операций
    
  lib.Register ("set_Viscosity", make_invoker (&Water::SetViscosity));
  lib.Register ("get_Viscosity", make_invoker (&Water::Viscosity));
  lib.Register ("PutStorm",      make_invoker (
                                   make_invoker (&Water::PutStorm), 
                                   make_invoker<void (Water&, const math::vec3f&, float)> (xtl::bind (&Water::PutStorm, _1, _2, _3, 0.05f)),
                                   make_invoker<void (Water&, const math::vec3f&)> (xtl::bind (&Water::PutStorm, _1, _2, 0.0005f, 0.05f))
  ));
  lib.Register ("PutWorldStorm",   make_invoker (
                                   make_invoker (&Water::PutWorldStorm), 
                                   make_invoker<void (Water&, const math::vec3f&, float)> (xtl::bind (&Water::PutWorldStorm, _1, _2, _3, 0.05f)),
                                   make_invoker<void (Water&, const math::vec3f&)> (xtl::bind (&Water::PutWorldStorm, _1, _2, 0.0005f, 0.05f))
  ));

    //регистрация типа данных

  environment.RegisterType<Water> (SCENE_CONTROLLER_WATER_LIBRARY);
}

/*
    Регистрация библиотеки работы с контроллером передвижения
*/

void bind_controller_move_to_node_point_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_MOVE_TO_NODE_POINT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&MoveToNodePoint::Create));

    //регистрация операций

  lib.Register ("set_AccelerationHandler", make_invoker<void (MoveToNodePoint::*) (const LinearAccelerationEvaluator&)> (&MoveToNodePoint::SetAccelerationHandler));
  lib.Register ("set_TransformSpace",      make_invoker (&MoveToNodePoint::SetTransformSpace));
  lib.Register ("get_TransformSpace",      make_invoker (&MoveToNodePoint::TransformSpace));
  lib.Register ("Start",                   make_invoker (&MoveToNodePoint::Start));
  lib.Register ("Stop",                    make_invoker (&MoveToNodePoint::Stop));

    //регистрация типа данных

  environment.RegisterType<MoveToNodePoint> (SCENE_CONTROLLER_MOVE_TO_NODE_POINT_LIBRARY);
}

/*
    Регистрация библиотеки работы с контроллером слежения
*/

void bind_controller_look_to_node_point_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_LOOK_TO_NODE_POINT_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&LookToNodePoint::Create));

    //регистрация операций

  lib.Register ("set_AccelerationHandler", make_invoker<void (LookToNodePoint::*) (const LinearAccelerationEvaluator&)> (&LookToNodePoint::SetAccelerationHandler));
  lib.Register ("Start",                   make_invoker (&LookToNodePoint::Start));
  lib.Register ("Stop",                    make_invoker (&LookToNodePoint::Stop));

    //регистрация типа данных

  environment.RegisterType<LookToNodePoint> (SCENE_CONTROLLER_LOOK_TO_NODE_POINT_LIBRARY);
}

/*
    Регистрация библиотеки работы с контроллером выравнивания
*/

void bind_controller_align_with_node_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_ALIGN_WITH_NODE_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&AlignWithNode::Create));

    //регистрация операций

  lib.Register ("set_AccelerationHandler", make_invoker<void (AlignWithNode::*) (const LinearAccelerationEvaluator&)> (&AlignWithNode::SetAccelerationHandler));
  lib.Register ("Start",                   make_invoker (&AlignWithNode::Start));
  lib.Register ("Stop",                    make_invoker (&AlignWithNode::Stop));

    //регистрация типа данных

  environment.RegisterType<AlignWithNode> (SCENE_CONTROLLER_ALIGN_WITH_NODE_LIBRARY);
}

void bind_controller_sync_physics_to_node_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_SYNC_PHYSICS_TO_NODE_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

  lib.Register ("Create",           make_invoker (&SyncPhysicsToNode::Create));
  lib.Register ("get_AttachedBody", make_invoker (&SyncPhysicsToNode::AttachedBody));

  environment.RegisterType<AlignWithNode> (SCENE_CONTROLLER_SYNC_PHYSICS_TO_NODE_LIBRARY);
}

void bind_controller_spine_animation_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_SPINE_ANIMATION_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

  lib.Register ("Create",             make_invoker (&SpineAnimation::Create));
  lib.Register ("get_AnimationState", make_invoker (&SpineAnimation::AnimationState));

  environment.RegisterType<AlignWithNode> (SCENE_CONTROLLER_SPINE_ANIMATION_LIBRARY);
}

void bind_controller_spine_skeleton_visual_structure_builder_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_CONTROLLER_SPINE_SKELETON_VISUAL_STRUCTURE_BUILDER_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_CONTROLLER_LIBRARY);

  lib.Register ("Create", make_invoker (&SpineSkeletonVisualStructureBuilder::Create));

  environment.RegisterType<AlignWithNode> (SCENE_CONTROLLER_SPINE_SKELETON_VISUAL_STRUCTURE_BUILDER_LIBRARY);
}

}

}
