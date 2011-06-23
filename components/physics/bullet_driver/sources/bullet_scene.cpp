#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

namespace
{

const char*       DEBUG_DRAWER_LOG_NAME = "physics.low_level.bullet.debug_drawer";
const math::vec3f DEFAULT_GRAVITY (0, -9.8f, 0);
const float       DEFAULT_SIMULATION_STEP = 1.f / 60.f;
const math::vec4f DRAW_TEXT_COLOR (0.f, 0.f, 0.f, 1.f);
const size_t      MAX_SIMULATION_SUBSTEPS = 120;

//Информация о точке касания (необходимо для обработчика удаления касания)
struct ContactPointInfo
{
  Scene*         scene;  //сцена
  CollisionEvent event;  //событие
};

bool contact_added_callback (btManifoldPoint& contact_point,
                             const btCollisionObject* object0,
                             int part_id0,
                             int index0,
                             const btCollisionObject* object1,
                             int part_id1,
                             int index1)
{
  RigidBodyInfo *body0_info = (RigidBodyInfo*)object0->getUserPointer (),
                *body1_info = (RigidBodyInfo*)object1->getUserPointer ();

  ContactPointInfo *contact_point_info;

  if (contact_point.m_userPersistentData)
  {
    contact_point_info = (ContactPointInfo*)contact_point.m_userPersistentData;

    contact_point_info->event.type = CollisionEventType_Process;
  }
  else
  {
    contact_point_info = new ContactPointInfo;

    contact_point_info->event.type     = CollisionEventType_Begin;
    contact_point_info->scene          = body0_info->scene;
    contact_point_info->event.body [0] = body0_info->body;
    contact_point_info->event.body [1] = body1_info->body;
  }

  vector_from_bullet_vector (contact_point.getPositionWorldOnA (), contact_point_info->event.point);

  try
  {
    body0_info->scene->ColissionNotify (contact_point_info->event);
  }
  catch (...)
  {
    //подавление исключений
  }

  contact_point.m_userPersistentData = contact_point_info;

  return false;
}

bool contact_destroyed_callback (void* user_persistent_data)
{
  ContactPointInfo* contact_point_info = (ContactPointInfo*)user_persistent_data;

  contact_point_info->event.type = CollisionEventType_End;

  try
  {
    contact_point_info->scene->ColissionNotify (contact_point_info->event);
  }
  catch (...)
  {
    //подавление исключений
  }

  delete contact_point_info;

  return false;
}

void check_create_joint_arguments (const char* source, IRigidBody* body1, IRigidBody* body2, RigidBody*& casted_body1, RigidBody*& casted_body2)
{
  if (!body1)
    throw xtl::make_null_argument_exception (source, "body1");

  if (!body2)
    throw xtl::make_null_argument_exception (source, "body2");

  casted_body1 = cast_object<RigidBody, IRigidBody> (body1, source, "body1");
  casted_body2 = cast_object<RigidBody, IRigidBody> (body2, source, "body1");
}

struct CollisionFilterDesc
{
  IScene::BroadphaseCollisionFilter filter;     //фильтр
  bool                              collides;   //режим фильтрации

  CollisionFilterDesc ()
    : collides (true)
    {}

  CollisionFilterDesc (const IScene::BroadphaseCollisionFilter& in_filter, bool in_collides)
    : filter (in_filter), collides (in_collides)
    {}
};

//Класс обрабатывающий результаты трассировки луча
class BulletRayTestCallback : public btCollisionWorld::RayResultCallback
{
  public:
    //Конструктор
    BulletRayTestCallback (const math::vec3f& in_ray_origin, const math::vec3f& in_ray_end, RayTestMode in_mode, const physics::low_level::IScene::RayTestCallback& in_callback)
      : callback (in_callback)
      , ray_origin (in_ray_origin)
      , ray_end (in_ray_end)
      , mode (in_mode)
      , farthest_hit_fraction (0.f)
      , collision_groups_count (0)
      , result_body (0)
      {}

