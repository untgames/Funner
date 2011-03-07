#include "shared.h"

using namespace common;
using namespace media::physics;
using namespace media::physics::shapes;

namespace
{

/*
    Константы
*/

const char* FLOAT_FORMAT = ".000"; //количество знаков после запятой при выводе вещественных чисел

/*
    Класс, сохраняющий физическую библиотеку в Xml-формате
*/

class XmlPhysicsLibrarySaver
{
  private:
    //Сохранение материала
    void Save (const Material& material)
    {
      XmlWriter::Scope library_scope (writer, "material");

      writer.WriteAttribute ("name", material.Name ());
      writer.WriteAttribute ("linear_damping", material.LinearDamping (), FLOAT_FORMAT);
      writer.WriteAttribute ("angular_damping", material.AngularDamping (), FLOAT_FORMAT);
      writer.WriteAttribute ("friction", material.Friction (), FLOAT_FORMAT);
      writer.WriteAttribute ("anisotropic_friction", material.AnisotropicFriction (), FLOAT_FORMAT);
      writer.WriteAttribute ("restitution", material.Restitution (), FLOAT_FORMAT);
    }

    //Сохранение сетки
    void Save (const TriangleMesh& mesh)
    {
      XmlWriter::Scope library_scope (writer, "triangle_mesh");

      writer.WriteAttribute ("name", mesh.Name ());
      writer.WriteAttribute ("is_convex", mesh.IsConvex () ? "1" : "0");

      if (mesh.VerticesCount ())
      {
        XmlWriter::Scope library_scope (writer, "vertices");

        writer.WriteAttribute ("count", mesh.VerticesCount ());
        writer.WriteData (xtl::make_iterator_range (mesh.VerticesCount (), mesh.Vertices ()));
      }

      if (mesh.TrianglesCount ())
      {
        XmlWriter::Scope library_scope (writer, "triangle_indices");

        writer.WriteAttribute ("count", mesh.TrianglesCount ());
        writer.WriteData (xtl::make_iterator_range (mesh.TrianglesCount () * 3, (size_t*)mesh.Triangles ()));
      }
    }

    const char* ShapeTypeName (ShapeType type)
    {
      switch (type)
      {
        case ShapeType_Box:          return "box";
        case ShapeType_Sphere:       return "sphere";
        case ShapeType_Capsule:      return "capsule";
        case ShapeType_Plane:        return "plane";
        case ShapeType_TriangleMesh: return "triangle_mesh";
        case ShapeType_Compound:     return "compound";
        default:
          throw xtl::make_argument_exception ("media::physics::XmlPhysicsLibrarySaver::ShapeTypeName", "type", type);
      }
    }

    //Сохранение описания геометрического тела
    void SaveShapeData (const Box& box)
    {
      writer.WriteAttribute ("half_dimensions", box.half_dimensions, FLOAT_FORMAT);
    }

    void SaveShapeData (const Sphere& sphere)
    {
      writer.WriteAttribute ("radius", sphere.radius, FLOAT_FORMAT);
    }

    void SaveShapeData (const Capsule& capsule)
    {
      writer.WriteAttribute ("radius", capsule.radius, FLOAT_FORMAT);
      writer.WriteAttribute ("height", capsule.height, FLOAT_FORMAT);
    }

    void SaveShapeData (const Plane& plane)
    {
      writer.WriteAttribute ("normal", plane.normal, FLOAT_FORMAT);
      writer.WriteAttribute ("d", plane.d, FLOAT_FORMAT);
    }

    void SaveShapeData (const TriangleMesh& mesh)
    {
      writer.WriteAttribute ("mesh_name", mesh.Name ());
    }

    void SaveShapeData (const Compound& compound)
    {
      for (size_t i = 0, count = compound.Size (); i < count; i++)
      {
        XmlWriter::Scope library_scope (writer, "shape_instance");

        writer.WriteAttribute ("name", compound.Shape (i).Name ());
        writer.WriteAttribute ("position", compound.Transform (i).position);
        writer.WriteAttribute ("orientation", compound.Transform (i).orientation);
      }
    }

