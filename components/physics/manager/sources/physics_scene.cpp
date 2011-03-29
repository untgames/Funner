#include "shared.h"

using namespace physics;

/*
   Физическая сцена
*/

typedef xtl::com_ptr<physics::low_level::IDriver>                      DriverPtr;
typedef stl::hash_map<physics::low_level::IRigidBody*, RigidBodyImpl*> RigidBodyMap;

struct Scene::Impl : public xtl::reference_counter, public xtl::trackable
{
  DriverPtr                                           driver;
  ScenePtr                                            scene;
  media::physics::PhysicsLibrary::RigidBodyCollection body_collection;
  RigidBodyMap                                        rigid_body_map;

  Impl (physics::low_level::IDriver* in_driver, ScenePtr in_scene, const media::physics::PhysicsLibrary::RigidBodyCollection& in_body_collection)
    : scene (in_scene), body_collection (in_body_collection)
  {
    if (!driver)
      throw xtl::make_null_argument_exception ("physics::Scene::Impl::Impl", "driver");

    driver = in_driver;

    scene->SetDefaultCollisionFilter (xtl::bind (&Scene::Impl::CollisionHandler, this, _1, _2));
  }

  ///Обработка удаления тела
  void OnBodyDestroy (RigidBodyImpl* body_impl)
  {
    rigid_body_map.erase (body_impl->LowLevelBody ());
  }

  ///Создание тел
  RigidBody CreateBody (RigidBodyPtr body, const Shape& shape, const Material& material)
  {
    RigidBody     return_value = RigidBodyImplProvider::CreateRigidBody (body, shape, material);
    RigidBodyImpl *body_impl   = RigidBodyImplProvider::Impl (return_value);

    body_impl->connect_tracker (xtl::bind (&Scene::Impl::OnBodyDestroy, this, body_impl), *this);

    rigid_body_map.insert_pair (body.get (), body_impl);

    return return_value;
  }

  RigidBody CreateBody (const char* name)
  {
    try
    {
      media::physics::RigidBody *media_body = body_collection.Find (name);

      if (!media_body)
        throw xtl::format_operation_exception ("", "Body '%s' was not loaded", name);

      Shape        shape        (ShapeImplProvider::CreateShape (driver.get (), media_body->Shape ()));
      RigidBodyPtr body         (scene->CreateRigidBody (ShapeImplProvider::LowLevelShape (shape), media_body->Mass ()));
      Material     material     (MaterialImplProvider::CreateMaterial (driver.get (), media_body->Material ()));
      RigidBody    return_value (CreateBody (body, shape, material));

      return_value.SetMassSpaceInertiaTensor (media_body->MassSpaceInertiaTensor ());

      size_t flags = 0, media_flags = media_body->Flags ();

      if (media_flags && media::physics::RigidBodyFlag_FrozenPositionX)
        flags |= physics::RigidBodyFlag_FrozenPositionX;
      if (media_flags && media::physics::RigidBodyFlag_FrozenPositionY)
        flags |= physics::RigidBodyFlag_FrozenPositionY;
      if (media_flags && media::physics::RigidBodyFlag_FrozenPositionZ)
        flags |= physics::RigidBodyFlag_FrozenPositionZ;
      if (media_flags && media::physics::RigidBodyFlag_FrozenRotationX)
        flags |= physics::RigidBodyFlag_FrozenRotationX;
      if (media_flags && media::physics::RigidBodyFlag_FrozenRotationY)
        flags |= physics::RigidBodyFlag_FrozenRotationY;
      if (media_flags && media::physics::RigidBodyFlag_FrozenRotationZ)
        flags |= physics::RigidBodyFlag_FrozenRotationZ;
      if (media_flags && media::physics::RigidBodyFlag_Kinematic)
        flags |= physics::RigidBodyFlag_Kinematic;

      return_value.SetFlags (flags);

      return return_value;
    }
    catch (xtl::exception& e)
    {
      e.touch ("physics::PhysicsManager::CreateMaterial (const char*)");
      throw;
    }
  }

  RigidBody CreateBody (const Shape& shape, float mass)
  {
    RigidBodyPtr body (scene->CreateRigidBody (ShapeImplProvider::LowLevelShape (shape), mass));
    Material     material (MaterialImplProvider::CreateMaterial (driver.get ()));

    return CreateBody (body, shape, material);
  }

  ///Создание связей между телами
  Joint CreateSphericalJoint (const JointBind& bind1, const JointBind& bind2)
  {
    RigidBodyArray bodies_array;

    bodies_array.push_back (bind1.body);
    bodies_array.push_back (bind2.body);

    physics::low_level::SphericalJointDesc joint_desc;

    joint_desc.anchor [0] = bind1.anchor;
    joint_desc.anchor [1] = bind2.anchor;

    JointPtr low_level_joint (scene->CreateSphericalJoint (RigidBodyImplProvider::LowLevelBody (bind1.body), RigidBodyImplProvider::LowLevelBody (bind2.body), joint_desc), false);

    return JointImplProvider::CreateJoint (low_level_joint, bodies_array);
  }