    //Вызов callback если было обнаружено пересечение
    void NotifyCallback ()
    {
      if (!result_body)
        return;

      callback (result_body, result_position, result_normal);
    }

    //Установка групп коллизии для фильтрации
    void SetCollisionGroups (size_t count, const size_t* groups)
    {
      collision_groups_count = count;
      collision_groups       = groups;
    }

    //Проверка нужно ли тестировать пересечение
    bool needsCollision (btBroadphaseProxy* proxy) const
    {
      if (!collision_groups_count)
        return true;

      RigidBody *body = ((RigidBodyInfo*)((btCollisionObject*)proxy->m_clientObject)->getUserPointer ())->body;

      size_t group = body->CollisionGroup ();

      for (size_t i = 0; i < collision_groups_count; i++)
        if (group == collision_groups [i])
          return true;

      return false;
    }

    //Обнаружено пересечение
    btScalar addSingleResult (btCollisionWorld::LocalRayResult& ray_result, bool normal_in_world_space)
    {
      bool update_result = true;

      switch (mode)
      {
        case RayTestMode_Closest:
          m_closestHitFraction = ray_result.m_hitFraction;
          break;
        case RayTestMode_Farthest:
          if (ray_result.m_hitFraction > farthest_hit_fraction)
            farthest_hit_fraction = ray_result.m_hitFraction;
          else
            update_result = false;
          break;
        default:
          break;
      }

      if (update_result)
      {
        btCollisionObject* collision_object = ray_result.m_collisionObject;

        RigidBody *body = ((RigidBodyInfo*)collision_object->getUserPointer ())->body;

        result_body     = body;
        result_position = (1 - ray_result.m_hitFraction) * ray_origin + ray_result.m_hitFraction * ray_end;

        if (normal_in_world_space)
        {
          vector_from_bullet_vector (ray_result.m_hitNormalLocal, result_normal);
        }
        else
        {
          ///need to transform normal into worldspace
          vector_from_bullet_vector (collision_object->getWorldTransform ().getBasis () * ray_result.m_hitNormalLocal, result_normal);
        }

        if (mode == RayTestMode_All)
          callback (body, result_position, result_normal);
      }

      return m_closestHitFraction;
    }

  private:
    const IScene::RayTestCallback& callback;                //колбек
    math::vec3f                    ray_origin;              //точка начала луча
    math::vec3f                    ray_end;                 //точка конца луча
    RayTestMode                    mode;                    //режим тестирования
    float                          farthest_hit_fraction;   //самое дальнее пересечение
    size_t                         collision_groups_count;  //количество груп коллизии
    const size_t*                  collision_groups;        //группы коллизии, с которыми проверяется пересечение луча
    RigidBody*                     result_body;             //пересеченное тело
    math::vec3f                    result_position;         //точка пересечения
    math::vec3f                    result_normal;           //нормаль в точке пересечения
};

//Класс реализующий отладочную отрисовку
class DebugDrawer : public btIDebugDraw
{
  public:
    DebugDrawer (render::debug::PrimitiveRender& in_render)
      : log (DEBUG_DRAWER_LOG_NAME), render (in_render), debug_mode (btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE)
      {}

    void drawLine (const btVector3& from, const btVector3& to, const btVector3& from_color, const btVector3& to_color)
    {
      render::debug::RenderablePoint vertices [2];

      vector_from_bullet_vector (from, vertices [0].position);
      vector_from_bullet_vector (to, vertices [1].position);

      vertices [0].color = math::vec4f (from_color.x (), from_color.y (), from_color.z (), 1.f);
      vertices [1].color = math::vec4f (to_color.x (), to_color.y (), to_color.z (), 1.f);

      render.DrawLine (vertices [0], vertices [1]);
    }

    void drawBox (const btVector3& box_min, const btVector3& box_max, const btVector3& bt_color, btScalar alpha)
    {
      math::vec3f min, max;

      vector_from_bullet_vector (box_min, min);
      vector_from_bullet_vector (box_max, max);

      render.DrawWireBox (min, max, math::vec4f (bt_color.x (), bt_color.y (), bt_color.z (), alpha));
    }

