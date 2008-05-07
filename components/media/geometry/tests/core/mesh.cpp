#include "shared.h"

void dump (const Mesh& mesh)
{
  printf ("Mesh '%s' (%u vertex_buffers, %u indices, %u primitives)\n", mesh.Name (), mesh.VertexBuffersCount (),
          mesh.IndexBuffer ().Size (), mesh.PrimitivesCount ());
          
  for (size_t i=0; i<mesh.VertexBuffersCount (); i++)
  {
    const VertexBuffer& vb = mesh.VertexBuffer (i);
    
    printf ("  vb#%u: vertices_count=%u, weights_count=%u\n", i, vb.VerticesCount (), vb.Weights ().Size ());
  }

  for (size_t i=0; i<mesh.PrimitivesCount (); i++)
  {
    const Primitive& p = mesh.Primitive (i);
    
    printf ("  primitive #%u: type='%s' vertex_buffer=%u first=%u count=%u material='%s'\n", i, get_type_name (p.type),
            p.vertex_buffer, p.first, p.count, p.material);
  }
}

int main ()
{
  Mesh mesh1;
  
  mesh1.Rename ("mesh1");
  
  VertexStream vs (get_vertex_format<CustomVertex> ());
  VertexBuffer vb;
  
  vb.Attach (vs);  
  
  mesh1.Attach (vb);
  mesh1.Attach (IndexBuffer (12));
  
  vs.Resize (5);
  
  printf ("dump mesh\n");
  
  dump (mesh1);
  
  printf ("add primitives\n");
  
  mesh1.AddPrimitive (PrimitiveType_TriangleList, 0, 12, 5, "my_material");
  mesh1.AddPrimitive (PrimitiveType_TriangleStrip, 0, 14, 8, "my_material1");
  
  dump (mesh1);
  
  printf ("resize vertex stream\n");
  
  vs.Resize (10);
  
  dump (mesh1);
  
  Mesh mesh2;
  
  printf ("copy mesh\n");
  
  mesh2 = mesh1.Clone ();
  
  mesh2.Rename ("mesh2");
  
  dump (mesh2);
  
  printf ("clear mesh\n");
  
  mesh1.Clear ();
  
  dump (mesh1);
  
  printf ("detach mesh buffers\n");
  
  mesh2.DetachAllBuffers ();
  
  dump (mesh2);
  
  printf ("remove primitives\n");
  
  mesh2.RemoveAllPrimitives ();
  
  dump (mesh2);
  
  return 0;
}
