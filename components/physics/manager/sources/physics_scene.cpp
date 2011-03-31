#include "shared.h"

using namespace physics;

/*
   Физическая сцена
*/

namespace
{

const size_t FILTERS_RESERVE_SIZE = 32;

struct CollisionFilterDesc : public xtl::reference_counter
{
  stl::string                      group1_mask;
  stl::string                      group2_mask;
  bool                             collides;
  Scene::BroadphaseCollisionFilter filter;
  size_t                           id;

  CollisionFilterDesc (const char* in_group1_mask, const char* in_group2_mask, bool in_collides, const Scene::BroadphaseCollisionFilter& in_filter, size_t in_id)
    : group1_mask (in_group1_mask)
    , group2_mask (in_group2_mask)
    , collides (in_collides)
    , filter (in_filter)
    , id (in_id)
    {}
};

struct CollisionGroupDesc
{
  stl::string name;
  size_t      id;

  CollisionGroupDesc (const char* in_name, size_t in_id)
    : name (in_name), id (in_id)
    {}
};

typedef xtl::intrusive_ptr<CollisionFilterDesc>                        CollisionFilterDescPtr;
typedef stl::vector<CollisionFilterDescPtr>                            CollisionFilterDescArray;
typedef xtl::com_ptr<physics::low_level::IDriver>                      DriverPtr;
typedef stl::hash_map<physics::low_level::IRigidBody*, RigidBodyImpl*> RigidBodyMap;
typedef stl::hash_map<stl::hash_key<const char*>, CollisionGroupDesc>  CollisionGroupsMap;
typedef stl::pair<size_t, size_t>                                      IndexPair;
typedef stl::hash_map<IndexPair, CollisionFilterDescPtr>               CollisionFiltersMap;

class CollisionHandler : public xtl::reference_counter, public xtl::trackable
{
  public:
    CollisionHandler (const char* in_group1_mask, const char* in_group2_mask, physics::CollisionEventType in_event_type,
                      const CollisionCallback& in_callback_handler)
      : group1_mask (in_group1_mask)
      , group2_mask (in_group2_mask)
      , event_type (in_event_type)
      , callback_handler (in_callback_handler)
      {}

    void OnNewGroupRegistered (const CollisionGroupsMap& groups, const CollisionGroupDesc& new_group)
    {
      bool mask1_matched = false, mask2_matched = false;

      if (common::wcmatch (new_group.name.c_str (), group1_mask.c_str ()))
        mask1_matched = true;
      if (common::wcmatch (new_group.name.c_str (), group2_mask.c_str ()))
        mask2_matched = true;

      if (!mask1_matched && !mask2_matched)
        return;

      for (CollisionGroupsMap::const_iterator iter = groups.begin (), end = groups.end (); iter != end; ++iter)
      {
        const CollisionGroupDesc& iter_group = iter->second;

        if (!((mask1_matched && common::wcmatch (iter_group.name.c_str (), group2_mask.c_str ())) ||
             (mask2_matched && common::wcmatch (iter_group.name.c_str (), group1_mask.c_str ()))))
          continue;

        size_t group1 = new_group.id, group2 = iter_group.id;

        if (group1 > group2)
        {
          size_t temp = group1;

          group1 = group2;
          group2 = temp;
        }

        valid_groups.insert (IndexPair (group1, group2));
      }
    }

    void OnCollision (const physics::low_level::CollisionEvent& event, RigidBody& body1, RigidBody& body2)
    {
      size_t group1 = event.body [0]->CollisionGroup (), group2 = event.body [1]->CollisionGroup ();

      if (group1 > group2)
      {
        size_t temp = group1;

        group1 = group2;
        group2 = temp;
      }

      if (valid_groups.find (IndexPair (group1, group2)) == valid_groups.end ())
        return;

      callback_handler (event_type, body1, body2, event.point);
    }

  private:
    typedef stl::hash_set<IndexPair> IndexPairSet;

  private:
    stl::string                 group1_mask;
    stl::string                 group2_mask;
    physics::CollisionEventType event_type;
    CollisionCallback           callback_handler;
    IndexPairSet                valid_groups;
};

typedef xtl::intrusive_ptr<CollisionHandler> CollisionHandlerPtr;
typedef stl::hash_set<CollisionHandler*>     CollisionHandlersSet;

}

