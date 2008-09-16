#include <stdio.h>

#include <string>

#include "core.h"

using namespace modeler;

void dump (const char* result_file_name, const DrawVertexArray& vertices, const DrawPrimitiveArray& primitives)
{
  FILE* result_file = fopen (result_file_name, "w");

  if (!result_file)
  {
    printf ("Can't open result file '%s'\n", result_file_name);
    return;
  }

  fprintf (result_file, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  fprintf (result_file, "<mesh_library>\n");
  fprintf (result_file, "  <vertex_streams>\n");
  fprintf (result_file, "    <vertex_stream id=\"vs#1\" vertices_count=\"%u\" vertex_size=\"24\">\n", vertices.size ());
  fprintf (result_file, "      <channel semantic=\"position\" type=\"float3\" offset=\"0\">\n");
  
  for (size_t i=0; i<vertices.size (); i++)
  {
    const Vec3f& p = vertices [i].position;

    fprintf (result_file, "%g %g %g ", p.x, p.y, p.z);
  }
  
  fprintf (result_file, "\n");
  fprintf (result_file, "      </channel>\n");
  fprintf (result_file, "      <channel semantic=\"normal\" type=\"float3\" offset=\"12\">\n");
  
  for (size_t i=0; i<vertices.size (); i++)
  {
    const Vec3f& n = vertices [i].normal;

    fprintf (result_file, "%g %g %g ", n.x, n.y, n.z);
  }
  
  fprintf (result_file, "\n");
  fprintf (result_file, "      </channel>\n");
  fprintf (result_file, "    </vertex_stream>\n");
  fprintf (result_file, "  </vertex_streams>\n");
  fprintf (result_file, "  <vertex_buffers>\n");
  fprintf (result_file, "    <vertex_buffer id=\"vb#1\">\n");
  fprintf (result_file, "      <streams>\n");
  fprintf (result_file, "        vs#1\n");
  fprintf (result_file, "      </streams>\n");
  fprintf (result_file, "    </vertex_buffer>\n");
  fprintf (result_file, "  </vertex_buffers>\n");
  fprintf (result_file, "  <meshes>\n");
  fprintf (result_file, "    <mesh name=\"mega-surface\">\n");
  fprintf (result_file, "      <vertex_buffers>\n");
  fprintf (result_file, "        vb#1\n");
  fprintf (result_file, "      </vertex_buffers>\n");
  fprintf (result_file, "      <primitives>\n");
  
  for (size_t i=0; i<primitives.size (); i++)
  {
    const DrawPrimitive& primitive = primitives [i];
    
    fprintf (result_file, "        <primitive type=\"triangle_strip\" material=\"\" vertex_buffer=\"0\" first=\"%u\" count=\"%u\"/>\n",
            primitive.first, primitive.count-2);
  }
  
  fprintf (result_file, "      </primitives>\n");
  fprintf (result_file, "    </mesh>\n");
  fprintf (result_file, "  </meshes>\n");
  fprintf (result_file, "</mesh_library>\n");

  fclose (result_file);
}

int main (int argc, char* argv[])
{
  if (argc != 4)
  {
    printf ("Usage: modeler-envelope model_data_file result_file iterations_count\n");
    return 1;
  }

  ModelData model_data;

  LoadModelData (argv[1], model_data);  
  
  DrawVertexArray    vertices;
  DrawPrimitiveArray primitives; 

  size_t iterations_count = atoi (argv[3]);

  BuildMegaSurface (model_data, iterations_count, iterations_count, vertices, primitives);

//  printf ("Mega surface builded successfull!\n");
//  printf ("Primitives %u, vertices %u:\n", primitives.size (), vertices.size ());
  
  dump (argv[2], vertices, primitives);  

  return 0;
}
