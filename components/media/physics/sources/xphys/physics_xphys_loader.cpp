#include "shared.h"

using namespace common;
using namespace media::physics;
using namespace media::physics::shapes;

namespace
{

const char* DEFAULT_GRAVITY         = "0 -9.8 0";
const float DEFAULT_SIMULATION_STEP = 1.f / 60.f;

}

namespace components
{

namespace xphys_loader
{

/*
    Константы
*/

const char* LOG_NAME = "media.physics.XmlPhysicsLibraryLoader"; //имя потока протоколирования

template <unsigned int Size>
void read (Parser::Iterator iter, const char* str, math::vector <float, Size>& value)
{
  StringArray components = common::split (str);

  if (components.Size () != Size)
    raise_parser_exception (*iter, "Invalid vector format");

  for (unsigned int i = 0; i < Size; i++)
    value [i] = (float)atof (components [i]);
}

void read (Parser::Iterator iter, const char* str, math::quatf& value)
{
  StringArray components = common::split (str);

  if (components.Size () != 4)
    raise_parser_exception (*iter, "Invalid quaternion format");

  for (unsigned char i = 0; i < 4; i++)
    value [i] = (float)atof (components [i]);
}

/*
    Загрузчик физической библиотеки в формате Xml
*/

class XmlPhysicsLibraryLoader
{
  private:
      //слияние библиотек
    template <class T>
    void MergeCollections (const media::physics::PhysicsLibraryCollection<T>& source, media::physics::PhysicsLibraryCollection<T>& destination)
    {
      for (typename media::physics::PhysicsLibraryCollection<T>::ConstIterator iter = source.CreateIterator (); iter; ++iter)
        destination.Attach (source.ItemId (iter), *iter);
    }

    void MergeLibraries (const PhysicsLibrary& source_library, PhysicsLibrary& target_library)
    {
      MergeCollections (source_library.RigidBodies (),      target_library.RigidBodies ());
      MergeCollections (source_library.Materials (),        target_library.Materials ());
      MergeCollections (source_library.Shapes (),           target_library.Shapes ());
      MergeCollections (source_library.TriangleMeshes (),   target_library.TriangleMeshes ());
      MergeCollections (source_library.Scenes (),           target_library.Scenes ());
    }

      //разбор подключаемых файлов
    void ParseInclude (Parser::Iterator include_iter)
    {
      try
      {
        MergeLibraries (PhysicsLibrary (get<const char*> (*include_iter, "path")), library);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::physics::XmlPhysicsLibraryLoader::ParseInclude");
        throw;
      }
    }

      //разбор материала
    void ParseMaterial (Parser::Iterator material_iter)
    {
      Material material;

      material.Rename (get<const char*> (*material_iter, "name"));

      if (material_iter->First ("linear_damping"))
        material.SetLinearDamping (get<float> (*material_iter, "linear_damping"));

      if (material_iter->First ("angular_damping"))
        material.SetAngularDamping (get<float> (*material_iter, "angular_damping"));

      if (material_iter->First ("friction"))
        material.SetFriction (get<float> (*material_iter, "friction"));

      if (material_iter->First ("anisotropic_friction"))
      {
        math::vec3f value;

        read (material_iter, get<const char*> (*material_iter, "anisotropic_friction"), value);

        material.SetAnisotropicFriction (value);
      }

      if (material_iter->First ("restitution"))
        material.SetRestitution (get<float> (*material_iter, "restitution"));

      library.Materials ().Attach (material.Name (), material);
    }

      //разбор сетки
    void ParseTriangleMesh (Parser::Iterator mesh_iter)
    {
      static const char* METHOD_NAME = "media::physics::XmlPhysicsLibraryLoader::ParseTriangleMesh";

      TriangleMesh mesh;

      mesh.Rename    (get<const char*> (*mesh_iter, "name"));
      mesh.SetConvex (get<int> (*mesh_iter, "is_convex") != 0);

      ParseNode vertices_node = mesh_iter->First ("vertices");

      unsigned int vertices_count = get<unsigned int> (vertices_node, "count");

      if (!vertices_count)
        throw xtl::format_operation_exception (METHOD_NAME, "Empty vertices in mesh '%s'", mesh.Name ());

      mesh.SetVerticesCount (vertices_count);

      read (vertices_node, "#text", mesh.Vertices (), vertices_count);

      ParseNode indices_node = mesh_iter->First ("triangle_indices");

      if (indices_node)
      {
        unsigned int triangles_count = get<unsigned int> (indices_node, "count");

        if (!triangles_count)
          throw xtl::format_operation_exception (METHOD_NAME, "Empty indices in mesh '%s'", mesh.Name ());

        mesh.SetTrianglesCount (triangles_count);

        read (indices_node, "#text", (unsigned int*)mesh.Triangles (), triangles_count * 3);
      }
      else if (!mesh.IsConvex ())
        throw xtl::format_operation_exception (METHOD_NAME, "Not convex mesh '%s' has no indices", mesh.Name ());

      library.TriangleMeshes ().Attach (mesh.Name (), mesh);
    }

