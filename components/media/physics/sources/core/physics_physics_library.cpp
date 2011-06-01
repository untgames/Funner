#include "shared.h"

using namespace media::physics;

/*
   Реализация физической библиотеки
*/

PhysicsLibrary::SaveOptions::SaveOptions ()
  : separate_meshes_file (false)
  {}

struct PhysicsLibrary::Impl : public xtl::reference_counter
{
  stl::string            name;            //имя библиотеки
  RigidBodyCollection    rigid_bodies;    //коллекция твердых тел
  MaterialCollection     materials;       //коллекция материалов
  ShapeCollection        shapes;          //коллекция геометрических тел
  TriangleMeshCollection triangle_meshes; //коллекция сеток
};

/*
   Конструкторы / деструктор / присваивание
*/

PhysicsLibrary::PhysicsLibrary ()
  : impl (new Impl)
{
}

PhysicsLibrary::PhysicsLibrary (const PhysicsLibrary& source)
  : impl (source.impl)
{
  addref (impl);
}

PhysicsLibrary::PhysicsLibrary (const char* file_name)
  : impl (new Impl)
{
  try
  {
    Load (file_name);
  }
  catch (xtl::exception& e)
  {
    release (impl);
    e.touch ("media::physics::PhysicsLibrary::PhysicsLibrary (const char*)");
    throw;
  }
  catch (...)
  {
    release (impl);
    throw;
  }
}

PhysicsLibrary::~PhysicsLibrary ()
{
  release (impl);
}

PhysicsLibrary& PhysicsLibrary::operator = (const PhysicsLibrary& source)
{
  PhysicsLibrary (source).Swap (*this);

  return *this;
}
    
/*
   Имя модели
*/

const char* PhysicsLibrary::Name () const
{
  return impl->name.c_str ();
}

void PhysicsLibrary::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::physics::PhysicsLibrary::Rename", name);

  impl->name = name;
}

/*
   Получение коллекций
*/

const PhysicsLibrary::RigidBodyCollection& PhysicsLibrary::RigidBodies () const
{
  return const_cast<PhysicsLibrary&> (*this).RigidBodies ();
}

PhysicsLibrary::RigidBodyCollection& PhysicsLibrary::RigidBodies ()
{
  return impl->rigid_bodies;
}

const PhysicsLibrary::MaterialCollection& PhysicsLibrary::Materials () const
{
  return const_cast<PhysicsLibrary&> (*this).Materials ();
}

PhysicsLibrary::MaterialCollection& PhysicsLibrary::Materials ()
{
  return impl->materials;
}

const PhysicsLibrary::ShapeCollection& PhysicsLibrary::Shapes () const
{
  return const_cast<PhysicsLibrary&> (*this).Shapes ();
}

PhysicsLibrary::ShapeCollection& PhysicsLibrary::Shapes ()
{
  return impl->shapes;
}

const PhysicsLibrary::TriangleMeshCollection& PhysicsLibrary::TriangleMeshes () const
{
  return const_cast<PhysicsLibrary&> (*this).TriangleMeshes ();
}

PhysicsLibrary::TriangleMeshCollection& PhysicsLibrary::TriangleMeshes ()
{
  return impl->triangle_meshes;
}

/*
   Загрузка / сохранение
*/

void PhysicsLibrary::Load (const char* file_name)
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::physics::PhysicsLibrary::Load", "file_name");

  try
  {
    static common::ComponentLoader loader ("media.physics.loaders.*");

    PhysicsLibraryManager::GetLoader (file_name, common::SerializerFindMode_ByName)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::physics::PhysicsLibrary::Load");
    throw;
  }
}

void PhysicsLibrary::Save (const char* file_name, const SaveOptions& options) const
{
  if (!file_name)
    throw xtl::make_null_argument_exception ("media::physics::PhysicsLibrary::Save", "file_name");

  try
  {
    static common::ComponentLoader loader ("media.physics.savers.*");

    PhysicsLibraryManager::GetSaver (file_name, common::SerializerFindMode_ByName)(file_name, options, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::physics::PhysicsLibrary::Save");
    throw;
  }
}

/*
   Обмен
*/

void PhysicsLibrary::Swap (PhysicsLibrary& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace physics
{

/*
   Обмен
*/

void swap (PhysicsLibrary& library1, PhysicsLibrary& library2)
{
  library1.Swap (library2);
}

}

}
