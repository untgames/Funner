#include "shared.h"

using namespace script;
using namespace physics;

namespace
{

/*
    Константы
*/

const char* COLLISION_EVENT_TYPE_LIBRARY = "Physics.CollisionEventType";
const char* JOINT_BIND_LIBRARY           = "Physics.JointBind";
const char* JOINT_LIBRARY                = "Physics.Joint";
const char* MANAGER_LIBRARY              = "Physics.Manager";
const char* MATERIAL_LIBRARY             = "Physics.Material";
const char* SCENE_LIBRARY                = "Physics.Scene";
const char* SHAPE_LIBRARY                = "Physics.Shape";
const char* COMPONENT_NAME               = "script.binds.Physics";
const char* BINDER_NAME                  = "Physics";

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
  lib.Register ("set_SimulationStep",   make_invoker (&Scene::SetSimulationStep));
  lib.Register ("CreateRigidBody",      make_invoker (
      make_invoker (xtl::implicit_cast<RigidBody (Scene::*)(const char*, const math::vec3f&)> (&Scene::CreateRigidBody)),
      make_invoker (xtl::implicit_cast<RigidBody (Scene::*)(const Shape&, float)> (&Scene::CreateRigidBody)),
      make_invoker<RigidBody (Scene&, const char*)> (xtl::bind (xtl::implicit_cast<RigidBody (Scene::*)(const char*, const math::vec3f&)> (&Scene::CreateRigidBody), _1, _2, math::vec3f (1.f)))
  ));
  lib.Register ("CreateSphericalJoint",   make_invoker (&Scene::CreateSphericalJoint));
  lib.Register ("CreateConeTwistJoint",   make_invoker (
      make_invoker (&Scene::CreateConeTwistJoint),
      make_invoker<Joint (Scene&, const JointBind&, const JointBind&, float)> (xtl::bind (&Scene::CreateConeTwistJoint, _1, _2, _3, _4, 360.f)),
      make_invoker<Joint (Scene&, const JointBind&, const JointBind&)> (xtl::bind (&Scene::CreateConeTwistJoint, _1, _2, _3, 360.f, 360.f))
  ));
  lib.Register ("CreateHingeJoint",          make_invoker (&Scene::CreateHingeJoint));
  lib.Register ("CreatePrismaticJoint",      make_invoker (&Scene::CreatePrismaticJoint));
  lib.Register ("PerformSimulation",         make_invoker (&Scene::PerformSimulation));
  lib.Register ("CreateCollisionFilter",     make_callback_invoker<Scene::BroadphaseCollisionFilter::signature_type> ());
  lib.Register ("AddCollisionFilter",        make_invoker (
      make_invoker (&Scene::AddCollisionFilter),
      make_invoker<size_t (Scene&, const char*, const char*, bool)> (xtl::bind (&Scene::AddCollisionFilter, _1, _2, _3, _4, Scene::BroadphaseCollisionFilter ()))
  ));
  lib.Register ("RemoveCollisionFilter",     make_invoker (&Scene::RemoveCollisionFilter));
  lib.Register ("RemoveAllCollisionFilters", make_invoker (&Scene::RemoveAllCollisionFilters));
  lib.Register ("CreateCollisionCallback",   make_callback_invoker<CollisionCallback::signature_type> ());
  lib.Register ("RegisterCollisionCallback", make_invoker (&Scene::RegisterCollisionCallback));

  environment.RegisterType<Scene> (SCENE_LIBRARY);
}

JointBind create_joint_bind (const RigidBody& body, const math::vec3f& anchor, const math::vec3f& axis)
{
  return JointBind (body, anchor, axis);
}

const RigidBody& get_joint_bind_body (const JointBind& joint_bind)
{
  return joint_bind.body;
}

const math::vec3f& get_joint_bind_axis (const JointBind& joint_bind)
{
  return joint_bind.axis;
}

const math::vec3f& get_joint_bind_anchor (const JointBind& joint_bind)
{
  return joint_bind.anchor;
}

void set_joint_bind_body (JointBind& joint_bind, const RigidBody& body)
{
  joint_bind.body = body;
}