      //разбор геометрического тела
    void ParseShape (Parser::Iterator shape_iter)
    {
      static const char* METHOD_NAME = "media::physics::XmlPhysicsLibrary::ParseShape";

      Shape shape;

      shape.Rename (get<const char*> (*shape_iter, "name"));

      if (shape_iter->First ("margin"))
        shape.SetMargin (get<float> (*shape_iter, "margin"));

      const char* type = get<const char*> (*shape_iter, "type");

      if (!xtl::xstrcmp (type, "box"))
      {
        math::vec3f value;

        read (shape_iter, get<const char*> (*shape_iter, "half_dimensions"), value);

        shape.SetData (Box (value));
      }
      else if (!xtl::xstrcmp (type, "sphere"))
        shape.SetData (Sphere (get<float> (*shape_iter, "radius")));
      else if (!xtl::xstrcmp (type, "capsule"))
        shape.SetData (Capsule (get<float> (*shape_iter, "radius"), get<float> (*shape_iter, "height")));
      else if (!xtl::xstrcmp (type, "plane"))
      {
        math::vec3f value;

        read (shape_iter, get<const char*> (*shape_iter, "normal"), value);

        shape.SetData (Plane (value, get<float> (*shape_iter, "d")));
      }
      else if (!xtl::xstrcmp (type, "triangle_mesh"))
      {
        const char* mesh_name = get<const char*> (*shape_iter, "mesh_name");

        TriangleMesh* mesh = library.TriangleMeshes ().Find (mesh_name);

        if (!mesh)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't find mesh '%s' referenced in shape '%s'", mesh_name, shape.Name ());

        shape.SetData (*mesh);
      }
      else if (!xtl::xstrcmp (type, "compound"))
      {
        Compound compound;

        for (Parser::NamesakeIterator iter = shape_iter->First ("shape_instance"); iter; ++iter)
        {
          const char* shape_name = get<const char*> (*iter, "name");

          Shape* shape_instance = library.Shapes ().Find (shape_name);

          if (!shape_instance)
            throw xtl::format_operation_exception (METHOD_NAME, "Can't find shape '%s' referenced in compound shape '%s'", shape_name, shape.Name ());

          math::vec3f shape_position;

          read (iter, get<const char*> (*iter, "position", "0 0 0"), shape_position);

          math::quatf shape_orientation;

          if (iter->First ("orientation"))
            read (iter, get<const char*> (*iter, "orientation"), shape_orientation);
          else if (iter->First ("rotation"))
          {
            math::vec3f rotation;

            read (iter, get<const char*> (*iter, "rotation", "0 0 0"), rotation);

            shape_orientation = math::to_quat (math::degree (rotation.x), math::degree (rotation.y), math::degree (rotation.z));
          }

          compound.Attach (*shape_instance, shape_position, shape_orientation);
        }

        shape.SetData (compound);
      }
      else
        throw xtl::format_operation_exception (METHOD_NAME, "Unknown shape type '%s' in shape '%s'", type, shape.Name ());

      library.Shapes ().Attach (shape.Name (), shape);
    }

