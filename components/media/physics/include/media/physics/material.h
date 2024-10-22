#ifndef MEDIALIB_PHYSICS_MATERIAL_HEADER
#define MEDIALIB_PHYSICS_MATERIAL_HEADER

#include <cstddef>

#include <math/vector.h>

namespace media
{

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Материал
///////////////////////////////////////////////////////////////////////////////////////////////////
class Material
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material  ();
    Material  (const Material&);
    ~Material ();

    Material& operator = (const Material&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание копии
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя материала
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name   () const;
    void        Rename (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление линейной/угловой аммортизацией
///////////////////////////////////////////////////////////////////////////////////////////////////
    float LinearDamping     () const;
    float AngularDamping    () const;
    void  SetLinearDamping  (float value);
    void  SetAngularDamping (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление трением
///////////////////////////////////////////////////////////////////////////////////////////////////
    float              Friction               () const;
    const math::vec3f& AnisotropicFriction    () const;
    void               SetFriction            (float value);
    void               SetAnisotropicFriction (const math::vec3f& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление упругостью
///////////////////////////////////////////////////////////////////////////////////////////////////
    float Restitution    () const;
    void  SetRestitution (float value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Material&);

  private:
    struct Impl;

    Material (Impl* impl);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Material&, Material&);

}

}

#endif
