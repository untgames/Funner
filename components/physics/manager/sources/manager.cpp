#include "shared.h"

using namespace physics;
using namespace physics::low_level;

/*
   Реализация менеджера физики
*/

typedef xtl::com_ptr<IDriver> DriverPtr;

struct PhysicsManager::Impl
{
  DriverPtr                      driver;   //драйвер
  media::physics::PhysicsLibrary library;  //библиотека описаний объектов

    //слияние коллекций
  template <class T>
  void MergeCollections (const media::physics::PhysicsLibraryCollection<T>& source, media::physics::PhysicsLibraryCollection<T>& destination)
  {
    for (typename media::physics::PhysicsLibraryCollection<T>::ConstIterator iter = source.CreateIterator (); iter; ++iter)
      destination.Attach (source.ItemId (iter), *iter);
  }

    //вычитание коллекций
  template <class T>
  void SubtractCollections (const media::physics::PhysicsLibraryCollection<T>& subtrahend, media::physics::PhysicsLibraryCollection<T>& minuend)
  {
    for (typename media::physics::PhysicsLibraryCollection<T>::ConstIterator iter = subtrahend.CreateIterator (); iter; ++iter)
      minuend.Detach (subtrahend.ItemId (iter));
  }

  ///Загрузка / выгрузка ресурсов
  void LoadLibrary (const char* name)
  {
    media::physics::PhysicsLibrary new_library (name);

    MergeCollections (new_library.RigidBodies (),    library.RigidBodies ());
    MergeCollections (new_library.Materials (),      library.Materials ());
    MergeCollections (new_library.Shapes (),         library.Shapes ());
    MergeCollections (new_library.TriangleMeshes (), library.TriangleMeshes ());
  }

  void UnloadLibrary (const char* name)
  {
    media::physics::PhysicsLibrary unload_library (name);

    SubtractCollection (unload_library.RigidBodies (),    library.RigidBodies ());
    SubtractCollection (unload_library.Materials (),      library.Materials ());
    SubtractCollection (unload_library.Shapes (),         library.Shapes ());
    SubtractCollection (unload_library.TriangleMeshes (), library.TriangleMeshes ());
  }
};

/*
   Конструктор / деструктор
*/

PhysicsManager::PhysicsManager (const char* driver_name)
  : impl (new Impl)
{
  try
  {
    driver = DriverManager::FindDriver (driver_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("physics::PhysicsManager::PhysicsManager");
    throw;
  }
}

PhysicsManager::~PhysicsManager ()
{
}

/*
   Описание
*/

const char* PhysicsManager::Description () const
{
  return impl->driver->GetDescription ();
}
    
/*
   Создание объектов
*/

Scene PhysicsManager::CreateScene ()
{

}

Material PhysicsManager::CreateMaterial ()
{

}

Material PhysicsManager::CreateMaterial (const char* name)
{

}

Shape CreateBoxShape (const math::vec3f& half_dimensions)
{

}

Shape PhysicsManager::CreateSphereShape (float radius)
{

}

Shape PhysicsManager::CreateCapsuleShape (float radius, float height)
{

}

Shape PhysicsManager::CreatePlaneShape (const math::vec3f& normal, float d)
{

}

Shape PhysicsManager::CreateCompoundShape (const ShapeList&)
{

}

Shape PhysicsManager::CreateShape (const media::Shape&)
{

}

Shape PhysicsManager::CreateShape (const char* name)
{

}

/*
   Загрузка / выгрузка ресурсов
*/

void PhysicsManager::LoadLibrary (const char* name)
{
  try
  {
    impl->LoadLibrary (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("physics::PhysicsManager::LoadLibrary");
    throw;
  }
}

void PhysicsManager::UnloadLibrary (const char* name)
{
  try
  {
    impl->UnloadLibrary (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("physics::PhysicsManager::UnloadLibrary");
    throw;
  }
}
