#include "shared.h"

void dump (const CustomVertex& v)
{
  printf ("  xyz=[%g %g %g] normal=[%g %g %g] rgba=[%u %u %u %u] st=[%g %g]\n", v.position.x, v.position.y, v.position.z,
          v.normal.x, v.normal.y, v.normal.z, v.color.x, v.color.y, v.color.z, v.color.w, v.texcoord0.x, v.texcoord0.y);
}

void dump (const VertexStream& s)
{
  printf ("vertex stream (%u vertices):\n", s.Size ());

  const CustomVertex* verts = s.Data<CustomVertex> ();

  for (size_t i=0; i<s.Size (); i++)
    dump (verts [i]);
}

int main ()
{
  printf ("Results of vertex_stream_test:\n");

  VertexStream vs1 (5, get_vertex_format<CustomVertex> (), sizeof (CustomVertex)), vs2 (get_vertex_format<CustomVertex> ());
  
  CustomVertex* verts = vs1.Data<CustomVertex> ();
  
  for (size_t i=0; i<vs1.Size (); i++)
  {
    verts [i].position  = float (i);
    verts [i].normal    = vec3f (1, 0, 0);
    verts [i].color     = vec4ub (i, i*2, i*3, i*4);
    verts [i].texcoord0 = vec2f (float (i), -float (i));
  }

  printf ("Dump vs1\n");

  dump (vs1);
  
  printf ("vs2 = vs1\n");    
  
  vs2 = vs1;
  
  dump (vs2);
  
  printf ("reserve vs1 (current capacity=%u)\n", vs1.Capacity ());
  
  vs1.Reserve (14);
  
  printf ("after vs1 reserve capacity=%u\n", vs1.Capacity ());
  
  printf ("resize vs1\n");

  vs1.Resize (6);

  CustomVertex& new_vertex = vs1.Data<CustomVertex> ()[5];

  new_vertex.position  = new_vertex.normal = 0;
  new_vertex.color     = 0;
  new_vertex.texcoord0 = 0;

  dump (vs1);
  
  printf ("instance vs2\n");

  vs2.Assign (vs1, BufferCloneMode_Instance);
  
  dump (vs2);

  printf ("clear vs1 (dump vs2)\n");
  
  vs1.Clear ();
  
  dump (vs2);
  
  printf ("dump vs2 format\n");

  dump (vs2.Format (), vs2.VertexSize ());

  return 0;
}
