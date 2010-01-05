#ifndef PHYSICS_LOW_LEVEL_SCENE_HEADER
#define PHYSICS_LOW_LEVEL_SCENE_HEADER

#include <xtl/functional_fwd>

#include <math/matrix.h>
#include <math/vector.h>

namespace physics
{

namespace low_level
{

//forward declarations
class IJoint;
class IDebugRenderable;
class IRigidBody;
class IShape;

enum CollisionEvent
{
  CollisionEvent_Begin,
  CollisionEvent_Process,
  CollisionEvent_End
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройки создания конического соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ConeTwistJointDesc
{
  IRigidBody* body1;
  IRigidBody* body2;
  math::vec3f body1_anchor;
  math::vec3f body2_anchor;
  math::vec3f body1_axis;
  math::vec3f body2_axis;
  float       swing_limit;
  float       twist_limit;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Физическая сцена
///////////////////////////////////////////////////////////////////////////////////////////////////
class IScene : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение данных для отладочной отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IDebugRenderable* DebugRenderable () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление гравитацией
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const math::vec3f& Gravity    () = 0;
    virtual void               SetGravity (const math::vec3f&) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Симуляция
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual float SimulationStep    () = 0;
    virtual void  SetSimulationStep (float) = 0;

    virtual void  PerformSimulation (float dt) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание тел в сцене
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IRigidBody* CreateRigidBody (IShape* shape, float mass, const math::mat4f& world_position) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание соединений между телами
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IJoint* CreateSphericalJoint (IRigidBody* body1, IRigidBody* body2, const math::vec3f& body1_anchor, const math::vec3f& body2_anchor) = 0;
    virtual IJoint* CreateConeTwistJoint (const ConeTwistJointDesc&) = 0;
    virtual IJoint* CreateHingeJoint     (IRigidBody* body1, IRigidBody* body2, const math::vec3f& body1_anchor, const math::vec3f& body2_anchor, const math::vec3f& body1_axis, const math::vec3f& body2_axis) = 0;
    virtual IJoint* CreatePrismaticJoint (IRigidBody* body1, IRigidBody* body2, const math::vec3f& body1_anchor, const math::vec3f& body2_anchor, const math::vec3f& body1_axis, const math::vec3f& body2_axis) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Фильтрация столкновений объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<bool (IRigidBody*, IRigidBody*)> BroadphaseCollisionFilter;

    virtual void SetBroadphaseCollisionFilter    (BroadphaseCollisionFilter* filter) = 0;
    virtual void SetCollisionGroupPairProcessing (size_t group1, size_t group2, bool collides) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка столкновений объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (IRigidBody*, IRigidBody*, size_t points_count, math::vec3f* points)> CollisionCallback;

    virtual xtl::connection RegisterCollisionCallback (CollisionEvent, const CollisionCallback&) = 0;
};

}

}

#endif
