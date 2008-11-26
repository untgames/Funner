#include "shared.h"

const char* RESULTS_DIR         = "results";
const char* SRC_MODEL_FILE_NAME = "data/loader_test.binmesh";
const char* DST_MODEL_FILE_NAME = "results/loader_test.binmesh";

void dump_hash (const char* file_name)
{
  common::FileHash file_hash;

  common::FileSystem::GetFileHash (file_name,file_hash);

  printf ("File '%s' hash: {", file_name);

  for (size_t i=0;i<15;i++)
    printf ("%02x,",file_hash.md5 [i]);

  printf ("%02x}\n",file_hash.md5 [15]);
}

int main ()
{
  try
  {
    printf ("Results of binmesh_loader_test:\n");

    if (!common::FileSystem::IsDir (RESULTS_DIR))
      common::FileSystem::Mkdir (RESULTS_DIR);

    media::geometry::MeshLibrary (SRC_MODEL_FILE_NAME).Save (DST_MODEL_FILE_NAME);

    dump_hash (SRC_MODEL_FILE_NAME);
    dump_hash (DST_MODEL_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
