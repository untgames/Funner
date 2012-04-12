#ifndef MEDIALIB_PHYSICS_RIGID_BODY_HEADER
#define MEDIALIB_PHYSICS_RIGID_BODY_HEADER

#include <math/quat.h>
#include <math/vector.h>

#include <media/physics/material.h>
#include <media/physics/shape.h>

namespace media
{

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RigidBodyFlag
{
  RigidBodyFlag_FrozenPositionX = 1,                   //�� ������������� �� ��� �
  RigidBodyFlag_FrozenPositionY = 1 << 1,              //�� ������������� �� ��� Y
  RigidBodyFlag_FrozenPositionZ = 1 << 2,              //�� ������������� �� ��� Z
  RigidBodyFlag_FrozenPosition  = RigidBodyFlag_FrozenPositionX | RigidBodyFlag_FrozenPositionY | RigidBodyFlag_FrozenPositionZ,
  RigidBodyFlag_FrozenRotationX = 1 << 3,              //�� ��������� ������ ��� �
  RigidBodyFlag_FrozenRotationY = 1 << 4,              //�� ��������� ������ ��� Y
  RigidBodyFlag_FrozenRotationZ = 1 << 5,              //�� ��������� ������ ��� Z
  RigidBodyFlag_FrozenRotation  = RigidBodyFlag_FrozenRotationX | RigidBodyFlag_FrozenRotationY | RigidBodyFlag_FrozenRotationZ,
  RigidBodyFlag_Kinematic       = 1 << 6,              //������ ������� ������������� �� ���� �� �������� �� ���� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class RigidBody
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RigidBody  ();
    RigidBody  (const RigidBody&);
    ~RigidBody ();

    RigidBody& operator = (const RigidBody&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    RigidBody Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Mass    () const;
    void  SetMass (float mass);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::vec3f&  MassSpaceInertiaTensor    () const;
    void                SetMassSpaceInertiaTensor (const math::vec3f& tensor);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const media::physics::Shape& Shape    () const;
    void                         SetShape (const media::physics::Shape& shape);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const media::physics::Material& Material    () const;
    void                            SetMaterial (const media::physics::Material& material);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Flags    () const;
    void   SetFlags (size_t flags);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* CollisionGroup    () const;
    void        SetCollisionGroup (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RigidBody&);

  private:
    struct Impl;
    Impl* impl;

  private:
    RigidBody (Impl* impl);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RigidBody&, RigidBody&);

}

}

#endif
