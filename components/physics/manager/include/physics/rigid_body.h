#ifndef PHYSICS_RIGID_BODY_HEADER
#define PHYSICS_RIGID_BODY_HEADER

#include <xtl/functional_fwd>

#include <physics/material.h>
#include <physics/shape.h>

namespace physics
{

//implementation forwards
class RigidBodyImpl;
class RigidBodyImplProvider;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Флаги поведения тела
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RigidBodyFlag
{
  RigidBodyFlag_FrozenPositionX = 1,                   //Не передвигается по оси Х
  RigidBodyFlag_FrozenPositionY = 1 << 1,              //Не передвигается по оси Y
  RigidBodyFlag_FrozenPositionZ = 1 << 2,              //Не передвигается по оси Z
  RigidBodyFlag_FrozenPosition  = RigidBodyFlag_FrozenPositionX | RigidBodyFlag_FrozenPositionY | RigidBodyFlag_FrozenPositionZ,
  RigidBodyFlag_FrozenRotationX = 1 << 3,              //Не вращается вокруг оси Х
  RigidBodyFlag_FrozenRotationY = 1 << 4,              //Не вращается вокруг оси Y
  RigidBodyFlag_FrozenRotationZ = 1 << 5,              //Не вращается вокруг оси Z
  RigidBodyFlag_FrozenRotation  = RigidBodyFlag_FrozenRotationX | RigidBodyFlag_FrozenRotationY | RigidBodyFlag_FrozenRotationZ,
  RigidBodyFlag_Kinematic       = 1 << 6,              //Другие объекты отталкиваются от тела не оказывая на него влияния
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Твердое тело
///////////////////////////////////////////////////////////////////////////////////////////////////
class RigidBody
{
  friend class RigidBodyImplProvider;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / копирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    RigidBody  (const RigidBody&);
    ~RigidBody ();

    RigidBody& operator = (const RigidBody&);        

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Масса
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Mass    () const;
    void  SetMass (float mass);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление тензором
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f&  MassSpaceInertiaTensor    () const;
    void                SetMassSpaceInertiaTensor (const math::vec3f& tensor);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Мировое положение
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Transform& WorldTransform    () const;
    void             SetWorldTransform (const Transform& transform);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление сном
///////////////////////////////////////////////////////////////////////////////////////////////////
    float SleepLinearVelocity     () const;
    float SleepAngularVelocity    () const;
    void  SetSleepLinearVelocity  (float value);
    void  SetSleepAngularVelocity (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление пороговым значением скорости перехода в режим непрерывного движения
///////////////////////////////////////////////////////////////////////////////////////////////////
    float CcdMotionThreshold    () const;
    void  SetCcdMotionThreshold (float);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Геометрическое тело
///////////////////////////////////////////////////////////////////////////////////////////////////
    const physics::Shape& Shape () const;
          physics::Shape& Shape ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Материал
///////////////////////////////////////////////////////////////////////////////////////////////////
    const physics::Material& Material    () const;
          physics::Material& Material    ();
    void                     SetMaterial (const physics::Material& material);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Флаги поведения
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Flags       () const;
    void   SetFlags    (size_t flags);
    size_t CheckFlags  (size_t flags) const { return Flags () & flags; }
    void   ChangeFlags (size_t flags, bool state);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Согласование объекта с группой
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* CollisionGroup    () const;
    void        SetCollisionGroup (const char*); 

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приложение силы/импульса/вращения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddForce         (const math::vec3f& force,   const math::vec3f& relative_position = 0);
    void AddImpulse       (const math::vec3f& impulse, const math::vec3f& relative_position = 0);
    void AddTorque        (const math::vec3f& torque);
    void AddTorqueImpulse (const math::vec3f& torque);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление линейной/угловой скоростями
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& LinearVelocity     () const;
    const math::vec3f& AngularVelocity    () const;
    void               SetLinearVelocity  (const math::vec3f& velocity);
    void               SetAngularVelocity (const math::vec3f& velocity);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение об изменении положения тела
///////////////////////////////////////////////////////////////////////////////////////////////////    
    typedef xtl::function<void (const RigidBody& body)> TransformUpdateCallback;

    xtl::connection RegisterTransformUpdateCallback (const TransformUpdateCallback& callback_handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка столкновений объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterCollisionCallback (const char* group_mask, CollisionEventType event_type, const CollisionCallback& callback_handler);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RigidBody&);

  private:
    RigidBody (RigidBodyImpl* impl);

  private:
    RigidBodyImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RigidBody&, RigidBody&);

}

#endif
