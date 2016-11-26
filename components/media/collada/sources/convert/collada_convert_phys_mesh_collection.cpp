#include "shared.h"

namespace
{

/*
    Преобразователь данных коллады
*/

class Converter
{
  public:
    ///Конструктор
    Converter (const media::collada::Model& in_model, media::physics::PhysicsLibrary& library)
      : model (in_model)
      , mesh_collection (library.TriangleMeshes ())
    {
      ConvertMeshLibrary (model.Meshes ());
    }

    ///Преобразование библиотеки
    void ConvertMeshLibrary (const media::collada::MeshLibrary& library)
    {
      for (media::collada::MeshLibrary::ConstIterator i = library.CreateIterator (); i; ++i)
        ConvertMesh (library.ItemId (i), *i);
    }

    ///Преобразование поверхности
    void ConvertSurface (const media::collada::Surface& src_surface, media::physics::shapes::TriangleMesh& dst_mesh, unsigned int first_vertex, unsigned int first_index)
    {
        //преобразование массивов поверхности
      size_t                        vertices_count = src_surface.VerticesCount ();
      const media::collada::Vertex* src_vertex     = src_surface.Vertices ();
      math::vec3f*                  dst_vertex     = dst_mesh.Vertices () + first_vertex;

      for (size_t i = 0; i < vertices_count; i++, src_vertex++, dst_vertex++)
        *dst_vertex = src_vertex->coord;

      unsigned int        indices_count = src_surface.IndicesCount ();
      const unsigned int* src_index     = src_surface.Indices ();
      unsigned int*       dst_index     = dst_mesh.Triangles () [0] + first_index;

      for (unsigned int i = 0; i < indices_count; i++, src_index++, dst_index++)
        *dst_index = *src_index + first_vertex;
    }    
    
    ///Преобразование меша
    void ConvertMesh (const char* id, const media::collada::Mesh& src_mesh)
    {
      static const char* METHOD_NAME = "media::collada::Converter::ConvertMesh";

        //создание меша

      media::physics::shapes::TriangleMesh dst_mesh;
      
      dst_mesh.Rename (id);

        //резервирование памяти для хранения индексов
        
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

      unsigned int first_vertex = 0, first_index = 0;

      for (media::collada::Mesh::SurfaceList::ConstIterator iter = src_mesh.Surfaces ().CreateIterator (); iter; ++iter)
      {
        ConvertSurface (*iter, dst_mesh, first_vertex, first_index);

        first_vertex += iter->VerticesCount ();
        first_index  += iter->IndicesCount ();
      }

        //регистрация меша

      mesh_collection.Attach (id, dst_mesh);
    }    

  private:
    const media::collada::Model&                             model;           //исходная коллада-модель
    media::physics::PhysicsLibrary::TriangleMeshCollection&  mesh_collection; //результирующая коллекция мешей
};

}

namespace media
{

namespace collada
{

///Преобразование коллада-модели в библиотеку мешей
void convert_triangle_meshes (const collada::Model& source, physics::PhysicsLibrary& destination)
{
  Converter (source, destination);
}

}

}
