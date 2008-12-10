#include <stdio.h>

#include <string>

#include "core.h"

using namespace modeler;

const char* DESC_FILE_SUFFIX        = ".desc";
const char* SAVE_COORDS_OPTION_NAME = "-save_coords_only";
const int   HEADER                  = 'TRJC';
const int   VERSION                 = 1;

void save_desc_file (const char* result_file_name, size_t file_size)
{
  size_t file_name_buffer_size = strlen (result_file_name) + strlen (DESC_FILE_SUFFIX) + 1;

  char* desc_file_name = new char [file_name_buffer_size];

  snprintf (desc_file_name, file_name_buffer_size, "%s%s", result_file_name, DESC_FILE_SUFFIX);

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
    save_desc_file (result_file_name, 0);
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

  fflush (result_file);

  save_desc_file (result_file_name, ftell (result_file));

  fclose (result_file);
}

void save_vertices (const char* result_file_name, const DrawVertexArray& vertices, size_t vertices_to_save)
{
  FILE* result_file = fopen (result_file_name, "wb");

  if (!result_file)
  {
    printf ("Can't open result file '%s'\n", result_file_name);
    return;
  }

  fwrite (&HEADER, sizeof (HEADER), 1, result_file);
  fwrite (&VERSION, sizeof (VERSION), 1, result_file);

  if (vertices_to_save > vertices.size ())
    vertices_to_save = vertices.size ();

  fwrite (&vertices_to_save, sizeof (vertices_to_save), 1, result_file);

  float index = 0, index_delta = vertices.size () / (float)vertices_to_save;

  for (size_t i = 0; i < vertices_to_save; i++, index += index_delta)
  {
    float nu1 = vertices[(size_t)index].position.x,
          nu2 = vertices[(size_t)index].position.y,
          nu3 = vertices[(size_t)index].position.z,
          nu_length = sqrt (nu1 * nu1 + nu2 * nu2 + nu3 * nu3);

    nu1 /= -nu_length;  //обход хитрости с развёрнутыми траекториями
    nu2 /= nu_length;
    nu3 /= nu_length;

    fwrite (&nu1, sizeof (nu1), 1, result_file);
    fwrite (&nu2, sizeof (nu2), 1, result_file);
    fwrite (&nu3, sizeof (nu3), 1, result_file);
  }

  fflush (result_file);

  save_desc_file (result_file_name, ftell (result_file));

  fclose (result_file);
}

int main (int argc, char* argv[])
{
  if (argc < 4 || argc > 5)
  {
    printf ("Usage: modeler-envelope [%s] model_data_file result_file iterations_count\n", SAVE_COORDS_OPTION_NAME);
    printf ("  If save_coord_only option setted, iteration_count is needed coords count\n");
    return 1;
  }

  bool save_coords_only = !strcmp (argv[1], SAVE_COORDS_OPTION_NAME);

  size_t options_count = save_coords_only ? 1 : 0;

  ModelData model_data;

  LoadModelData (argv[1 + options_count], model_data);

  DrawVertexArray    vertices;
  DrawPrimitiveArray primitives;

  size_t iterations_count = atoi (argv[3 + options_count]);

  if (iterations_count < 1)
  {
    if (!save_coords_only)
      save_desc_file (argv[2 + options_count], 0);

    return 2;
  }

  BuildMegaSurface (model_data, iterations_count, iterations_count, vertices, primitives);

  if (options_count)
    save_vertices (argv[2 + options_count], vertices, atoi (argv[3 + options_count]));
  else
    dump (argv[2 + options_count], vertices, primitives);

  return 0;
}
