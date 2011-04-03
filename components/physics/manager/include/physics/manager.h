#ifndef PHYSICS_MANAGER_HEADER
#define PHYSICS_MANAGER_HEADER

#include <stl/auto_ptr.h>

#include <physics/material.h>
#include <physics/scene.h>
#include <physics/shape.h>

namespace media
{

namespace physics
{

//forward declaration
class Shape;

}

}

namespace physics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер физики
///////////////////////////////////////////////////////////////////////////////////////////////////
class PhysicsManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PhysicsManager (const char* driver_name);
    ~PhysicsManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene    CreateScene         ();
    Material CreateMaterial      ();
    Material CreateMaterial      (const char* name);
    Shape    CreateBoxShape      (const math::vec3f& half_dimensions);
    Shape    CreateSphereShape   (float radius);
    Shape    CreateCapsuleShape  (float radius, float height);
    Shape    CreatePlaneShape    (const math::vec3f& normal, float d);
    Shape    CreateCompoundShape (const ShapeList&);
    Shape    CreateShape         (const media::physics::Shape&, const math::vec3f& scale = 1.f);
    Shape    CreateShape         (const char* name, const math::vec3f& scale = 1.f);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка / выгрузка ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadLibrary   (const char* name);
    void UnloadLibrary (const char* name);

  private:
    PhysicsManager (const PhysicsManager&); //no impl
    PhysicsManager& operator = (const PhysicsManager&); //no impl
  
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

#endif
