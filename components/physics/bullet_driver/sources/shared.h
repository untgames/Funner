#ifndef PHYSICS_BULLET_DRIVER_SHARED_HEADER
#define PHYSICS_BULLET_DRIVER_SHARED_HEADER

#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>

#include <physics/low_level/common.h>
#include <physics/low_level/driver.h>
#include <physics/low_level/joints.h>
#include <physics/low_level/material.h>
#include <physics/low_level/rigid_body.h>
#include <physics/low_level/scene.h>
#include <physics/low_level/shape.h>

#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <LinearMath/btDefaultMotionState.h>

namespace physics
{

namespace low_level
{

namespace bullet
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ ��������� �������������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Object: virtual public IObject, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Object () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    Object (const Object&); //no impl
    Object& operator = (const Object&); //no impl
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Shape : public IShape, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shape (btCollisionShape* shape);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Margin    ();
    void  GetMargin (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� bullet ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    btCollisionShape* BulletCollisionShape ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Material : public IMaterial, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������/������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& LinearDamping     ();
    const math::vec3f& AngularDamping    ();
    void               SetLinearDamping  (const math::vec3f& value);
    void               SetAngularDamping (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Friction               ();
    float AnisotropicFriction    ();
    void  SetFriction            (float value);
    void  SetAnisotropicFriction (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Restitution    ();
    void  SetRestitution (float value);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class RigidBody : public IRigidBody, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RigidBody (IShape* shape, float mass);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bullet::Shape* Shape ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bullet::Material* Material    ();
    void              SetMaterial (IMaterial* material);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Flags    ();
    void   SetFlags (size_t flags);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����/��������/��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddForce   (const math::vec3f& force,   const math::vec3f& relative_position);
    void AddImpulse (const math::vec3f& impulse, const math::vec3f& relative_position);
    void AddTorque  (const math::vec3f& torgue);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������/������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& LinearVelocity     ();
    const math::vec3f& AngularVelocity    ();
    void               SetLinearVelocity  (const math::vec3f& velocity);
    void               SetAngularVelocity (const math::vec3f& velocity);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& CenterOfMassLocalPosition    ();
    void               SetCenterOfMassLocalPosition (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Mass    ();
    void  SetMass (float mass);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f&  MassSpaceInertiaTensor    ();
    void                SetMassSpaceInertiaTensor (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    float SleepLinearVelocity     ();
    float SleepAngularVelocity    ();
    void  SetSleepLinearVelocity  (float value);
    void  SetSleepAngularVelocity (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ��������� �������� �������� � ����� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float CcdMotionThreshold    ();
    void  SetCcdMotionThreshold (float);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ������� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t CollisionGroup    ();
    void   SetCollisionGroup (size_t group_number);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Transform& WorldTransform    ();
    void             SetWorldTransform (const Transform& transform);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� bullet ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    btRigidBody* BulletRigidBody ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
class Joint : public IJoint, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Joint ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t     ObjectsCount ();
    RigidBody* GetObject    (size_t index);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Scene : public IScene, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f& Gravity    ();
    void               SetGravity (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float SimulationStep    ();
    void  SetSimulationStep (float step);

    void  PerformSimulation (float dt);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��� � ����� (����, ���������� ������� ������, �������� ������������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    RigidBody* CreateRigidBody (IShape* shape, float mass);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Joint* CreateSphericalJoint (IRigidBody* body1, IRigidBody* body2, const SphericalJointDesc& desc);
    Joint* CreateConeTwistJoint (IRigidBody* body1, IRigidBody* body2, const ConeTwistJointDesc& desc);
    Joint* CreateHingeJoint     (IRigidBody* body1, IRigidBody* body2, const HingeJointDesc& desc);
    Joint* CreatePrismaticJoint (IRigidBody* body1, IRigidBody* body2, const PrismaticJointDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCollisionFilter (size_t group1, size_t group2, bool collides, const BroadphaseCollisionFilter& filter);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection RegisterCollisionCallback (CollisionEventType event_type, const CollisionCallback& callback_handler);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver : public IDriver, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene* CreateScene ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////
    Shape* CreateBoxShape          (const math::vec3f& half_dimensions);
    Shape* CreateSphereShape       (float radius);
    Shape* CreateCapsuleShape      (float radius, float height);
    Shape* CreatePlaneShape        (const math::vec3f& normal, float d);
    Shape* CreateConvexShape       (size_t vertices_count, math::vec3f* vertices);
    Shape* CreateTriangleMeshShape (size_t vertices_count, math::vec3f* vertices, size_t triangles_count, int* triangles);
    Shape* CreateCompoundShape     (size_t shapes_count, IShape* shapes, Transform* local_transforms);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
