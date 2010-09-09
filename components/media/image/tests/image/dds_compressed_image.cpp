#include "shared.h"

using namespace media;

const char* IMAGE_NAME = "data/pic1.dds";

int main ()
{
  printf ("Results of dds_compressed_image_test:\n");

  try
  {
    CompressedImage image (IMAGE_NAME);
    
    printf ("width:        %u\n", image.Width ());
    printf ("height:       %u\n", image.Height ());
    printf ("layers_count: %u\n", image.LayersCount ());
    printf ("mips_count:   %u\n", image.MipsCount ());
    printf ("format:       '%s'\n", image.Format ());
    printf ("layers:\n");
    
    for (size_t i=0; i<image.LayersCount (); i++)
    {
      printf ("  layer%u:\n", i);

      for (size_t j=0; j<image.MipsCount (); j++)
      {
        size_t size = image.BitmapSize (i, j);
        size_t hash = common::crc32 (image.Bitmap (i, j), size);

        printf ("    mip%u: size=%u, crc32=%08x\n", j, size, hash);
      }
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
