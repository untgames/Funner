#include "shared.h"

using namespace media::geometry;
using namespace render::scene;
using namespace render::scene::client;

namespace
{

///Height map
class HeightMap: public VisualModel
{
  public:
///Constructor
    HeightMap (scene_graph::HeightMap& height_map, SceneManager& manager)
      : VisualModel (height_map, manager, interchange::NodeType_DynamicMesh)
      , dynamic_mesh_helper (*this, mesh)
      , need_update_size (true)
      , need_update_vertices (true)
      , position_normal_hash ()
      , color_texcoord_hash ()
      , on_update_sizes_connection (height_map.RegisterEventHandler (scene_graph::HeightMapEvent_OnSizesUpdate, xtl::bind (&HeightMap::UpdateSizesNotify, this)))
      , on_update_vertices_connection (height_map.RegisterEventHandler (scene_graph::HeightMapEvent_OnVerticesUpdate, xtl::bind (&HeightMap::UpdateVerticesNotify, this)))
    {
      media::geometry::IndexBuffer index_buffer (0, media::geometry::IndexType_UInt16);

      mesh.Attach (index_buffer);

      media::geometry::VertexBuffer vertex_buffer;
      media::geometry::VertexFormat position_normal_vertex_format, color_texcoord_vertex_format;

      position_normal_vertex_format.AddAttribute (media::geometry::VertexAttributeSemantic_Position, media::geometry::VertexAttributeType_Float3, 0);
      position_normal_vertex_format.AddAttribute (media::geometry::VertexAttributeSemantic_Normal, media::geometry::VertexAttributeType_Float3, sizeof (float) * 3);
      color_texcoord_vertex_format.AddAttribute (media::geometry::VertexAttributeSemantic_Color, media::geometry::VertexAttributeType_Float4, 0);
      color_texcoord_vertex_format.AddAttribute (media::geometry::VertexAttributeSemantic_TexCoord0, media::geometry::VertexAttributeType_Float2, sizeof (float) * 4);

      media::geometry::VertexDeclaration position_normal_vertex_declaration (position_normal_vertex_format),
                                         color_texcoord_vertex_declaration  (color_texcoord_vertex_format);
      media::geometry::VertexStream      position_normal_vertex_stream      (position_normal_vertex_declaration),
                                         color_texcoord_vertex_stream       (color_texcoord_vertex_declaration);

      vertex_buffer.Attach (position_normal_vertex_stream);
      vertex_buffer.Attach (color_texcoord_vertex_stream);

      mesh.Attach (vertex_buffer);
    }

///Source node
    scene_graph::HeightMap& SourceNode () const { return static_cast<scene_graph::HeightMap&> (Node::SourceNode ()); }

