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
  stl::auto_ptr<SceneImpl> scene_impl (new SceneImpl (impl->driver->CreateScene ()));

  SceneInternal return_value (scene_impl.get ());

  scene_impl.release ();

  return return_value;
}

Material PhysicsManager::CreateMaterial ()
{
  stl::auto_ptr<MaterialImpl> material_impl (new MaterialImpl (impl->driver.get ()));

  MaterialInternal return_value (material_impl.get ());

  material_impl.release ();

  return return_value;
}

Material PhysicsManager::CreateMaterial (const char* name)
{
  try
  {
    media::physics::Material *media_material = impl->library.Materials ().Find (name);

    if (!media_material)
      throw xtl::format_operation_exception ("", "Material '%s' was not loaded", name);

    stl::auto_ptr<MaterialImpl> material_impl (new MaterialImpl (impl->driver.get ()));

    MaterialInternal return_value (material_impl.get ());

    return_value.SetLinearDamping       (media_material->LinearDamping ());
    return_value.SetAngularDamping      (media_material->AngularDamping ());
    return_value.SetFriction            (media_material->Friction ());
    return_value.SetAnisotropicFriction (media_material->AnisotropicFriction ());
    return_value.SetRestitution         (media_material->Restitution ());

    material_impl.release ();

    return return_value;
  }
  catch (xtl::exception& e)
  {
    e.touch ("physics::PhysicsManager::CreateMaterial (const char*)");
    throw;
  }
}

Shape CreateBoxShape (const math::vec3f& half_dimensions)
{
  throw xtl::make_not_implemented_exception ("physics::PhysicsManager::CreateBoxShape");
}

Shape PhysicsManager::CreateSphereShape (float radius)
{
  throw xtl::make_not_implemented_exception ("physics::PhysicsManager::CreateSphereShape");
}

Shape PhysicsManager::CreateCapsuleShape (float radius, float height)
{
  throw xtl::make_not_implemented_exception ("physics::PhysicsManager::CreateCapsuleShape");
}

Shape PhysicsManager::CreatePlaneShape (const math::vec3f& normal, float d)
{
  throw xtl::make_not_implemented_exception ("physics::PhysicsManager::CreatePlaneShape");
}

Shape PhysicsManager::CreateCompoundShape (const ShapeList& shape_list)
{
  throw xtl::make_not_implemented_exception ("physics::PhysicsManager::CreateCompound");
}

Shape PhysicsManager::CreateShape (const media::physics::Shape& shape)
{
  throw xtl::make_not_implemented_exception ("physics::PhysicsManager::CreateShape");
}

Shape PhysicsManager::CreateShape (const char* name)
{
  throw xtl::make_not_implemented_exception ("physics::PhysicsManager::CreateShape");
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
