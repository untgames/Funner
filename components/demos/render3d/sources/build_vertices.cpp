#include "shared.h"

namespace
{

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
    
    media::geometry::VertexFormat format;
  
    get_vertex_format (static_cast<TargetVertex*> (0), format);
    
    media::geometry::VertexDeclaration target_declaration (format);
    
    if (!mesh)
      return;
      
    math::mat4f tm = root.ObjectTM (model);
      
    for (size_t i=0, count=mesh->vertex_buffers.size (); i<count; i++)
    {
      media::geometry::VertexBuffer& source_vertex_buffer = mesh->vertex_buffers [i]->source_vertex_buffer;
      media::geometry::VertexStream  target_vertex_stream (source_vertex_buffer, target_declaration);

      TargetVertex* verts = target_vertex_stream.Data<TargetVertex> ();
            
        printf ("HEYEH! verts_count=%u vertex_size=%u verts=%p\n", target_vertex_stream.Size (), 
          target_vertex_stream.VertexSize (), verts);

      if (!verts)
        continue;                

      target_verts.insert (target_verts.end (), &verts->position, &verts->position + target_vertex_stream.Size ());

      for (VertexArray::iterator iter=target_verts.end ()-target_vertex_stream.Size (), end=target_verts.end (); iter!=end; ++iter)
      {
        math::vec3f& v = *iter;

        v = tm * v;
      }
    }    
  }
};

}

void build_vertices (Node& root, Test& test, VertexArray& verts)
{
  VertexBuilder (root, test, verts);
}
