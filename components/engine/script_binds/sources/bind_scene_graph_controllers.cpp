#include "scene_graph.h"

namespace
{

const char* SCENE_CONTROLLER_ALIGN_WITH_NODE_LIBRARY    = "Scene.Controllers.AlignWithNode";
const char* SCENE_CONTROLLER_LOOK_TO_NODE_POINT_LIBRARY = "Scene.Controllers.LookToNodePoint";
const char* SCENE_CONTROLLER_MOVE_TO_NODE_POINT_LIBRARY = "Scene.Controllers.MoveToNodePoint";
const char* SCENE_CONTROLLER_WATER_LIBRARY              = "Scene.Controllers.Water";

}

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
  lib.Register ("Start",                   make_invoker<void (MoveToNodePoint::*) (Node::Pointer node, const math::vec3f&)> (&MoveToNodePoint::Start));
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
  lib.Register ("Start",                   make_invoker<void (LookToNodePoint::*) (Node::Pointer node, const math::vec3f&, NodeOrt, NodeOrt)> (&LookToNodePoint::Start));
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
  lib.Register ("Start",                   make_invoker<void (AlignWithNode::*) (Node::Pointer node, NodeOrt, NodeOrt, NodeOrt)> (&AlignWithNode::Start));
  lib.Register ("Stop",                    make_invoker (&AlignWithNode::Stop));

    //регистрация типа данных

  environment.RegisterType<AlignWithNode> (SCENE_CONTROLLER_ALIGN_WITH_NODE_LIBRARY);
}

}

}
