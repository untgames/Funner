#include "shared.h"

using namespace media;

const char* IMAGE_NAME = "data/armor.dds";

int main ()
{
  try
  {
    DdsImage image (IMAGE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
