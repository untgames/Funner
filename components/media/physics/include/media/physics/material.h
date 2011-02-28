#ifndef MEDIALIB_PHYSICS_MATERIAL_HEADER
#define MEDIALIB_PHYSICS_MATERIAL_HEADER

#include <cstddef>

#include <math/vector.h>

namespace media
{

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Material
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material  ();
    Material  (const Material&);
    ~Material ();

    Material& operator = (const Material&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������/������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float LinearDamping     () const;
    float AngularDamping    () const;
    void  SetLinearDamping  (float value);
    void  SetAngularDamping (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float              Friction               () const;
    const math::vec3f& AnisotropicFriction    () const;
    void               SetFriction            (float value);
    void               SetAnisotropicFriction (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Restitution    () const;
    void  SetRestitution (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Material&);

  private:
    struct Impl;

    Material (Impl* impl);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Material&, Material&);

}

}

#endif
