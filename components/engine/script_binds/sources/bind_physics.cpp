#include "shared.h"

using namespace script;
using namespace physics;

namespace
{

/*
    Константы
*/

const char* MANAGER_LIBRARY = "Physics.Manager";
const char* SCENE_LIBRARY   = "Physics.Scene";
const char* COMPONENT_NAME  = "script.binds.Physics";
const char* BINDER_NAME     = "Physics";

void bind_manager_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (MANAGER_LIBRARY);
  
  lib.Register ("get_Description",    make_invoker (&PhysicsManager::Description));
  lib.Register ("CreateScene",        make_invoker (&PhysicsManager::CreateScene));
  lib.Register ("CreateMaterial",     make_invoker (
      make_invoker (xtl::implicit_cast<Material (PhysicsManager::*)(const char*)> (&PhysicsManager::CreateMaterial)),
      make_invoker (xtl::implicit_cast<Material (PhysicsManager::*)()> (&PhysicsManager::CreateMaterial))
  ));
  lib.Register ("CreateBoxShape",     make_invoker (&PhysicsManager::CreateBoxShape));
  lib.Register ("CreateSphereShape",  make_invoker (&PhysicsManager::CreateSphereShape));
  lib.Register ("CreateCapsuleShape", make_invoker (&PhysicsManager::CreateCapsuleShape));
  lib.Register ("CreatePlaneShape",   make_invoker (&PhysicsManager::CreatePlaneShape));
  lib.Register ("CreateShape",        make_invoker (
      make_invoker (xtl::implicit_cast<Shape (PhysicsManager::*)(const char*, const math::vec3f&)> (&PhysicsManager::CreateShape)),
      make_invoker<Shape (PhysicsManager&, const char*)> (xtl::bind (xtl::implicit_cast<Shape (PhysicsManager::*)(const char*, const math::vec3f&)> (&PhysicsManager::CreateShape), _1, _2, math::vec3f (1.f)))
  ));

  environment.RegisterType<PhysicsManager> (MANAGER_LIBRARY);
}

void bind_scene_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SCENE_LIBRARY);

  lib.Register ("get_Id",               make_invoker (&Scene::Id));
  lib.Register ("get_Gravity",          make_invoker (&Scene::Gravity));
  lib.Register ("get_SimulationStep",   make_invoker (&Scene::SimulationStep));
  lib.Register ("set_Gravity",          make_invoker (&Scene::SetGravity));
  lib.Register ("set_SimulationStep",   make_invoker (&Scene::SimulationStep));
  lib.Register ("CreateRigidBody",      make_invoker (
      make_invoker (xtl::implicit_cast<RigidBody (Scene::*)(const char*, const math::vec3f&)> (&Scene::CreateRigidBody)),
      make_invoker (xtl::implicit_cast<RigidBody (Scene::*)(const Shape&, float)> (&Scene::CreateRigidBody)),
      make_invoker<RigidBody (Scene&, const char*)> (xtl::bind (xtl::implicit_cast<RigidBody (Scene::*)(const char*, const math::vec3f&)> (&Scene::CreateRigidBody), _1, _2, math::vec3f (1.f)))
  ));
  lib.Register ("PerformSimulation",    make_invoker (&Scene::PerformSimulation));

  environment.RegisterType<Scene> (SCENE_LIBRARY);
}

void bind_physics_library (Environment& environment)
{
  bind_manager_library (environment);
  bind_scene_library (environment);
}

}

namespace components
{

namespace physics_script_binds
{

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
      bind_physics_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> PhysicsScriptBind (COMPONENT_NAME);

}

}

}
