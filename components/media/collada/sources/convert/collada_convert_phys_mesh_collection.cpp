#include "shared.h"

namespace
{

/*
    ѕреобразователь данных коллады
*/

class Converter
{
  public:
    /// онструктор
    Converter (const media::collada::Model& in_model, media::physics::PhysicsLibrary& library)
      : model (in_model)
      , mesh_collection (library.TriangleMeshes ())
    {
      ConvertMeshLibrary (model.Meshes ());
    }

    ///ѕреобразование библиотеки
    void ConvertMeshLibrary (const media::collada::MeshLibrary& library)
    {
      for (media::collada::MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
        ConvertMesh (library.ItemId (i), *i);
    }

    ///ѕреобразование поверхности
    void ConvertSurface (const media::collada::Surface& src_surface, media::physics::shapes::TriangleMesh& dst_mesh, size_t first_vertex, size_t first_index)
    {
        //преобразование массивов поверхности
      size_t                        vertices_count = src_surface.VerticesCount ();
      const media::collada::Vertex* src_vertex     = src_surface.Vertices ();
      math::vec3f*                  dst_vertex     = dst_mesh.Vertices () + first_vertex;

      for (size_t i = 0; i < vertices_count; i++, src_vertex++, dst_vertex++)
        *dst_vertex = src_vertex->coord;

      size_t        indices_count = src_surface.IndicesCount ();
      const size_t* src_index     = src_surface.Indices ();
      size_t*       dst_index     = dst_mesh.Triangles () [0] + first_index;

      for (size_t i = 0; i < indices_count; i++, src_index++, dst_index++)
        *dst_index = *src_index + first_vertex;
    }    
    
    ///ѕреобразование меша
    void ConvertMesh (const char* id, const media::collada::Mesh& src_mesh)
    {
      static const char* METHOD_NAME = "media::collada::Converter::ConvertMesh";

        //создание меша

      media::physics::shapes::TriangleMesh dst_mesh;
      
      dst_mesh.Rename (id);

        //резервирование пам€ти дл€ хранени€ индексов
        
      size_t vertices_count = 0, indices_count = 0;
      
      for (media::collada::Mesh::SurfaceList::ConstIterator iter = src_mesh.Surfaces ().CreateIterator (); iter; ++iter)
      {
        if (iter->PrimitiveType () != media::collada::PrimitiveType_TriangleList)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't convert surface from mesh '%s' with primitive type %d, only trinalge list supported", id, iter->PrimitiveType ());

        if (iter->IndicesCount () % 3)
          throw xtl::format_operation_exception (METHOD_NAME, "Can't convert surface from mesh '%s' with indices count %d, indices count must divide by 3", id, iter->IndicesCount ());
        
        vertices_count += iter->VerticesCount ();
        indices_count  += iter->IndicesCount ();
      }

      dst_mesh.SetVerticesCount  (vertices_count);
      dst_mesh.SetTrianglesCount (indices_count / 3);

        //преобразование поверхностей

      size_t first_vertex = 0, first_index = 0;

      for (media::collada::Mesh::SurfaceList::ConstIterator iter = src_mesh.Surfaces ().CreateIterator (); iter; ++iter)
      {
        ConvertSurface (*iter, dst_mesh, first_vertex, first_index);

        first_vertex += iter->VerticesCount ();
        first_index  += iter->IndicesCount ();
      }

        //регистраци€ меша

      mesh_collection.Attach (id, dst_mesh);
    }    

  private:
    const media::collada::Model&                             model;           //исходна€ коллада-модель
    media::physics::PhysicsLibrary::TriangleMeshCollection&  mesh_collection; //результирующа€ коллекци€ мешей
};

}

namespace media
{

namespace collada
{

///ѕреобразование коллада-модели в библиотеку мешей
void convert_triangle_meshes (const collada::Model& source, physics::PhysicsLibrary& destination)
{
  Converter (source, destination);
}

}

}
