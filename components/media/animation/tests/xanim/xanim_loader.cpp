#include "shared.h"

const char* LOG_MASK            = "media.animation.*";
const char* SRC_ANIM_FILE_NAME = "data/test.xanim";
const char* DST_ANIM_FILE_NAME = "/io/stdout/test.xanim";

using namespace media::animation;

void print_log (const char*, const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  try
  {
    printf ("Results of xanim_loader_test:\n");

    common::LogFilter log_filter (LOG_MASK, &print_log);

    AnimationLibrary (SRC_ANIM_FILE_NAME).Save (DST_ANIM_FILE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
