#include <stdio.h>

#include <string>

#include "core.h"

const char* DESC_FILE_SUFFIX            = ".desc";
const int   TRAJECTORIES_COORDS_HEADER  = 'TRJC';
const int   TRAJECTORIES_COORDS_VERSION = 1;

void save_desc_file (const char* result_file_name, size_t file_size)
{
  size_t file_name_buffer_size = strlen (result_file_name) + strlen (DESC_FILE_SUFFIX) + 1;

  char* desc_file_name = new char [file_name_buffer_size];

  sprintf (desc_file_name, "%s%s", result_file_name, DESC_FILE_SUFFIX);

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
  if ((argc < 6) || (argc > 8))
  {
    printf ("Usage: modeler-trajectory mode args\n");
    printf ("  'mode' values:\n");
    printf ("    'args-input': 'args' = nu1 nu2 nu3 model_data_file result_file vertices_count\n");
    printf ("    'bin-file-input': 'args' = nu_file nu_index model_data_file result_file vertices_count\n");
    printf ("    'text-file-input': 'args' = nu_file model_data_file result_file vertices_count\n");
    return 1;
  }

  ModelData model_data;
  size_t    optional_parameters_count;
  float     nu[3];

  if (!strcmp (argv[1], "args-input"))
  {
    nu[0] = (float)atof (argv[2]);
    nu[1] = (float)atof (argv[3]);
    nu[2] = (float)atof (argv[4]);
    optional_parameters_count = 3;
  }
  else if (!strcmp (argv[1], "bin-file-input"))
  {
    FILE* nu_file = fopen (argv[2], "rb");

    if (!nu_file)
    {
      printf ("Can't open nu file '%s'\n", argv[2]);
      save_desc_file (argv[5], 0);
      return 1;
    }

    int header, version;

    fread (&header, sizeof (header), 1, nu_file);
    fread (&version, sizeof (version), 1, nu_file);

    if ((header != TRAJECTORIES_COORDS_HEADER) || (version != TRAJECTORIES_COORDS_VERSION))
    {
      printf ("nu file '%s' has unknown header or version\n", argv[2]);
      save_desc_file (argv[5], 0);
      fclose (nu_file);
      return 1;
    }

    size_t read_position = sizeof (int) * 2 + sizeof (size_t) + atoi (argv[3]) * sizeof (float) * 3;

    if (fseek (nu_file, read_position, SEEK_SET))
    {
      printf ("Can't seek in nu file '%s' to position %u\n", argv[2], read_position);
      save_desc_file (argv[5], 0);
      fclose (nu_file);
      return 1;
    }

    for (size_t i = 0; i < 3; i++)
    {
      if (!fread (&nu[i], sizeof (nu[i]), 1, nu_file))
      {
        printf ("Can't read nu%u from file '%s'\n", i, argv[2]);
        save_desc_file (argv[5], 0);
        fclose (nu_file);
        return 1;
      }
    }

    fclose (nu_file);

    optional_parameters_count = 2;
  }
  else if (!strcmp (argv[1], "text-file-input"))
  {
    FILE* nu_file = fopen (argv[2], "r");

    if (!nu_file)
    {
      printf ("Can't open nu file '%s'\n", argv[2]);
      save_desc_file (argv[4], 0);
      return 1;
    }

    if (fscanf (nu_file, "%f %f %f", &nu[0], &nu[1], &nu[2]) != 3)
    {
      printf ("Incorrect nu file '%s' format, must be: float float float.\n", argv[2]);
      save_desc_file (argv[4], 0);
      return 1;
    }

    fclose (nu_file);

    optional_parameters_count = 1;
  }
  else
  {
    printf ("Unknown mode parameter, launch application without args to usage description.\n");
    return 1;
  }

  LoadModelData (argv[2 + optional_parameters_count], model_data);

  DrawVertexArray    vertices;
  DrawPrimitiveArray primitives;

  BuildTrajectory (model_data, nu[0], nu[1], nu[2], atoi (argv[4 + optional_parameters_count]), vertices, primitives);

  dump (argv[3 + optional_parameters_count], vertices, primitives);

  return 0;
}