    //разбор твёрдого тела
    void ParseRigidBody (Parser::Iterator body_iter)
    {
      static const char* METHOD_NAME = "media::physics::XmlPhysicsLibraryLoader::ParseRigidBody";

      RigidBody body;

      body.Rename (get<const char*> (*body_iter, "name"));
      body.SetCollisionGroup (get<const char*> (*body_iter, "collision_group", ""));

      if (body_iter->First ("mass"))
        body.SetMass (get<float> (*body_iter, "mass"));

      if (body_iter->First ("mass_space_inertia_tensor"))
      {
        math::vec3f value;

        read (body_iter, get<const char*> (*body_iter, "mass_space_inertia_tensor"), value);

        body.SetMassSpaceInertiaTensor (value);
      }

      if (body_iter->First ("material"))
      {
        const char* material_name = get<const char*> (*body_iter, "material");

        Material* material = library.Materials ().Find (material_name);

        if (!material)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't find material '%s' referenced from rigid body '%s'", material_name, body.Name ());

        body.SetMaterial (*material);
      }

      const char* shape_name = get<const char*> (*body_iter, "shape");

      Shape* shape = library.Shapes ().Find (shape_name);

      if (!shape)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't find shape '%s' referenced from rigid body '%s'", shape_name, body.Name ());

      body.SetShape (*shape);

      size_t body_flags = 0;

      if (get<int> (*body_iter, "frozen_position_x", 0) != 0)
        body_flags |= RigidBodyFlag_FrozenPositionX;
      if (get<int> (*body_iter, "frozen_position_y", 0) != 0)
        body_flags |= RigidBodyFlag_FrozenPositionY;
      if (get<int> (*body_iter, "frozen_position_z", 0) != 0)
        body_flags |= RigidBodyFlag_FrozenPositionZ;
      if (get<int> (*body_iter, "frozen_position", 0) != 0)
        body_flags |= RigidBodyFlag_FrozenPosition;
      if (get<int> (*body_iter, "frozen_rotation_x", 0) != 0)
        body_flags |= RigidBodyFlag_FrozenRotationX;
      if (get<int> (*body_iter, "frozen_rotation_y", 0) != 0)
        body_flags |= RigidBodyFlag_FrozenRotationY;
      if (get<int> (*body_iter, "frozen_rotation_z", 0) != 0)
        body_flags |= RigidBodyFlag_FrozenRotationZ;
      if (get<int> (*body_iter, "frozen_rotation", 0) != 0)
        body_flags |= RigidBodyFlag_FrozenRotation;
      if (get<int> (*body_iter, "kinematic", 0) != 0)
        body_flags |= RigidBodyFlag_Kinematic;

      body.SetFlags (body_flags);

      library.RigidBodies ().Attach (body.Name (), body);
    }

      //разбор фильтра коллизий
    void ParseCollisionFilter (Parser::Iterator filter_iter, Scene& scene)
    {
      CollisionFilter filter;

      filter.SetGroup1Wildcard (get<const char*> (*filter_iter, "group1_wildcard"));
      filter.SetGroup2Wildcard (get<const char*> (*filter_iter, "group2_wildcard"));
      filter.SetCollides (get<int> (*filter_iter, "collides") != 0);

      scene.CollisionFilters ().Add (filter);
    }

      //разбор сцены
    void ParseScene (Parser::Iterator scene_iter)
    {
      Scene scene;

      scene.Rename (get<const char*> (*scene_iter, "name"));
      scene.SetSimulationStep (get<float> (*scene_iter, "simulation_step", DEFAULT_SIMULATION_STEP));

      math::vec3f value;

      read (scene_iter, get<const char*> (*scene_iter, "gravity", DEFAULT_GRAVITY), value);

      scene.SetGravity (value);


      for_each_child (*scene_iter, "collision_filters.collision_filter", xtl::bind (&XmlPhysicsLibraryLoader::ParseCollisionFilter, this, _1, scene));

      library.Scenes ().Attach (scene.Name (), scene);
    }

      //разбор библиотеки
    void ParseLibrary (Parser::Iterator library_iter)
    {
      for_each_child (*library_iter, "include", xtl::bind (&XmlPhysicsLibraryLoader::ParseInclude, this, _1));
      for_each_child (*library_iter, "materials.material", xtl::bind (&XmlPhysicsLibraryLoader::ParseMaterial, this, _1));
      for_each_child (*library_iter, "triangle_meshes.triangle_mesh", xtl::bind (&XmlPhysicsLibraryLoader::ParseTriangleMesh, this, _1));
      for_each_child (*library_iter, "shapes.shape", xtl::bind (&XmlPhysicsLibraryLoader::ParseShape, this, _1));
      for_each_child (*library_iter, "rigid_bodies.rigid_body", xtl::bind (&XmlPhysicsLibraryLoader::ParseRigidBody, this, _1));
      for_each_child (*library_iter, "scenes.scene", xtl::bind (&XmlPhysicsLibraryLoader::ParseScene, this, _1));
    }

  public:
    XmlPhysicsLibraryLoader (const char* file_name, PhysicsLibrary& out_library)
      : parser (file_name, "xml")
    {
      ParseLibrary (parser.Root ().First ("physics_library"));

        //протоколирование

      Log log (LOG_NAME);

      parser.Log ().Print (xtl::bind (&Log::Print, xtl::ref (log), _1));

      MergeLibraries (library, out_library);
    }
    
  private:
    Parser         parser;   //парсер
    PhysicsLibrary library;  //библиотека в которую ведется чтение данных
};

/*
    Автоматическая регистрация компонента
*/

class XPhysLoaderComponent
{
  public:
    XPhysLoaderComponent ()
    {
      PhysicsLibraryManager::RegisterLoader ("xphys", &LoadLibrary);
    }

  private:
    static void LoadLibrary (const char* file_name, PhysicsLibrary& library)
    {
      XmlPhysicsLibraryLoader (file_name, library);
    }
};

extern "C"
{

ComponentRegistrator<XPhysLoaderComponent> XPhysLoader ("media.physics.loaders.XPhysLoader");

}

}

}
