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
const char* RIGID_BODY_LIBRARY           = "Physics.RigidBody";
const char* RIGID_BODY_FLAGS_LIBRARY     = "Physics.RigidBodyFlag";
const char* SCENE_LIBRARY                = "Physics.Scene";
const char* SHAPE_LIBRARY                = "Physics.Shape";
const char* SHAPE_LIST_LIBRARY           = "Physics.ShapeList";
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
  lib.Register ("CreateBoxShape",      make_invoker (&PhysicsManager::CreateBoxShape));
  lib.Register ("CreateSphereShape",   make_invoker (&PhysicsManager::CreateSphereShape));
  lib.Register ("CreateCapsuleShape",  make_invoker (&PhysicsManager::CreateCapsuleShape));
  lib.Register ("CreatePlaneShape",    make_invoker (&PhysicsManager::CreatePlaneShape));
  lib.Register ("CreateCompoundShape", make_invoker (&PhysicsManager::CreateCompoundShape));
  lib.Register ("CreateShape",         make_invoker (
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

ShapeList create_shape_list ()
{
  return ShapeList ();
}

const Shape& get_shape_list_shape (const ShapeList& list, size_t index)
{
  return list [index];
}

const math::vec3f& get_shape_list_shape_position (const ShapeList& list, size_t index)
{
  return list.Transform (index).position;
}

const math::quatf& get_shape_list_shape_orientation (const ShapeList& list, size_t index)
{
  return list.Transform (index).orientation;
}

void bind_shape_list_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SHAPE_LIST_LIBRARY);

  lib.Register ("get_Size", make_invoker (&ShapeList::Size));
  lib.Register ("Create",   make_invoker (&create_shape_list));
  lib.Register ("Add",      make_invoker (
      make_invoker (xtl::implicit_cast<void (ShapeList::*)(const Shape&, const math::vec3f&, const math::quatf&)> (&ShapeList::Add)),
      make_invoker (xtl::implicit_cast<void (ShapeList::*)(const Shape&, const math::quatf&)> (&ShapeList::Add)),
      make_invoker<void (ShapeList&, const Shape&, const math::vec3f&)> (xtl::bind (xtl::implicit_cast<void (ShapeList::*)(const Shape&, const math::vec3f&, const math::quatf&)> (&ShapeList::Add), _1, _2, _3, math::quatf ())),
      make_invoker<void (ShapeList&, const Shape&)> (xtl::bind (xtl::implicit_cast<void (ShapeList::*)(const Shape&, const math::vec3f&, const math::quatf&)> (&ShapeList::Add), _1, _2, math::vec3f (), math::quatf ()))
  ));
  lib.Register ("Remove",              make_invoker (&ShapeList::Remove));
  lib.Register ("Clear",               make_invoker (&ShapeList::Clear));
  lib.Register ("Shape",            make_invoker (&get_shape_list_shape));
  lib.Register ("ShapePosition",    make_invoker (&get_shape_list_shape_position));
  lib.Register ("ShapeOrientation", make_invoker (&get_shape_list_shape_orientation));

  environment.RegisterType<ShapeList> (SHAPE_LIST_LIBRARY);
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

const math::vec3f& get_rigid_body_world_position (const RigidBody& body)
{
  return body.WorldTransform ().position;
}

const math::quatf& get_rigid_body_world_orientation (const RigidBody& body)
{
  return body.WorldTransform ().orientation;
}

void set_rigid_body_world_position (RigidBody& body, const math::vec3f& position)
{
  Transform transform = body.WorldTransform ();

  transform.position = position;

  body.SetWorldTransform (transform);
}

void set_rigid_body_world_orientation (RigidBody& body, const math::quatf& orientation)
{
  Transform transform = body.WorldTransform ();

  transform.orientation = orientation;

  body.SetWorldTransform (transform);
}

bool get_rigid_body_flag (RigidBody& body, size_t flag)
{
  return body.CheckFlags (flag) == flag;
}

void bind_rigid_body_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (RIGID_BODY_LIBRARY);

  lib.Register ("get_Id",                     make_invoker (&RigidBody::Id));
  lib.Register ("get_Mass",                   make_invoker (&RigidBody::Mass));
  lib.Register ("get_MassSpaceInertiaTensor", make_invoker (&RigidBody::MassSpaceInertiaTensor));
  lib.Register ("get_WorldPosition",          make_invoker (&get_rigid_body_world_position));
  lib.Register ("get_WorldOrientation",       make_invoker (&get_rigid_body_world_orientation));
  lib.Register ("get_SleepLinearVelocity",    make_invoker (&RigidBody::SleepLinearVelocity));
  lib.Register ("get_SleepAngularVelocity",   make_invoker (&RigidBody::SleepAngularVelocity));
  lib.Register ("get_CcdMotionThreshold",     make_invoker (&RigidBody::CcdMotionThreshold));
  lib.Register ("get_Shape",                  make_invoker (xtl::implicit_cast<const Shape& (RigidBody::*) () const> (&RigidBody::Shape)));
  lib.Register ("get_Material",               make_invoker (xtl::implicit_cast<const Material& (RigidBody::*) () const> (&RigidBody::Material)));
  lib.Register ("get_CollisionGroup",         make_invoker (&RigidBody::CollisionGroup));
  lib.Register ("get_LinearVelocity",         make_invoker (&RigidBody::LinearVelocity));
  lib.Register ("get_AngularVelocity",        make_invoker (&RigidBody::AngularVelocity));
  lib.Register ("set_Mass",                   make_invoker (&RigidBody::SetMass));
  lib.Register ("set_MassSpaceInertiaTensor", make_invoker (&RigidBody::SetMassSpaceInertiaTensor));
  lib.Register ("set_WorldPosition",          make_invoker (&set_rigid_body_world_position));
  lib.Register ("set_WorldOrientation",       make_invoker (&set_rigid_body_world_orientation));
  lib.Register ("set_SleepLinearVelocity",    make_invoker (&RigidBody::SetSleepLinearVelocity));
  lib.Register ("set_SleepAngularVelocity",   make_invoker (&RigidBody::SetSleepAngularVelocity));
  lib.Register ("set_CcdMotionThreshold",     make_invoker (&RigidBody::SetCcdMotionThreshold));
  lib.Register ("set_Material",               make_invoker (&RigidBody::SetMaterial));
  lib.Register ("set_CollisionGroup",         make_invoker (&RigidBody::SetCollisionGroup));
  lib.Register ("set_LinearVelocity",         make_invoker (&RigidBody::SetLinearVelocity));
  lib.Register ("set_AngularVelocity",        make_invoker (&RigidBody::SetAngularVelocity));
  lib.Register ("GetFlag",                    make_invoker (&get_rigid_body_flag));
  lib.Register ("SetFlag",                    make_invoker (&RigidBody::ChangeFlags));
  lib.Register ("AddForce",                   make_invoker (
      make_invoker (&RigidBody::AddForce),
      make_invoker<void (RigidBody&, const math::vec3f&)> (xtl::bind (&RigidBody::AddForce, _1, _2, math::vec3f (0.f)))
  ));
  lib.Register ("AddImpulse",                 make_invoker (
      make_invoker (&RigidBody::AddImpulse),
      make_invoker<void (RigidBody&, const math::vec3f&)> (xtl::bind (&RigidBody::AddImpulse, _1, _2, math::vec3f (0.f)))
  ));
  lib.Register ("AddTorque",                       make_invoker (&RigidBody::AddTorque));
  lib.Register ("CreateTransformUpdateCallback",   make_callback_invoker<RigidBody::TransformUpdateCallback::signature_type> ());
  lib.Register ("RegisterTransformUpdateCallback", make_invoker (&RigidBody::RegisterTransformUpdateCallback));
  lib.Register ("CreateCollisionCallback",         make_callback_invoker<CollisionCallback::signature_type> ());
  lib.Register ("RegisterCollisionCallback",       make_invoker (&RigidBody::RegisterCollisionCallback));

  environment.RegisterType<RigidBody> (RIGID_BODY_LIBRARY);
}

