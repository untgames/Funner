#include <stdio.h>

#include <string>

#include "core.h"

const char* DESC_FILE_SUFFIX = ".desc";

void save_desc_file (const char* result_file_name, size_t file_size)
{
  size_t file_name_buffer_size = strlen (result_file_name) + strlen (DESC_FILE_SUFFIX) + 1;

  char* desc_file_name = new char [file_name_buffer_size];

  _snprintf (desc_file_name, file_name_buffer_size, "%s%s", result_file_name, DESC_FILE_SUFFIX);

  FILE* desc_file = fopen (desc_file_name, "w");

  delete [] desc_file_name;

  if (!desc_file)
  {
    printf ("Can't open desc file '%s'\n", desc_file_name);
    return;
  }

  fwrite (&file_size, sizeof (file_size), 1, desc_file);

  fclose (desc_file);
}

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
  fprintf (result_file, "    <vertex_stream id=\"vs#1\" vertices_count=\"%u\" vertex_size=\"16\">\n", vertices.size ());
  fprintf (result_file, "      <channel semantic=\"position\" type=\"float3\" offset=\"0\">\n");
  
  for (size_t i=0; i<vertices.size (); i++)
  {
    const Vec3f& p = vertices [i].position;

    fprintf (result_file, "%g %g %g ", p.x, p.y, p.z);
  }
  
  fprintf (result_file, "\n");
  fprintf (result_file, "      </channel>\n");
  fprintf (result_file, "      <channel semantic=\"color\" type=\"ubyte4\" offset=\"12\">\n");
  
  for (size_t i=0; i<vertices.size (); i++)
  {
    const Color4f& c = vertices [i].color;

    fprintf (result_file, "%u %u %u %u ", size_t (255.0f * c.r), size_t (255.0f * c.g), size_t (255.0f * c.b), size_t (255.0f * c.a));
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
  fprintf (result_file, "    <mesh name=\"trajectory\">\n");
  fprintf (result_file, "      <vertex_buffers>\n");
  fprintf (result_file, "        vb#1\n");
  fprintf (result_file, "      </vertex_buffers>\n");
  fprintf (result_file, "      <primitives>\n");
  
  for (size_t i=0; i<primitives.size (); i++)
  {
    const DrawPrimitive& primitive = primitives [i];
    
    fprintf (result_file, "        <primitive type=\"line_list\" material=\"\" vertex_buffer=\"0\" first=\"%u\" count=\"%u\"/>\n",
            primitive.first, primitive.count);
  }
  
  fprintf (result_file, "      </primitives>\n");
  fprintf (result_file, "    </mesh>\n");
  fprintf (result_file, "  </meshes>\n");
  fprintf (result_file, "</mesh_library>\n");

  fflush (result_file);

  save_desc_file (result_file_name, ftell (result_file));

  fclose (result_file);
}

int main (int argc, char* argv[])
{
  if (argc != 7)
  {
    printf ("Usage: modeler-trajectory model_data_file nu1 nu2 nu3 result_file iterations_count\n");
    return 1;
  }

  ModelData model_data;

  double nu1 = atof (argv[2]), nu2 = atof (argv[3]), nu3 = atof (argv[4]);

  double nu_sqr_sum = nu1 * nu1 + nu2 * nu2 + nu3 * nu3;

  if ((nu_sqr_sum < 0.99) || (nu_sqr_sum > 1.01))
  {
    save_desc_file (argv[5], 0);
    printf ("Invalid input nu data, nu1 * nu1 + nu2 * nu2 + nu3 * nu3 must be 1\n");
    return 2;
  }

  LoadModelData (argv[1], model_data);
  
  DrawVertexArray    vertices;
  DrawPrimitiveArray primitives;

  BuildTrajectory (model_data, nu1, nu2, nu3, atoi (argv[6]), vertices, primitives);
  
  dump (argv[5], vertices, primitives);

  return 0;
}