    void drawSphere (const btVector3& p, btScalar radius, const btVector3& bt_color)
    {
      math::vec3f position;

      vector_from_bullet_vector (p, position);

      math::vec4f color (bt_color.x (), bt_color.y (), bt_color.z (), 1.f);

      render.DrawWireSphere (position, radius, color);
    }

    void drawLine (const btVector3& from, const btVector3& to, const btVector3& color)
    {
      drawLine (from, to, color, color);
    }

    void drawTriangle (const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& /*n0*/, const btVector3& /*n1*/, const btVector3& /*n2*/, const btVector3& color, btScalar alpha)
    {
      drawTriangle (v0, v1, v2, color, alpha);
    }

    void drawTriangle (const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& bt_color, btScalar alpha)
    {
      render::debug::RenderablePoint vertices [3];

      math::vec4f color (bt_color.x (), bt_color.y (), bt_color.z (), alpha);

      vector_from_bullet_vector (v0, vertices [0].position);
      vector_from_bullet_vector (v1, vertices [1].position);
      vector_from_bullet_vector (v2, vertices [2].position);

      vertices [0].color = color;
      vertices [1].color = color;
      vertices [2].color = color;

      render.DrawTriangle (vertices);
    }

    void drawContactPoint (const btVector3& point_on_b, const btVector3& normal_on_b, btScalar distance, int life_time, const btVector3& color)
    {
      drawLine (point_on_b, point_on_b + normal_on_b.normalized () * distance, color);
    }

    void reportErrorWarning (const char* warning)
    {
      log.Print (warning);
    }

    void draw3dText (const btVector3& location, const char* text)
    {
      math::vec3f position;

      vector_from_bullet_vector (location, position);

      render.DrawText (text, position, DRAW_TEXT_COLOR);
    }

    void setDebugMode (int in_debug_mode)
    {
      debug_mode = in_debug_mode;
    }

    int getDebugMode() const
    {
      return debug_mode;
    }

  private:
    common::Log                     log;
    render::debug::PrimitiveRender& render;
    int                             debug_mode;
};

}

typedef stl::hash_map<RigidBody*, xtl::auto_connection>        BodyDestroyConnectionsMap;
typedef stl::hash_map<Joint*, xtl::auto_connection>            JointDestroyConnectionsMap;
typedef stl::pair<size_t, size_t>                              CollisionGroupPair;
typedef stl::hash_map<CollisionGroupPair, CollisionFilterDesc> CollisionFiltersMap;
typedef xtl::signal<void (const CollisionEvent&)>              CollisionSignal;

/*
    Описание реализации физической сцены
*/

struct Scene::Impl : public btOverlapFilterCallback
{
  BroadphaseCollisionFilter           default_collision_filter;   //фильтр коллизий по умолчанию
  btDefaultCollisionConfiguration     *collision_configuration;   //конфигурация обработчика коллизий
  btCollisionDispatcher               *collision_dispatcher;      //обработчик коллизий
  btBroadphaseInterface               *broadphase_interface;      //обработчик фазы broadphase
  btSequentialImpulseConstraintSolver *solver;                    //рассчетчик соединений
  btDiscreteDynamicsWorld             *dynamics_world;            //физический мир
  math::vec3f                         gravity;                    //гравитация
  float                               simulation_step;            //шаг симуляции
  BodyDestroyConnectionsMap           body_destroy_connections;   //соединения удаления тел
  JointDestroyConnectionsMap          joint_destroy_connections;  //соединения удаления соединений тел
  CollisionFiltersMap                 collision_filters_map;      //карта фильтров коллизий (ключ - пара номеров групп, первое значение не больше второго)
  CollisionSignal                     collision_signals [CollisionEventType_Num]; //сигналы обработки коллизий