void bind_rigid_body_flags_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (RIGID_BODY_FLAGS_LIBRARY);

  lib.Register ("get_FrozenPositionX",   make_const (RigidBodyFlag_FrozenPositionX));
  lib.Register ("get_FrozenPositionY",   make_const (RigidBodyFlag_FrozenPositionY));
  lib.Register ("get_FrozenPositionZ",   make_const (RigidBodyFlag_FrozenPositionZ));
  lib.Register ("get_FrozenPosition",    make_const (RigidBodyFlag_FrozenPosition));
  lib.Register ("get_FrozenRotationX",   make_const (RigidBodyFlag_FrozenRotationX));
  lib.Register ("get_FrozenRotationY",   make_const (RigidBodyFlag_FrozenRotationY));
  lib.Register ("get_FrozenRotationZ",   make_const (RigidBodyFlag_FrozenRotationZ));
  lib.Register ("get_FrozenRotation",    make_const (RigidBodyFlag_FrozenRotation));
  lib.Register ("get_Kinematic",         make_const (RigidBodyFlag_Kinematic));
}

void bind_physics_library (Environment& environment)
{
  bind_manager_library (environment);
  bind_scene_library (environment);
  bind_joint_bind_library (environment);
  bind_joint_library (environment);
  bind_collision_event_type_library (environment);
  bind_shape_library (environment);
  bind_shape_list_library (environment);
  bind_material_library (environment);
  bind_rigid_body_library (environment);
  bind_rigid_body_flags_library (environment);
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
