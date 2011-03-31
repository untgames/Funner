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

    SubtractCollections (unload_library.RigidBodies (),    library.RigidBodies ());
    SubtractCollections (unload_library.Materials (),      library.Materials ());
    SubtractCollections (unload_library.Shapes (),         library.Shapes ());
    SubtractCollections (unload_library.TriangleMeshes (), library.TriangleMeshes ());
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
    if (!driver_name)
      throw xtl::make_null_argument_exception ("", "driver_name");

    impl->driver = DriverManager::FindDriver (driver_name);

    if (!impl->driver)
      throw xtl::format_operation_exception ("", "Driver '%s' not found", driver_name);
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
  return SceneImplProvider::CreateScene (impl->driver.get (), ScenePtr (impl->driver->CreateScene (), false), impl->library.RigidBodies ());
}

Material PhysicsManager::CreateMaterial ()
{
  return MaterialImplProvider::CreateMaterial (impl->driver.get ());
}

Material PhysicsManager::CreateMaterial (const char* name)
{
  try
  {
    media::physics::Material *media_material = impl->library.Materials ().Find (name);

    if (!media_material)
      throw xtl::format_operation_exception ("", "Material '%s' was not loaded", name);

    return MaterialImplProvider::CreateMaterial (impl->driver.get (), *media_material);
  }
  catch (xtl::exception& e)
  {
    e.touch ("physics::PhysicsManager::CreateMaterial (const char*)");
    throw;
  }
}

Shape PhysicsManager::CreateBoxShape (const math::vec3f& half_dimensions)
{
  return ShapeImplProvider::CreateShape (ShapePtr (impl->driver->CreateBoxShape (half_dimensions), false));
}

Shape PhysicsManager::CreateSphereShape (float radius)
{
  return ShapeImplProvider::CreateShape (ShapePtr (impl->driver->CreateSphereShape (radius), false));
}

Shape PhysicsManager::CreateCapsuleShape (float radius, float height)
{
  return ShapeImplProvider::CreateShape (ShapePtr (impl->driver->CreateCapsuleShape (radius, height), false));
}

Shape PhysicsManager::CreatePlaneShape (const math::vec3f& normal, float d)
{
  return ShapeImplProvider::CreateShape (ShapePtr (impl->driver->CreatePlaneShape (normal, d), false));
}

Shape PhysicsManager::CreateCompoundShape (const ShapeList& shape_list)
{
  try
  {
    return ShapeImplProvider::CreateShape (impl->driver.get (), shape_list);
  }
  catch (xtl::exception& e)
  {
    e.touch ("physics::PhysicsManager::CreateCompoundShape");
    throw;
  }
}

Shape PhysicsManager::CreateShape (const media::physics::Shape& shape)
{
  try
  {
    return ShapeImplProvider::CreateShape (impl->driver.get (), shape);
  }
  catch (xtl::exception& e)
  {
    e.touch ("physics::PhysicsManager::CreateShape");
    throw;
  }
}

Shape PhysicsManager::CreateShape (const char* name)
{
  try
  {
    media::physics::Shape* media_shape = impl->library.Shapes ().Find (name);

    if (!media_shape)
      throw xtl::format_operation_exception ("", "Shape '%s' was not loaded", name);

    return CreateShape (*media_shape);
  }
  catch (xtl::exception& e)
  {
    e.touch ("physics::PhysicsManager::CreateShape (const char*)");
    throw;
  }
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