  Impl ()
  {
    collision_configuration = new btDefaultCollisionConfiguration ();
    collision_dispatcher    = new btCollisionDispatcher (collision_configuration);

    broadphase_interface = new btDbvtBroadphase ();

    solver = new btSequentialImpulseConstraintSolver ();

    dynamics_world = new btDiscreteDynamicsWorld (collision_dispatcher, broadphase_interface, solver, collision_configuration);

    dynamics_world->getPairCache ()->setOverlapFilterCallback (this);
  }

  ~Impl ()
  {
    delete dynamics_world;
    delete solver;
    delete broadphase_interface;
    delete collision_dispatcher;
    delete collision_configuration;
  }

  void OnRigidBodyDestroy (RigidBody* body)
  {
    dynamics_world->removeRigidBody (body->BulletRigidBody ());

    body_destroy_connections.erase (body);
  }

  void OnJointDestroy (Joint* joint)
  {
    dynamics_world->removeConstraint (joint->BulletJoint ());

    joint_destroy_connections.erase (joint);
  }

  Joint* AddJoint (RigidBody* body1, RigidBody* body2, btTypedConstraint* joint)
  {
    Joint* return_value = new Joint (body1, body2, joint);

    joint_destroy_connections [return_value] = return_value->RegisterDestroyHandler (xtl::bind (&Scene::Impl::OnJointDestroy, this, _1));

    dynamics_world->addConstraint (joint);

    return return_value;
  }

