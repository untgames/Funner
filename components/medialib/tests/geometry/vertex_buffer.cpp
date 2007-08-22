#include "shared.h"

void dump (VertexBuffer& vb)
{
  printf ("Vertex buffer (%u streams, %u vertices, %u weights, vertex_size=%u)\n", vb.StreamsCount (),
          vb.VerticesCount (), vb.Weights ().Size (), vb.VertexSize ());
          
  for (size_t i=0; i<vb.StreamsCount (); i++)
  {
    printf (" stream #%u: vertices_count=%u\n", i, vb.Stream (i).Size ());
    dump (vb.Stream (i).Format ());
  }
}

int main ()
{
  printf ("Results of vertex_buffer_test:\n");
  
  VertexStream vs1 (12, make_vertex_declaration<CustomVertex> ()),
               vs2 (10, make_vertex_declaration<Vertex<Position2f> > ());
  VertexWeightStream weights (12);
  VertexBuffer vb1, vb2;

  vb1.Attach (vs1);
  vb1.Attach (vs2, BufferCloneMode_Copy);
  vb1.AttachWeights (weights);
  
  printf ("Dump vb1\n");
  
  dump (vb1);
  
  printf ("Dump vb2\n");
  
  dump (vb2);  
  
  printf ("vb2 = vb1\n");
  
  vb2 = vb1;  
  
  dump (vb2);
  
  printf ("resize streams\n");
  
  vs1.Resize (9);
  vs2.Resize (8);
  
  printf ("Dump vb1\n");
  
  dump (vb1);
  
  printf ("Dump vb2\n");
  
  dump (vb2);
  
  printf ("detach stream from vb1\n");
  
  vb1.Detach (1);
  
  dump (vb1);
  
  printf ("clear vb2\n");
  
  vb2.Clear ();
  
  dump (vb2);

  return 0;
}
