#include "shared.h"

const char* LOG_MASK            = "media.physics.*";
const char* SRC_PHYS_FILE_NAME = "data/test.xphys";
const char* DST_PHYS_FILE_NAME = "/io/stdout/test.xphys";

using namespace media::physics;

void print_log (const char*, const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  try
  {
    printf ("Results of xphys_loader_test:\n");

    common::LogFilter log_filter (LOG_MASK, &print_log);

    PhysicsLibrary::SaveOptions save_options;

    PhysicsLibrary (SRC_PHYS_FILE_NAME).Save (DST_PHYS_FILE_NAME, save_options);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
