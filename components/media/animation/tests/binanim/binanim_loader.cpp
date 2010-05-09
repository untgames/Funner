#include "shared.h"

const char* LOG_MASK           = "media.animation.*";
const char* RESULTS_DIR        = "results";
const char* SRC_ANIM_FILE_NAME = "data/loader_test.binanim";
const char* DST_ANIM_FILE_NAME = "results/loader_test.binanim";

using namespace media::animation;

void print_log (const char*, const char* message)
{
  printf ("%s\n", message);
}

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
    printf ("Results of binanim_loader_test:\n");

    if (!common::FileSystem::IsDir (RESULTS_DIR))
      common::FileSystem::Mkdir (RESULTS_DIR);

    common::LogFilter log_filter (LOG_MASK, &print_log);

    AnimationLibrary (SRC_ANIM_FILE_NAME).Save (DST_ANIM_FILE_NAME);

    dump_hash (SRC_ANIM_FILE_NAME);
    dump_hash (DST_ANIM_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
