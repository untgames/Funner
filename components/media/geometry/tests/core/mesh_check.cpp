#include "shared.h"

typedef Vertex<CustomVertex, VertexInfluence> MyVertex;

void print (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  try
  {
    Mesh mesh;
    
    mesh.Rename ("mesh1");

    VertexStream vs (2, make_vertex_declaration<MyVertex> ());

    MyVertex* verts = vs.Data<MyVertex> ();

    if (verts)
    { 
      static float z = 0.0f;
      
      verts [0].position      = vec3f (1.0f/z, -1.0f, 0.0f);
      verts [0].normal        = 0.0f;
      verts [0].color         = 255;
      verts [0].texcoord0     = 0.0f;
      verts [0].first_weight  = 0;
      verts [0].weights_count = 4;
      verts [1].position      = 0.0f;
      verts [1].normal        = 0.0f;
      verts [1].color         = 255;
      verts [1].texcoord0     = vec2f (sqrt (-1.0f), 1.0f);
      verts [1].first_weight  = 2;
      verts [1].weights_count = 3;
    }

    VertexWeightStream vws (4);

    VertexWeight* weights = vws.Data ();

    weights [0].joint_index  = 0;
    weights [0].joint_weight = -0.4f;
    weights [1].joint_index  = 1;
    weights [1].joint_weight = 0.2f;
    weights [2].joint_index  = 6;
    weights [2].joint_weight = 0.3f;
    weights [3].joint_index  = 2;
    weights [3].joint_weight = 0.0f;

    IndexBuffer ib (5);

    unsigned int* indices = ib.Data ();

    for (size_t i=0; i<ib.Size (); i++)
      indices [i] = i % 3;
      
    VertexBuffer vb;

    vb.Attach (vs);
    vb.AttachWeights (vws);

    mesh.Attach (vb);
    mesh.Attach (ib);

    mesh.AddPrimitive (PrimitiveType_LineList, 0, 6, 3, 0);
    mesh.AddPrimitive (PrimitiveType_TriangleList, 0, 0, 2, "");
    mesh.AddPrimitive (PrimitiveType_TriangleStrip, 0, 0, 3, "");
    mesh.AddPrimitive (PrimitiveType_TriangleList, 1, 0, 2, "");

    printf ("check=%d\n", check (mesh, 5, &print));
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