  Joint CreateConeTwistJoint (const JointBind& bind1, const JointBind& bind2, float swing_limit_in_degrees, float twist_limit)
  {
    RigidBodyArray bodies_array;

    bodies_array.push_back (bind1.body);
    bodies_array.push_back (bind2.body);

    physics::low_level::ConeTwistJointDesc joint_desc;

    joint_desc.anchor [0]  = bind1.anchor;
    joint_desc.anchor [1]  = bind2.anchor;
    joint_desc.swing_limit = swing_limit_in_degrees;
    joint_desc.twist_limit = twist_limit;

    JointPtr low_level_joint (scene->CreateConeTwistJoint (RigidBodyImplProvider::LowLevelBody (bind1.body), RigidBodyImplProvider::LowLevelBody (bind2.body), joint_desc), false);

    return JointImplProvider::CreateJoint (low_level_joint, bodies_array);
  }

  Joint CreateHingeJoint (const JointBind& bind1, const JointBind& bind2)
  {
    RigidBodyArray bodies_array;

    bodies_array.push_back (bind1.body);
    bodies_array.push_back (bind2.body);

    physics::low_level::HingeJointDesc joint_desc;

    joint_desc.anchor [0]  = bind1.anchor;
    joint_desc.anchor [1]  = bind2.anchor;
    joint_desc.axis [0]    = bind1.axis;
    joint_desc.axis [1]    = bind2.axis;

    JointPtr low_level_joint (scene->CreateHingeJoint (RigidBodyImplProvider::LowLevelBody (bind1.body), RigidBodyImplProvider::LowLevelBody (bind2.body), joint_desc), false);

    return JointImplProvider::CreateJoint (low_level_joint, bodies_array);
  }

  Joint CreatePrismaticJoint (const JointBind& bind1, const JointBind& bind2)
  {
    RigidBodyArray bodies_array;

    bodies_array.push_back (bind1.body);
    bodies_array.push_back (bind2.body);

    physics::low_level::PrismaticJointDesc joint_desc;

    joint_desc.anchor [0]  = bind1.anchor;
    joint_desc.anchor [1]  = bind2.anchor;
    joint_desc.axis [0]    = bind1.axis;
    joint_desc.axis [1]    = bind2.axis;

    JointPtr low_level_joint (scene->CreatePrismaticJoint (RigidBodyImplProvider::LowLevelBody (bind1.body), RigidBodyImplProvider::LowLevelBody (bind2.body), joint_desc), false);

    return JointImplProvider::CreateJoint (low_level_joint, bodies_array);
  }

  /*
    Фильтрация столкновений объектов. Объекты сталкиваются, если:
     - не задан фильтр;
     - задан дефолтный фильтр и collides = true;
     - заданный фильтр возвращает true и collides = true;
     - заданный фильтр возвращает false и collides = false.
    Приоритет фильтрации: чем позже добавлен фильтр, тем выше его приоритет
  */

  bool CollisionFilter (IRigidBody* body1, IRigidBody* body2)
  {

  }

  size_t AddCollisionFilter (const char* group1_mask, const char* group2_mask, bool collides, const Scene::BroadphaseCollisionFilter& filter)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::AddCollisionFilter");
  }

  void RemoveCollisionFilter (size_t id)
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveCollisionFilter");
  }

  void RemoveAllCollisionFilters ()
  {
    throw xtl::make_not_implemented_exception ("physics::SceneImpl::RemoveAllCollisionFilters");
  }

  /*
     Обработка столкновений объектов
  */

  physics::low_level::CollisionEventType ConvertCollisionEventType (physics::CollisionEventType type)
  {
    switch (type)
    {
      case physics::CollisionEventType_Begin:   return physics::low_level::CollisionEventType_Begin;
      case physics::CollisionEventType_Process: return physics::low_level::CollisionEventType_Process;
      case physics::CollisionEventType_End:     return physics::low_level::CollisionEventType_End;
      default:
        throw xtl::make_argument_exception ("physics::Scene::Impl::ConvertCollisionEventType", "type", type);
    }
  }

  xtl::connection RegisterCollisionCallback (const char* group1_mask, const char* group2_mask, physics::CollisionEventType event_type, const CollisionCallback& callback_handler)
  {
    return scene->RegisterCollisionCallback (ConvertCollisionEventType (event_type),
                                             xtl::bind (&Scene::Impl::CollisionHandler, this, _1, stl::string (group1_mask), stl::string (group2_mask), event_type, callback_handler));
  }

  void CollisionHandler (const physics::low_level::CollisionEvent& event, const stl::string& group1_mask,
                         const stl::string& group2_mask, CollisionEventType wanted_event, const CollisionCallback& callback_handler)
  {
    if (event.type != ConvertCollisionEventType (wanted_event))
      return;

    RigidBodyMap::iterator body1_iter = rigid_body_map.find (event.body [0]),
                           body2_iter = rigid_body_map.find (event.body [1]);

    if (body1_iter == rigid_body_map.end () || body2_iter == rigid_body_map.end ())
      throw xtl::format_operation_exception ("physics::Scene::Impl::CollisionHandler", "Can't find all collided bodies");

    RigidBody body1 = RigidBodyImplProvider::CreateRigidBody (body1_iter->second),
              body2 = RigidBodyImplProvider::CreateRigidBody (body2_iter->second);

    if (!common::wcmatch (body1.CollisionGroup (), group1_mask.c_str ()) && !common::wcmatch (body1.CollisionGroup (), group2_mask.c_str ()) ||
        !common::wcmatch (body2.CollisionGroup (), group1_mask.c_str ()) && !common::wcmatch (body2.CollisionGroup (), group2_mask.c_str ()))
      return;

    callback_handler (wanted_event, body1, body2, event.point);
  }
};

