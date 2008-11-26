#include <cstdio>
#include <exception>

#include <xtl/function.h>

#include <media/mesh.h>

#include <common/log.h>

const char* LOG_MASK = "*";

void print_log (const char*, const char* message)
{
  printf ("%s\n", message);
}

int main (int argc, char* argv[])
{
  if (argc != 3)
  {
    printf ("Usage: mesh-converter source_file_name destination_file_name");

    return 1;
  }

  try
  {
    common::LogFilter log_filter (LOG_MASK, &print_log);

    media::geometry::MeshLibrary (argv[1]).Save (argv[2]);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