struct Scene::Impl : public xtl::reference_counter, public xtl::trackable
{
  DriverPtr                                           driver;
  RigidBodyMap                                        rigid_body_map;
  CollisionGroupsMap                                  collision_groups;              //Зарегистрированные группы коллизий
  CollisionFilterDescArray                            collision_filters;             //Зарегистрированные в низкоуровневой сцене фильтры
  CollisionFiltersMap                                 registered_filters;            //Зарегистрированные фильтры
  xtl::auto_connection                                collision_connections [physics::low_level::CollisionEventType_Num];  //Соединения обработки коллизий для оповещения тел
  CollisionHandlersSet                                registered_collision_handlers; //Зарегестрированные обработчики коллизий
  ScenePtr                                            scene;
  media::physics::PhysicsLibrary::RigidBodyCollection body_collection;               //Тела сцены
  size_t                                              next_collision_filter_id;
  size_t                                              next_collision_group_id;

  Impl (physics::low_level::IDriver* in_driver, ScenePtr in_scene, const media::physics::PhysicsLibrary::RigidBodyCollection& in_body_collection)
    : driver (in_driver)
    , scene (in_scene)
    , body_collection (in_body_collection)
    , next_collision_filter_id (0)
    , next_collision_group_id (0)
  {
    if (!driver)
      throw xtl::make_null_argument_exception ("physics::Scene::Impl::Impl", "driver");

    collision_filters.reserve (FILTERS_RESERVE_SIZE);

    for (size_t i = 0; i < physics::low_level::CollisionEventType_Num; i++)
      collision_connections [i] = scene->RegisterCollisionCallback ((physics::low_level::CollisionEventType)i, xtl::bind (&Scene::Impl::BodyCollisionHandler, this, _1));
  }

  //Получение номера группы коллизий по имени.
  size_t CollisionGroupForName (const char* name)
  {
    if (!name)
      return 0;

    CollisionGroupsMap::iterator iter = collision_groups.find (name);

    if (iter != collision_groups.end ())
      return iter->second.id;

    if (next_collision_group_id == (size_t)-1)
      throw xtl::format_operation_exception ("physics::Scene::Impl::CollisionGroupForName", "All groups ids used");

    CollisionGroupDesc new_group_desc (name, next_collision_group_id);

    collision_groups.insert_pair (name, new_group_desc);

    for (CollisionFilterDescArray::iterator iter = collision_filters.begin (), end = collision_filters.end (); iter != end; ++iter)
      RegisterCollisionFilter (*iter);

    for (CollisionHandlersSet::iterator iter = registered_collision_handlers.begin (), end = registered_collision_handlers.end (); iter != end; ++iter)
      (*iter)->OnNewGroupRegistered (collision_groups, new_group_desc);

    return next_collision_group_id++;
  }

  ///Обработка удаления тела
  void OnBodyDestroy (RigidBodyImpl* body_impl)
  {
    rigid_body_map.erase (body_impl->LowLevelBody ());
  }

  ///Обработка удаления обработчика коллизий
  void OnCollisionHandlerDestroy (CollisionHandler* handler)
  {
    registered_collision_handlers.erase (handler);
  }

  ///Создание тел
  RigidBody CreateRigidBody (RigidBodyPtr body, const Shape& shape, const Material& material)
  {
    RigidBody     return_value = RigidBodyImplProvider::CreateRigidBody (body, shape, material, SceneImplProvider::CreateScene (this));
    RigidBodyImpl *body_impl   = RigidBodyImplProvider::Impl (return_value);

    body_impl->connect_tracker (xtl::bind (&Scene::Impl::OnBodyDestroy, this, body_impl), *this);

    rigid_body_map.insert_pair (body.get (), body_impl);

    return return_value;
  }