  protected:
///Synchronization implementation
    void UpdateCore (client::Context& context)
    {
      try
      {
        VisualModel::UpdateCore (context);

        //update media mesh
        scene_graph::HeightMap& height_map = SourceNode ();

        bool material_changed = last_material != height_map.Material ();

        if (need_update_size || need_update_vertices || material_changed)
        {
          int vertices_count  = height_map.RowsCount () * height_map.ColumnsCount (),
              triangles_count = (height_map.RowsCount () - 1) * (height_map.ColumnsCount () - 1) * 2,
              indices_count   = triangles_count * 3;

          if (need_update_size)
          {
            media::geometry::IndexBuffer& index_buffer                   = mesh.IndexBuffer ();
            media::geometry::VertexBuffer& vertex_buffer                 = mesh.VertexBuffer (0);
            media::geometry::VertexStream& position_normal_vertex_stream = vertex_buffer.Stream (0),
                                           color_texcoord_vertex_stream  = vertex_buffer.Stream (1);

            index_buffer.Resize (indices_count);
            position_normal_vertex_stream.Resize (vertices_count);
            color_texcoord_vertex_stream.Resize (vertices_count);

            if (vertices_count)
            {
              uint16_t* indices = index_buffer.Data<uint16_t> ();

              for (unsigned int row = 0, rows_count = height_map.RowsCount (), columns_count = height_map.ColumnsCount (), base_index = 0; row < rows_count - 1; row++)
                for (unsigned int column = 0; column < columns_count - 1; column++, base_index += 6)
                {
                  unsigned int top_row_base_vertex    = row * columns_count + column,
                               bottom_row_base_vertex = (row + 1) * columns_count + column;

                  indices[base_index]     = top_row_base_vertex;
                  indices[base_index + 1] = bottom_row_base_vertex;
                  indices[base_index + 2] = top_row_base_vertex + 1;
                  indices[base_index + 3] = top_row_base_vertex + 1;
                  indices[base_index + 4] = bottom_row_base_vertex;
                  indices[base_index + 5] = bottom_row_base_vertex + 1;
                }
            }
          }

          if (need_update_vertices && vertices_count)
          {
            media::geometry::VertexBuffer& vertex_buffer                 = mesh.VertexBuffer (0);
            media::geometry::VertexStream& position_normal_vertex_stream = vertex_buffer.Stream (0),
                                           color_texcoord_vertex_stream  = vertex_buffer.Stream (1);

            float* current_vertex_position_normal = (float*)position_normal_vertex_stream.Data ();
            float* current_vertex_color_texcoord  = (float*)color_texcoord_vertex_stream.Data ();

            const scene_graph::HeightMap::VertexDesc* current_vertex = height_map.Vertices ();

            float row_delimiter    = height_map.RowsCount () - 1,
                  column_delimiter = height_map.ColumnsCount () - 1;

            for (unsigned int row = 0, rows_count = height_map.RowsCount (), columns_count = height_map.ColumnsCount (); row < rows_count; row++)
              for (unsigned int column = 0; column < columns_count; column++, current_vertex_position_normal += 6, current_vertex_color_texcoord += 6, current_vertex++)
              {
                float x = column / column_delimiter,
                      y = row / row_delimiter;

                current_vertex_position_normal[0] = x - 0.5f;
                current_vertex_position_normal[1] = 0.5f - y;
                current_vertex_position_normal[2] = current_vertex->height;
                current_vertex_position_normal[3] = current_vertex->normal.x;
                current_vertex_position_normal[4] = current_vertex->normal.y;
                current_vertex_position_normal[5] = current_vertex->normal.z;

                current_vertex_color_texcoord[0] = current_vertex->color.x;
                current_vertex_color_texcoord[1] = current_vertex->color.y;
                current_vertex_color_texcoord[2] = current_vertex->color.z;
                current_vertex_color_texcoord[3] = current_vertex->color.w;
                current_vertex_color_texcoord[4] = x;
                current_vertex_color_texcoord[5] = 1.f - y;
              }

            size_t new_position_normal_hash = common::crc32 (position_normal_vertex_stream.Data (), position_normal_vertex_stream.Size () * position_normal_vertex_stream.VertexSize ()),
                   new_color_texcoord_hash  = common::crc32 (color_texcoord_vertex_stream.Data (), color_texcoord_vertex_stream.Size () * color_texcoord_vertex_stream.VertexSize ());

            if (position_normal_hash != new_position_normal_hash)
            {
              position_normal_hash = new_position_normal_hash;

              position_normal_vertex_stream.InvalidateData ();
            }

            if (color_texcoord_hash != new_color_texcoord_hash)
            {
              color_texcoord_hash = new_color_texcoord_hash;

              color_texcoord_vertex_stream.InvalidateData ();
            }
          }

          if (need_update_size || material_changed)
          {
            mesh.RemoveAllPrimitives ();

            if (vertices_count)
              mesh.AddPrimitive (PrimitiveType_TriangleList, 0, 0, triangles_count, height_map.Material ());
          }

          need_update_size     = false;
          need_update_vertices = false;

          if (material_changed)
            last_material = height_map.Material ();
        }

        dynamic_mesh_helper.UpdateCore (context, mesh);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::DynamicMesh::UpdateCore");
        throw;
      }
    }

  private:
    void UpdateSizesNotify ()
    {
      need_update_size = true;
    }

    void UpdateVerticesNotify ()
    {
      need_update_vertices = true;
    }

  private:
    media::geometry::Mesh mesh;
    DynamicMeshHelper     dynamic_mesh_helper;
    stl::string           last_material;
    bool                  need_update_size;
    bool                  need_update_vertices;
    size_t                position_normal_hash;
    size_t                color_texcoord_hash;
    xtl::auto_connection  on_update_sizes_connection;
    xtl::auto_connection  on_update_vertices_connection;
};

}

namespace render {
namespace scene {
namespace client {

Node* create_node (scene_graph::HeightMap& height_map, SceneManager& scene_manager)
{
  return new HeightMap (height_map, scene_manager);
}

}}}