/*
   Конструктор / деструктор / копирование
*/

Scene::Scene (Impl* source_impl)
  : impl (source_impl)
  {}

Scene::Scene (const Scene& source)
  : impl (source.impl)
{
  addref (impl);
}

Scene::~Scene ()
{
  release (impl);
}

Scene& Scene::operator = (const Scene& source)
{
  Scene (source).Swap (*this);

  return *this;
}

/*
   Создание тел
*/

RigidBody Scene::CreateBody (const char* name)
{
  return impl->CreateBody (name);
}

RigidBody Scene::CreateBody (const Shape& shape, float mass)
{
  return impl->CreateBody (shape, mass);
}

/*
   Создание связей между телами
*/

Joint Scene::CreateSphericalJoint (const JointBind& bind1, const JointBind& bind2)
{
  return impl->CreateSphericalJoint (bind1, bind2);
}

Joint Scene::CreateConeTwistJoint (const JointBind& bind1, const JointBind& bind2, float swing_limit_in_degrees, float twist_limit)
{
  return impl->CreateConeTwistJoint (bind1, bind2, swing_limit_in_degrees, twist_limit);
}

Joint Scene::CreateHingeJoint (const JointBind& bind1, const JointBind& bind2)
{
  return impl->CreateHingeJoint (bind1, bind2);
}

Joint Scene::CreatePrismaticJoint (const JointBind& bind1, const JointBind& bind2)
{
  return impl->CreatePrismaticJoint (bind1, bind2);
}

/*
   Управление гравитацией
*/

const math::vec3f& Scene::Gravity () const
{
  return impl->scene->Gravity ();
}

void Scene::SetGravity (const math::vec3f& value)
{
  impl->scene->SetGravity (value);
}

/*
   Параметры симуляции
*/

float Scene::SimulationStep () const
{
  return impl->scene->SimulationStep ();
}

void Scene::SetSimulationStep (float step)
{
  impl->scene->SetSimulationStep (step);
}

/*
   Симуляция
*/
void Scene::PerformSimulation (float dt)
{
  impl->scene->PerformSimulation (dt);
}

/*
  Фильтрация столкновений объектов. Объекты сталкиваются, если:
   - не задан фильтр;
   - задан дефолтный фильтр и collides = true;
   - заданный фильтр возвращает true и collides = true;
   - заданный фильтр возвращает false и collides = false.
  Приоритет фильтрации: чем позже добавлен фильтр, тем выше его приоритет
*/

size_t Scene::AddCollisionFilter (const char* group1_mask, const char* group2_mask, bool collides, const BroadphaseCollisionFilter& filter)
{
  return impl->AddCollisionFilter (group1_mask, group2_mask, collides, filter);
}

void Scene::RemoveCollisionFilter (size_t id)
{
  impl->RemoveCollisionFilter (id);
}

void Scene::RemoveAllCollisionFilters ()
{
  impl->RemoveAllCollisionFilters ();
}

/*
   Обработка столкновений объектов
*/

xtl::connection Scene::RegisterCollisionCallback (const char* group1_mask, const char* group2_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  return impl->RegisterCollisionCallback (group1_mask, group2_mask, event_type, callback_handler);
}

/*
   Отладочная отрисовка
*/

void Scene::Draw (render::debug::PrimitiveRender& render)
{
  impl->scene->Draw (render);
}

/*
   Обмен
*/

void Scene::Swap (Scene& scene)
{
  stl::swap (impl, scene.impl);
}

namespace physics
{

/*
   Обмен
*/

void swap (Scene& scene1, Scene& scene2)
{
  scene1.Swap (scene2);
}

}

/*
   Создание
*/

Scene SceneImplProvider::CreateScene (physics::low_level::IDriver* driver, ScenePtr scene, const media::physics::PhysicsLibrary::RigidBodyCollection& body_collection)
{
  return Scene (new Scene::Impl (driver, scene, body_collection));
}
