#include "shared.h"

namespace
{

#pragma pack (1)

typedef media::geometry::Vertex<media::geometry::Position3f> TargetVertex;

struct VertexBuilder: public xtl::visitor<void, scene_graph::VisualModel>
{
  VertexArray& target_verts;
  Test&        test;
  Node&        root;
  
  VertexBuilder (Node& in_root, Test& in_test, VertexArray& in_verts)
    : target_verts (in_verts)
    , test (in_test)
    , root (in_root)
  {
    root.VisitEach (*this);    
  }

  void visit (scene_graph::VisualModel& model)
  {
    ModelMeshPtr mesh = test.mesh_manager.FindMesh (model.MeshName ());

    if (!mesh)
      return;        
    
    media::geometry::VertexFormat format;
  
    get_vertex_format (static_cast<TargetVertex*> (0), format);
    
    media::geometry::VertexDeclaration target_declaration (format);    
      
    math::mat4f tm    = inverse (root.WorldTM ()) * model.WorldTM ();
    math::vec3f scale = 1.0f;//root.WorldScale ();
    
    math::vec3f p = tm * math::vec3f (0.0f);

    media::geometry::Mesh& source_mesh = mesh->source_mesh;
    
    size_t primitives_count = source_mesh.PrimitivesCount ();
    
    for (size_t i=0; i<primitives_count; i++)
    {
      const media::geometry::Primitive& primitive     = source_mesh.Primitive (i);
      media::geometry::VertexBuffer&    vertex_buffer = source_mesh.VertexBuffer (primitive.vertex_buffer);
      media::geometry::IndexBuffer&     index_buffer  = source_mesh.IndexBuffer ();      
      media::geometry::VertexStream     vertex_stream (vertex_buffer, target_declaration);

      TargetVertex* verts = vertex_stream.Data<TargetVertex> ();

      if (!verts)
        continue;

      size_t indices_count = media::geometry::get_points_count (primitive.type, primitive.count);

      target_verts.reserve (target_verts.size () + indices_count);

      size_t* indices = index_buffer.Data ();

//      printf ("indices count: %u\n", indices_count);
      
      for (size_t j=0; j<indices_count; j++)
      {
        if (j >= index_buffer.Size ())
        {
//          printf ("bad index index %u/%u\n", j, indices_count);
          continue;
        }

        if (indices [j] >= vertex_stream.Size ())
        {
//          printf ("bad index %u/%u\n", indices [j], vertex_stream.Size ());
          continue;
        }
            
        math::vec3f v = (tm * verts [indices [j]].position) * scale;
        
        target_verts.push_back (v);
      }
    }

/*    for (size_t i=0, count=mesh->vertex_buffers.size (); i<count; i++)
    {
      media::geometry::VertexBuffer& source_vertex_buffer = mesh->vertex_buffers [i]->source_vertex_buffer;
      media::geometry::VertexStream  target_vertex_stream (source_vertex_buffer, target_declaration);

      TargetVertex* verts = target_vertex_stream.Data<TargetVertex> ();

      if (!verts)
        continue;                

      target_verts.insert (target_verts.end (), &verts->position, &verts->position + target_vertex_stream.Size ());

      for (VertexArray::iterator iter=target_verts.end ()-target_vertex_stream.Size (), end=target_verts.end (); iter!=end; ++iter)
      {
        math::vec3f v = *iter;

        *iter = (tm * v) * scale;
      }
    }    */
  }
};

}

void build_vertices (Node& root, Test& test, VertexArray& verts)
{
  VertexBuilder (root, test, verts);
}

void build_sphere (const VertexArray& verts, math::vec3f& center, float& radius)
{
  float factor = 1.0f / verts.size ();

  for (VertexArray::const_iterator iter=verts.begin (), end=verts.end (); iter!=end; ++iter)
    center += *iter * factor;
    
  math::vec3f max_dim;
  
  for (VertexArray::const_iterator iter=verts.begin (), end=verts.end (); iter!=end; ++iter)
  {
    math::vec3f dim = abs (*iter - center);

    for (int j=0; j<3; j++)
      if (dim [j] > max_dim [j])
        max_dim [j] = dim [j];        
  }  
  
  radius = stl::max (max_dim.x, stl::max (max_dim.y, max_dim.z));
}
