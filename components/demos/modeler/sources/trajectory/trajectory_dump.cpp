#include <stdio.h>

#include <string>

#include "core.h"

const char* INPUT_FILE_NAME_PREFIX = "media/models/input.";
const char* INPUT_FILE_NAME_SUFFIX = ".dat";

void dump (const DrawVertexArray& vertices, const DrawPrimitiveArray& primitives)
{
  printf ("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  printf ("<mesh_library>\n");
  printf ("  <vertex_streams>\n");
  printf ("    <vertex_stream id=\"vs#1\" vertices_count=\"%u\" vertex_size=\"16\">\n", vertices.size ());
  printf ("      <channel semantic=\"position\" type=\"float3\" offset=\"0\">\n");
  
  for (size_t i=0; i<vertices.size (); i++)
  {
    const Vec3f& p = vertices [i].position;

    printf ("%g %g %g ", p.x, p.y, p.z);
  }
  
  printf ("\n");
  printf ("      </channel>\n");
  printf ("      <channel semantic=\"color\" type=\"ubyte4\" offset=\"12\">\n");
  
  for (size_t i=0; i<vertices.size (); i++)
  {
    const Color4f& c = vertices [i].color;

    printf ("%u %u %u %u ", size_t (255.0f * c.r), size_t (255.0f * c.g), size_t (255.0f * c.b), size_t (255.0f * c.a));
  }

  printf ("\n");  
  printf ("      </channel>\n");
  printf ("    </vertex_stream>\n");
  printf ("  </vertex_streams>\n");
  printf ("  <vertex_buffers>\n");
  printf ("    <vertex_buffer id=\"vb#1\">\n");
  printf ("      <streams>\n");
  printf ("        vs#1\n");
  printf ("      </streams>\n");
  printf ("    </vertex_buffer>\n");
  printf ("  </vertex_buffers>\n");
  printf ("  <meshes>\n");
  printf ("    <mesh name=\"trajectory\">\n");
  printf ("      <vertex_buffers>\n");
  printf ("        vb#1\n");
  printf ("      </vertex_buffers>\n");
  printf ("      <primitives>\n");
  
  for (size_t i=0; i<primitives.size (); i++)
  {
    const DrawPrimitive& primitive = primitives [i];
    
    printf ("        <primitive type=\"line_list\" material=\"\" vertex_buffer=\"0\" first=\"%u\" count=\"%u\"/>\n",
            primitive.first, primitive.count);
  }
  
  printf ("      </primitives>\n");
  printf ("    </mesh>\n");
  printf ("  </meshes>\n");
  printf ("</mesh_library>\n");
}

int main (int argc, char* argv[])
{
  if (argc != 2)
  {
    printf ("No arg!\n");
    return 1;
  }

  ModelData model_data;

  std::string input_file_name (INPUT_FILE_NAME_PREFIX);

  input_file_name.append (argv[1]);
  input_file_name.append (INPUT_FILE_NAME_SUFFIX);

  LoadModelData (input_file_name.c_str (), model_data);
  
  DrawVertexArray    vertices;
  DrawPrimitiveArray primitives;

  BuildTrajectory (model_data, vertices, primitives);
  
  dump (vertices, primitives);

  return 0;
}
