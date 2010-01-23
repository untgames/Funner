#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

namespace
{

const math::vec3f DEFAULT_GRAVITY (0, -9.8f, 0);
const float       DEFAULT_SIMULATION_STEP = 1.f / 60.f;
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

  ContactPointInfo *contact_point_info = new ContactPointInfo;

  contact_point_info->scene          = body0_info->scene;
  contact_point_info->event.type     = CollisionEventType_Begin;
  contact_point_info->event.body [0] = body0_info->body;
  contact_point_info->event.body [1] = body1_info->body;

  vector_from_bullet_vector (contact_point.getPositionWorldOnA (), contact_point_info->event.point);

  body0_info->scene->ColissionNotify (contact_point_info->event);

  contact_point.m_userPersistentData = contact_point_info;

  return false;
}

bool contact_destroyed_callback (void* user_persistent_data)
{
  ContactPointInfo* contact_point_info = (ContactPointInfo*)user_persistent_data;

  contact_point_info->event.type = CollisionEventType_End;

  contact_point_info->scene->ColissionNotify (contact_point_info->event);

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
      return true;

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
   Оповещение о коллизии
*/

void Scene::ColissionNotify (const CollisionEvent& event)
{
  impl->collision_signals [event.type] (event);
}
