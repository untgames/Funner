#include "shared.h"

typedef Vertex<Position4f, Normalf> Vertex1;
typedef Vertex<TexChannel<0>::Coord2f> Vertex2;

void try_convert (VertexAttributeType src_type, VertexAttributeType dst_type)
{
  try
  {
    VertexFormat src_vf, dst_vf;

    src_vf.AddAttribute (VertexAttributeSemantic_Position, src_type, 0);
    dst_vf.AddAttribute (VertexAttributeSemantic_Position, dst_type, 0);
    
    VertexStream src_vs (src_vf), dst_vs (src_vs, dst_vf);
  }
  catch (std::exception& exception)
  {
    printf ("%s\n", exception.what ());
  }
}

int main ()
{
  try
  {
    printf ("Results of vertex_stream_convert_test:\n");    
    
    VertexStream vs1 (10, make_vertex_declaration<Vertex1> ()),
                 vs2 (10, make_vertex_declaration<Vertex2> ());

    VertexBuffer vb;
    
    vb.Attach (vs1);
    vb.Attach (vs2);  
    
    Vertex1* verts1 = vs1.Data<Vertex1> ();
    Vertex2* verts2 = vs2.Data<Vertex2> ();
    
    if (!verts1 || !verts2)
    {
      printf ("wrong format!\n");
      return 0;
    }
    
    for (size_t i=0, count=vb.VerticesCount (); i<count; i++, verts1++, verts2++)
    {
      verts1->position  = vec3f (float (i), float (i*2), float (i*3), float (i*4));
      verts1->normal    = vec3f (0, 1, 0);
      verts2->texcoord0 = vec2f (float (i), -float (i));
    }  
    
    VertexStream vs3 (vb, make_vertex_declaration<CustomVertex> ());
    
    dump (vs3);
    
    VertexStream vs4 (vb);
    
    dump (vs4.Format ());
    
    printf ("try convert\n");
    
    for (size_t i=0; i<VertexAttributeType_Num; i++)
      for (size_t j=0; j<VertexAttributeType_Num; j++)
        try_convert ((VertexAttributeType)i, (VertexAttributeType)j);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