  bool needBroadphaseCollision (btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
  {
    RigidBody *body0 = ((RigidBodyInfo*)((btCollisionObject*)proxy0->m_clientObject)->getUserPointer ())->body,
              *body1 = ((RigidBodyInfo*)((btCollisionObject*)proxy1->m_clientObject)->getUserPointer ())->body;

    size_t group0 = body0->CollisionGroup (),
           group1 = body1->CollisionGroup ();

    if (group0 > group1)
    {
      size_t temp = group0;

      group0 = group1;
      group1 = temp;
    }

    CollisionFiltersMap::const_iterator iter = collision_filters_map.find (CollisionGroupPair (group0, group1));

    if (iter == collision_filters_map.end ())
    {
      if (!default_collision_filter)
        return true;
      else
        return default_collision_filter (body0, body1);
    }

    if (!iter->second.filter)
      return iter->second.collides;

    return !(iter->second.filter (body0, body1) ^ iter->second.collides);
  }
};

/*
    Конструктор
*/

Scene::Scene ()
  : impl (new Impl)
{
  if (!gContactAddedCallback)
  {
    gContactAddedCallback     = contact_added_callback;
    gContactDestroyedCallback = contact_destroyed_callback;
  }

  SetGravity (DEFAULT_GRAVITY);
  SetSimulationStep (DEFAULT_SIMULATION_STEP);
}

/*
   Управление гравитацией
*/

const math::vec3f& Scene::Gravity ()
{
  return impl->gravity;
}

void Scene::SetGravity (const math::vec3f& value)
{
  impl->gravity = value;

  btVector3 new_gravity (value.x, value.y, value.z);

  impl->dynamics_world->setGravity (new_gravity);
}

/*
   Симуляция
*/

float Scene::SimulationStep ()
{
  return impl->simulation_step;
}

void Scene::SetSimulationStep (float step)
{
  impl->simulation_step = step;
}

void Scene::PerformSimulation (float dt)
{
  impl->dynamics_world->stepSimulation (dt, MAX_SIMULATION_SUBSTEPS, impl->simulation_step);
}

/*
   Создание тел в сцене (тела, обладающие нулевой массой, являются статическими)
*/

RigidBody* Scene::CreateRigidBody (IShape* shape, float mass)
{
  static const char* METHOD_NAME = "physics::low_level::bullet::Scene::CreateRigidBody";

  if (!shape)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shape");

  Shape* bullet_shape = cast_object<Shape, IShape> (shape, METHOD_NAME, "shape");

  RigidBody* return_value = new RigidBody (bullet_shape, mass);

  RigidBodyInfo* body_info = new RigidBodyInfo;

  body_info->body  = return_value;
  body_info->scene = this;

  return_value->BulletRigidBody ()->setUserPointer (body_info);

  impl->body_destroy_connections [return_value] = return_value->RegisterDestroyHandler (xtl::bind (&Scene::Impl::OnRigidBodyDestroy, impl.get (), _1));

  impl->dynamics_world->addRigidBody (return_value->BulletRigidBody ());

  return return_value;
}

/*
   Создание соединений между телами
*/

Joint* Scene::CreateSphericalJoint (IRigidBody* body1, IRigidBody* body2, const SphericalJointDesc& desc)
{
  RigidBody *casted_body1, *casted_body2;

  check_create_joint_arguments ("physics::low_level::bullet::Scene::CreateSphericalJoint", body1, body2, casted_body1, casted_body2);

  btPoint2PointConstraint *joint = new btPoint2PointConstraint (*casted_body1->BulletRigidBody (), *casted_body2->BulletRigidBody (),
      btVector3 (desc.anchor [0].x, desc.anchor [0].y, desc.anchor [0].z), btVector3 (desc.anchor [1].x, desc.anchor [1].y, desc.anchor [1].z));

  return impl->AddJoint (casted_body1, casted_body2, joint);
}

Joint* Scene::CreateConeTwistJoint (IRigidBody* body1, IRigidBody* body2, const ConeTwistJointDesc& desc)
{
  RigidBody *casted_body1, *casted_body2;

  check_create_joint_arguments ("physics::low_level::bullet::Scene::CreateConeTwistJoint", body1, body2, casted_body1, casted_body2);

  btTransform body1_transform (btQuaternion (btVector3 (desc.axis [0].x, desc.axis [0].y, desc.axis [0].z), 0), btVector3 (desc.anchor [0].x, desc.anchor [0].y, desc.anchor [0].z)),
              body2_transform (btQuaternion (btVector3 (desc.axis [1].x, desc.axis [1].y, desc.axis [1].z), 0), btVector3 (desc.anchor [1].x, desc.anchor [1].y, desc.anchor [1].z));

  btConeTwistConstraint *joint = new btConeTwistConstraint (*casted_body1->BulletRigidBody (), *casted_body2->BulletRigidBody (),
                                                            body1_transform, body2_transform);

  joint->setLimit (desc.swing_limit, desc.swing_limit, desc.twist_limit);

  return impl->AddJoint (casted_body1, casted_body2, joint);
}

Joint* Scene::CreateHingeJoint (IRigidBody* body1, IRigidBody* body2, const HingeJointDesc& desc)
{
  RigidBody *casted_body1, *casted_body2;

  check_create_joint_arguments ("physics::low_level::bullet::Scene::CreateHingeJoint", body1, body2, casted_body1, casted_body2);

  btVector3 body1_axis (desc.axis [0].x, desc.axis [0].y, desc.axis [0].z),
            body2_axis (desc.axis [1].x, desc.axis [1].y, desc.axis [1].z);

  btHingeConstraint *joint = new btHingeConstraint (*casted_body1->BulletRigidBody (), *casted_body2->BulletRigidBody (),
      btVector3 (desc.anchor [0].x, desc.anchor [0].y, desc.anchor [0].z), btVector3 (desc.anchor [1].x, desc.anchor [1].y, desc.anchor [1].z),
      body1_axis, body2_axis);

  return impl->AddJoint (casted_body1, casted_body2, joint);
}

Joint* Scene::CreatePrismaticJoint (IRigidBody* body1, IRigidBody* body2, const PrismaticJointDesc& desc)
{
  RigidBody *casted_body1, *casted_body2;

  check_create_joint_arguments ("physics::low_level::bullet::Scene::CreatePrismaticJoint", body1, body2, casted_body1, casted_body2);

  btTransform body1_transform (btQuaternion (btVector3 (desc.axis [0].x, desc.axis [0].y, desc.axis [0].z), 0), btVector3 (desc.anchor [0].x, desc.anchor [0].y, desc.anchor [0].z)),
              body2_transform (btQuaternion (btVector3 (desc.axis [1].x, desc.axis [1].y, desc.axis [1].z), 0), btVector3 (desc.anchor [1].x, desc.anchor [1].y, desc.anchor [1].z));

  btSliderConstraint *joint = new btSliderConstraint (*casted_body1->BulletRigidBody (), *casted_body2->BulletRigidBody (),
                                                      body1_transform, body2_transform, true);   //??????????последний параметр не ясен?????????

  return impl->AddJoint (casted_body1, casted_body2, joint);
}

/*
   Фильтрация столкновений объектов
*/

void Scene::SetCollisionFilter (size_t group1, size_t group2, bool collides, const BroadphaseCollisionFilter& filter)
{
  if (group1 > group2)
  {
    size_t temp = group1;

    group1 = group2;
    group2 = temp;
  }

  impl->collision_filters_map [CollisionGroupPair (group1, group2)] = CollisionFilterDesc (filter, collides);
}

void Scene::SetDefaultCollisionFilter (const BroadphaseCollisionFilter& filter)
{
  impl->default_collision_filter = filter;
}

/*
   Обработка столкновений объектов
*/

xtl::connection Scene::RegisterCollisionCallback (CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  if (event_type < 0 || event_type >= CollisionEventType_Num)
    throw xtl::make_argument_exception ("physics::low_level::bullet_driver::Scene::RegisterCollisionCallback", "event_type", event_type);

  return impl->collision_signals [event_type].connect (callback_handler);
}

/*
   Трассировка луча, если коллбек возвращает true - поиск пересечений продолжается
*/

void Scene::RayTest (const math::vec3f& ray_origin, const math::vec3f& ray_end, RayTestMode mode, const RayTestCallback& callback_handler)
{
  if (mode < 0 || mode >= RayTestMode_Num)
    throw xtl::make_argument_exception ("physics::low_level::bullet_driver::Scene::RayTest (const math::vec3f&, const math::vec3f&, RayTestMode, const RayTestCallback&)", "mode", mode);

  btVector3 bullet_origin (ray_origin.x, ray_origin.y, ray_origin.z), bullet_direction (ray_end.x, ray_end.y, ray_end.z);

  BulletRayTestCallback callback (ray_origin, ray_end, mode, callback_handler);

  impl->dynamics_world->rayTest (bullet_origin, bullet_direction, callback);

  if (mode != RayTestMode_All)
    callback.NotifyCallback ();
}

void Scene::RayTest (const math::vec3f& ray_origin, const math::vec3f& ray_end, size_t collision_groups_count,
                     const size_t* collision_groups, RayTestMode mode, const RayTestCallback& callback_handler)
{
  static const char* METHOD_NAME = "physics::low_level::bullet_driver::Scene::RayTest (const math::vec3f&, const math::vec3f&, size_t, size_t*, RayTestMode, const RayTestCallback&)";

  if (!collision_groups_count)
    return;

  if (!collision_groups)
    throw xtl::make_null_argument_exception (METHOD_NAME, "collision_groups");

  if (mode < 0 || mode >= RayTestMode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "mode", mode);

  btVector3 bullet_origin (ray_origin.x, ray_origin.y, ray_origin.z), bullet_direction (ray_end.x, ray_end.y, ray_end.z);

  BulletRayTestCallback callback (ray_origin, ray_end, mode, callback_handler);

  callback.SetCollisionGroups (collision_groups_count, collision_groups);

  impl->dynamics_world->rayTest (bullet_origin, bullet_direction, callback);

  if (mode != RayTestMode_All)
    callback.NotifyCallback ();
}

/*
   Отладочная отрисовка
*/

void Scene::Draw (render::debug::PrimitiveRender& render)
{
  DebugDrawer drawer (render);

  impl->dynamics_world->setDebugDrawer (&drawer);

  impl->dynamics_world->debugDrawWorld ();

  impl->dynamics_world->setDebugDrawer (0);
}

/*
   Оповещение о коллизии
*/

void Scene::ColissionNotify (const CollisionEvent& event)
{
  impl->collision_signals [event.type] (event);
}