void set_joint_bind_axis (JointBind& joint_bind, const math::vec3f& axis)
{
  joint_bind.axis = axis;
}

void set_joint_bind_anchor (JointBind& joint_bind, const math::vec3f& anchor)
{
  joint_bind.anchor = anchor;
}

void bind_joint_bind_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (JOINT_BIND_LIBRARY);

  lib.Register ("Create",     make_invoker (
      make_invoker<JointBind (const RigidBody&, const math::vec3f&, const math::vec3f&)> (xtl::bind (&create_joint_bind, _1, _2, _3)),
      make_invoker<JointBind (const RigidBody&, const math::vec3f&)> (xtl::bind (&create_joint_bind, _1, _2, math::vec3f (0, 0, 1))),
      make_invoker<JointBind (const RigidBody&)> (xtl::bind (&create_joint_bind, _1, math::vec3f (), math::vec3f (0, 0, 1)))
  ));
  lib.Register ("get_Body",   make_invoker (&get_joint_bind_body));
  lib.Register ("get_Anchor", make_invoker (&get_joint_bind_anchor));
  lib.Register ("get_Axis",   make_invoker (&get_joint_bind_axis));
  lib.Register ("set_Body",   make_invoker (&set_joint_bind_body));
  lib.Register ("set_Anchor", make_invoker (&set_joint_bind_anchor));
  lib.Register ("set_Axis",   make_invoker (&set_joint_bind_axis));

  environment.RegisterType<JointBind> (JOINT_BIND_LIBRARY);
}

void bind_joint_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (JOINT_LIBRARY);

  lib.Register ("get_Id",          make_invoker (&Joint::Id));
  lib.Register ("get_BodiesCount", make_invoker (&Joint::BodiesCount));
  lib.Register ("get_Body",        make_invoker (xtl::implicit_cast<const RigidBody& (Joint::*) (size_t) const>(&Joint::Body)));

  environment.RegisterType<Joint> (JOINT_LIBRARY);
}

void bind_collision_event_type_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (COLLISION_EVENT_TYPE_LIBRARY);

  lib.Register ("get_Begin",   make_const (CollisionEventType_Begin));
  lib.Register ("get_Process", make_const (CollisionEventType_Process));
  lib.Register ("get_End",     make_const (CollisionEventType_End));
}

void bind_shape_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SHAPE_LIBRARY);

  lib.Register ("get_Id",     make_invoker (&Shape::Id));
  lib.Register ("get_Margin", make_invoker (&Shape::Margin));
  lib.Register ("set_Margin", make_invoker (&Shape::SetMargin));

  environment.RegisterType<Shape> (SHAPE_LIBRARY);
}

void bind_material_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (MATERIAL_LIBRARY);

  lib.Register ("get_Id",                  make_invoker (&Material::Id));
  lib.Register ("get_LinearDamping",       make_invoker (&Material::LinearDamping));
  lib.Register ("get_AngularDamping",      make_invoker (&Material::AngularDamping));
  lib.Register ("get_Friction"       ,     make_invoker (&Material::Friction));
  lib.Register ("get_AnisotropicFriction", make_invoker (&Material::AnisotropicFriction));
  lib.Register ("get_Restitution",         make_invoker (&Material::Restitution));
  lib.Register ("set_LinearDamping",       make_invoker (&Material::SetLinearDamping));
  lib.Register ("set_AngularDamping",      make_invoker (&Material::SetAngularDamping));
  lib.Register ("set_Friction",            make_invoker (&Material::SetFriction));
  lib.Register ("set_AnisotropicFriction", make_invoker (&Material::SetAnisotropicFriction));
  lib.Register ("set_Restitution",         make_invoker (&Material::SetRestitution));
  lib.Register ("Clone",                   make_invoker (&Material::Clone));

  environment.RegisterType<Material> (MATERIAL_LIBRARY);
}

void bind_physics_library (Environment& environment)
{
  bind_manager_library (environment);
  bind_scene_library (environment);
  bind_joint_bind_library (environment);
  bind_joint_library (environment);
  bind_collision_event_type_library (environment);
  bind_shape_library (environment);
  bind_material_library (environment);
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