    //Сохранение геометрического тела
    void Save (const Shape& shape)
    {
      XmlWriter::Scope library_scope (writer, "shape");

      writer.WriteAttribute ("name", shape.Name ());
      writer.WriteAttribute ("margin", shape.Margin (), FLOAT_FORMAT);
      writer.WriteAttribute ("type", ShapeTypeName (shape.Type ()));

      switch (shape.Type ())
      {
        case ShapeType_Box:
          SaveShapeData (shape.Data<Box> ());
          break;
        case ShapeType_Sphere:
          SaveShapeData (shape.Data<Sphere> ());
          break;
        case ShapeType_Capsule:
          SaveShapeData (shape.Data<Capsule> ());
          break;
        case ShapeType_Plane:
          SaveShapeData (shape.Data<Plane> ());
          break;
        case ShapeType_TriangleMesh:
          SaveShapeData (shape.Data<TriangleMesh> ());
          break;
        case ShapeType_Compound:
          SaveShapeData (shape.Data<Compound> ());
          break;
        default:
          throw xtl::make_argument_exception ("media::physics::XmlPhysicsLibrarySaver::Save (const Shape&)", "shape.Type ()", shape.Type ());
      }
    }

    //Сохранение твердого тела
    void Save (const RigidBody& body)
    {
      if (!xtl::xstrlen (body.Shape ().Name ()))
        throw xtl::format_operation_exception ("media::physics::XmlPhysicsLibrarySaver::Save (const RigidBody&)", "Empty shape in body '%s'", body.Name ());

      XmlWriter::Scope library_scope (writer, "rigid_body");

      writer.WriteAttribute ("name", body.Name ());
      writer.WriteAttribute ("mass", body.Mass (), FLOAT_FORMAT);
      writer.WriteAttribute ("center_of_mass_position", body.CenterOfMassPosition (), FLOAT_FORMAT);
      writer.WriteAttribute ("center_of_mass_orientation", body.CenterOfMassOrientation (), FLOAT_FORMAT);
      writer.WriteAttribute ("mass_space_inertia_tensor", body.MassSpaceInertiaTensor (), FLOAT_FORMAT);
      writer.WriteAttribute ("shape", body.Shape ().Name ());
      writer.WriteAttribute ("material", body.Material ().Name ());

      size_t body_flags = body.Flags ();

      if ((body_flags & RigidBodyFlag_FrozenPosition) == RigidBodyFlag_FrozenPosition)
        writer.WriteAttribute ("frozen_position", "1");
      else
      {
        if (body_flags & RigidBodyFlag_FrozenPositionX)
          writer.WriteAttribute ("frozen_position_x", "1");
        if (body_flags & RigidBodyFlag_FrozenPositionY)
          writer.WriteAttribute ("frozen_position_y", "1");
        if (body_flags & RigidBodyFlag_FrozenPositionZ)
          writer.WriteAttribute ("frozen_position_z", "1");
      }

      if ((body_flags & RigidBodyFlag_FrozenRotation) == RigidBodyFlag_FrozenRotation)
        writer.WriteAttribute ("frozen_rotation", "1");
      else
      {
        if (body_flags & RigidBodyFlag_FrozenRotationX)
          writer.WriteAttribute ("frozen_rotation_x", "1");
        if (body_flags & RigidBodyFlag_FrozenRotationY)
          writer.WriteAttribute ("frozen_rotation_y", "1");
        if (body_flags & RigidBodyFlag_FrozenRotationZ)
          writer.WriteAttribute ("frozen_rotation_z", "1");
      }

      if (body_flags & RigidBodyFlag_Kinematic)
        writer.WriteAttribute ("kinematic", "1");
    }

    //Сохранение коллекции
    template <class T>
    void SavePhysicsLibraryCollection (const PhysicsLibraryCollection<T>& collection)
    {
      for (typename PhysicsLibraryCollection <T>::ConstIterator iter = collection.CreateIterator (); iter; ++iter)
        Save (*iter);
    }