  RigidBody CreateRigidBody (const char* name)
  {
    try
    {
      media::physics::RigidBody *media_body = body_collection.Find (name);

      if (!media_body)
        throw xtl::format_operation_exception ("", "Body '%s' was not loaded", name);

      Shape        shape        (ShapeImplProvider::CreateShape (driver.get (), media_body->Shape ()));
      RigidBodyPtr body         (scene->CreateRigidBody (ShapeImplProvider::LowLevelShape (shape), media_body->Mass ()), false);
      Material     material     (MaterialImplProvider::CreateMaterial (driver.get (), media_body->Material ()));
      RigidBody    return_value (CreateRigidBody (body, shape, material));

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

  RigidBody CreateRigidBody (const Shape& shape, float mass)
  {
    RigidBodyPtr body (scene->CreateRigidBody (ShapeImplProvider::LowLevelShape (shape), mass), false);
    Material     material (MaterialImplProvider::CreateMaterial (driver.get ()));

    return CreateRigidBody (body, shape, material);
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

  RigidBodyImpl* FindRigidBodyImpl (physics::low_level::IRigidBody* body)
  {
    RigidBodyMap::iterator body_iter = rigid_body_map.find (body);

    return body_iter == rigid_body_map.end () ? 0 : body_iter->second;
  }

  /*
    Фильтрация столкновений объектов. Объекты сталкиваются, если:
     - не задан фильтр;
     - задан дефолтный фильтр и collides = true;
     - заданный фильтр возвращает true и collides = true;
     - заданный фильтр возвращает false и collides = false.
    Приоритет фильтрации: чем позже добавлен фильтр, тем выше его приоритет
  */

  bool CollisionFilter (physics::low_level::IRigidBody* low_level_body1, physics::low_level::IRigidBody* low_level_body2, const Scene::BroadphaseCollisionFilter& filter)
  {
    RigidBodyImpl *body1_impl = FindRigidBodyImpl (low_level_body1),
                  *body2_impl = FindRigidBodyImpl (low_level_body2);

    if (!body1_impl || !body2_impl)
      return true;

    RigidBody body1 = RigidBodyImplProvider::CreateRigidBody (body1_impl),
              body2 = RigidBodyImplProvider::CreateRigidBody (body2_impl);

    return filter (body1, body2);
  }

  void RegisterCollisionFilter (CollisionFilterDescPtr filter)
  {
    for (CollisionGroupsMap::iterator group1_iter = collision_groups.begin (), group_end_iter = collision_groups.end (); group1_iter != group_end_iter; ++group1_iter)
    {
      const CollisionGroupDesc& group1 = group1_iter->second;

      bool mask1_matched = false, mask2_matched = false;

      if (common::wcmatch (group1.name.c_str (), filter->group1_mask.c_str ()))
        mask1_matched = true;
      if (common::wcmatch (group1.name.c_str (), filter->group2_mask.c_str ()))
        mask2_matched = true;

      if (!mask1_matched && !mask2_matched)
        continue;

      for (CollisionGroupsMap::iterator group2_iter = group1_iter; group2_iter != group_end_iter; ++group2_iter)
      {
        const CollisionGroupDesc& group2 = group2_iter->second;

        size_t first_group_id, second_group_id;

        if (group1.id < group2.id)
        {
          first_group_id  = group1.id;
          second_group_id = group2.id;
        }
        else
        {
          first_group_id  = group2.id;
          second_group_id = group1.id;
        }

        IndexPair group_pair (first_group_id, second_group_id);

        if (registered_filters.find (group_pair) != registered_filters.end ())
          continue;

        if (!((mask1_matched && common::wcmatch (group2.name.c_str (), filter->group2_mask.c_str ())) ||
             (mask2_matched && common::wcmatch (group2.name.c_str (), filter->group1_mask.c_str ()))))
          continue;

        scene->SetCollisionFilter (first_group_id, second_group_id, filter->collides,
                                   filter->filter ? xtl::bind (&Scene::Impl::CollisionFilter, this, _1, _2, filter->filter) : physics::low_level::IScene::BroadphaseCollisionFilter ());

        registered_filters.insert_pair (group_pair, filter);
      }
    }
  }

  size_t AddCollisionFilter (const char* group1_mask, const char* group2_mask, bool collides, const Scene::BroadphaseCollisionFilter& filter)
  {
    static const char* METHOD_NAME = "physics::SceneImpl::AddCollisionFilter";

    if (!group1_mask)
      throw xtl::make_null_argument_exception (METHOD_NAME, "group1_mask");

    if (!group2_mask)
      throw xtl::make_null_argument_exception (METHOD_NAME, "group2_mask");

    if (next_collision_filter_id == (size_t)-1)
      throw xtl::format_operation_exception (METHOD_NAME, "All filters ids used");

    CollisionFilterDescPtr filter_desc (new CollisionFilterDesc (group1_mask, group2_mask, collides, filter, next_collision_filter_id), false);

    collision_filters.push_back (filter_desc);

    RegisterCollisionFilter (filter_desc);

    return next_collision_filter_id++;
  }

  void RemoveCollisionFilter (size_t id)
  {
    for (CollisionFilterDescArray::iterator iter = collision_filters.begin (), end = collision_filters.end (); iter != end; ++iter)
    {
      if ((*iter)->id != id)
        continue;

      bool filter_map_modified = false;

      for (CollisionFiltersMap::iterator filter_iter = registered_filters.begin (); filter_iter != registered_filters.end ();)
      {
        if (filter_iter->second == *iter)  //Фильтр необходимо разорвать
        {
          filter_map_modified = true;

          scene->SetCollisionFilter (filter_iter->first.first, filter_iter->first.second, true);

          CollisionFiltersMap::iterator next = filter_iter;

          ++next;

          registered_filters.erase (filter_iter);

          filter_iter = next;
        }
        else
          ++filter_iter;
      }

      if (filter_map_modified)
      {
        for (CollisionFilterDescArray::iterator update_iter = iter + 1; update_iter != end; ++update_iter)
          RegisterCollisionFilter (*update_iter);
      }

      collision_filters.erase (iter);

      return;
    }
  }

  void RemoveAllCollisionFilters ()
  {
    for (CollisionFiltersMap::iterator iter = registered_filters.begin (), end = registered_filters.end (); iter != end; ++iter)
      scene->SetCollisionFilter (iter->first.first, iter->first.second, true);

    registered_filters.clear ();
    collision_filters.clear ();
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
    try
    {
      if (!group1_mask)
        throw xtl::make_null_argument_exception ("", "group1_mask");

      if (!group2_mask)
        throw xtl::make_null_argument_exception ("", "group2_mask");

      CollisionHandlerPtr handler (new ::CollisionHandler (group1_mask, group2_mask, event_type, callback_handler), false);

      registered_collision_handlers.insert (handler.get ());

      handler->connect_tracker (xtl::bind (&Scene::Impl::OnCollisionHandlerDestroy, this, handler.get ()), *this);

      return scene->RegisterCollisionCallback (ConvertCollisionEventType (event_type),
                                               xtl::bind (&Scene::Impl::CollisionHandler, this, _1, handler));
    }
    catch (xtl::exception& e)
    {
      e.touch ("physics::Scene::RegisterCollisionCallback");
      throw;
    }
  }

  void BodyCollisionHandler (const physics::low_level::CollisionEvent& event)
  {
    static const char* METHOD_NAME = "physics::Scene::Impl::BodyCollisionHandler";

    RigidBodyImpl *body1_impl = FindRigidBodyImpl (event.body [0]),
                  *body2_impl = FindRigidBodyImpl (event.body [1]);

    if (!body1_impl || !body2_impl)
      throw xtl::format_operation_exception (METHOD_NAME, "Can't find all collided bodies");

    RigidBody body1 = RigidBodyImplProvider::CreateRigidBody (body1_impl),
              body2 = RigidBodyImplProvider::CreateRigidBody (body2_impl);

    CollisionEventType event_type;

    switch (event.type)
    {
      case physics::low_level::CollisionEventType_Begin:
        event_type = physics::CollisionEventType_Begin;
        break;
      case physics::low_level::CollisionEventType_Process:
        event_type = physics::CollisionEventType_Process;
        break;
      case physics::low_level::CollisionEventType_End:
        event_type = physics::CollisionEventType_End;
        break;
      default:
        throw xtl::format_operation_exception (METHOD_NAME, "Unsupported event type %d", event.type);
    }

    body1_impl->OnCollision (event_type, body2, event.point);
    body2_impl->OnCollision (event_type, body1, event.point);
  }

  void CollisionHandler (const physics::low_level::CollisionEvent& event, CollisionHandlerPtr handler)
  {
    RigidBodyImpl *body1_impl = FindRigidBodyImpl (event.body [0]),
                  *body2_impl = FindRigidBodyImpl (event.body [1]);

    if (!body1_impl || !body2_impl)
      throw xtl::format_operation_exception ("physics::Scene::Impl::CollisionHandler", "Can't find all collided bodies");

    RigidBody body1 = RigidBodyImplProvider::CreateRigidBody (body1_impl),
              body2 = RigidBodyImplProvider::CreateRigidBody (body2_impl);

    handler->OnCollision (event, body1, body2);
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

RigidBody Scene::CreateRigidBody (const char* name)
{
  return impl->CreateRigidBody (name);
}

RigidBody Scene::CreateRigidBody (const Shape& shape, float mass)
{
  return impl->CreateRigidBody (shape, mass);
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

Scene SceneImplProvider::CreateScene (Scene::Impl* impl)
{
  addref (impl);

  try
  {
    return Scene (impl);
  }
  catch (...)
  {
    release (impl);
    throw;
  }
}

/*
   Получение номера группы коллизий по имени
*/

size_t SceneImplProvider::CollisionGroupForName (const Scene& scene, const char* name)
{
  return scene.impl->CollisionGroupForName (name);
}