    //Сохранение материалов
    void SaveMaterials (const PhysicsLibrary::MaterialCollection& materials)
    {
      XmlWriter::Scope library_scope (writer, "materials");

      SavePhysicsLibraryCollection (materials);
    }
    
    //Сохранение сеток
    void SaveTriangleMeshes (const PhysicsLibrary::TriangleMeshCollection& meshes, const PhysicsLibrary::SaveOptions& options)
    {
      if (options.separate_meshes_file)
      {
        XmlWriter::Scope library_scope (writer, "include");

        writer.WriteAttribute ("path", options.meshes_file_name);

        PhysicsLibrary meshes_library;

        PhysicsLibrary::TriangleMeshCollection& out_meshes = meshes_library.TriangleMeshes ();

        for (PhysicsLibrary::TriangleMeshCollection::ConstIterator iter = meshes.CreateIterator (); iter; ++iter)
          out_meshes.Attach (meshes.ItemId (iter), *iter);

        PhysicsLibrary::SaveOptions meshes_options;

        memset (&meshes_options, 0, sizeof (meshes_options));

        meshes_library.Save (options.meshes_file_name, meshes_options);
      }
      else
      {
        XmlWriter::Scope library_scope (writer, "triangle_meshes");

        SavePhysicsLibraryCollection (meshes);
      }
    }

    //Сохранение геометрических тел
    void SaveShapes (const PhysicsLibrary::ShapeCollection& shapes)
    {
      XmlWriter::Scope library_scope (writer, "shapes");

      SavePhysicsLibraryCollection (shapes);
    }

    //Сохранение твердых тел
    void SaveRigidBodies (const PhysicsLibrary::RigidBodyCollection& bodies)
    {
      XmlWriter::Scope library_scope (writer, "rigid_bodies");

      SavePhysicsLibraryCollection (bodies);
    }

  public:
      //конструктор
    XmlPhysicsLibrarySaver (const char* file_name, const PhysicsLibrary::SaveOptions& options, const PhysicsLibrary& library) : writer (file_name)
    {
      XmlWriter::Scope library_scope (writer, "physics_library");

        //построение коллекции triangle_meshes
      PhysicsLibrary::TriangleMeshCollection triangle_meshes = library.TriangleMeshes (), all_triangle_meshes;


      for (PhysicsLibrary::TriangleMeshCollection::ConstIterator iter = triangle_meshes.CreateIterator (); iter; ++iter)
        all_triangle_meshes.Attach (triangle_meshes.ItemId (iter), *iter);

      for (PhysicsLibrary::ShapeCollection::ConstIterator iter = library.Shapes ().CreateIterator (); iter; ++iter)
      {
        if (iter->Type () != ShapeType_TriangleMesh)
          continue;

        const TriangleMesh& mesh = iter->Data <TriangleMesh> ();

        if (all_triangle_meshes.Find (mesh.Name ()))
          continue;

        all_triangle_meshes.Attach (mesh.Name (), mesh);
      }

      SaveMaterials (library.Materials ());
      SaveTriangleMeshes (triangle_meshes, options);
      SaveShapes (library.Shapes ());
      SaveRigidBodies (library.RigidBodies ());
    }

  private:
    XmlWriter writer; //сериализатор Xml
};

/*
    Автоматическая регистрация компонента
*/

class XPhysSaverComponent
{
  public:
    XPhysSaverComponent ()
    {
      PhysicsLibraryManager::RegisterSaver ("xphys", &SaveLibrary);
    }

  private:
    static void SaveLibrary (const char* file_name, const PhysicsLibrary::SaveOptions& options, const PhysicsLibrary& library)
    {
      XmlPhysicsLibrarySaver (file_name, options, library);
    }
};

}

extern "C"
{

ComponentRegistrator<XPhysSaverComponent> XPhysSaver ("media.physics.savers.XPhysSaver");

}
