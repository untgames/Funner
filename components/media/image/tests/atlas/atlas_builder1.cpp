#include "shared.h"

using namespace media;
using namespace common;

const char* SOURCE_IMAGE_NAME                  = "data/pic1.jpg";
const char* RESULTS_DIR                        = "results";
const char* RESULT_POWER_OF_TWO_ATLAS_NAME     = "results/power_of_two_atlas.xatlas";
const char* RESULT_NON_POWER_OF_TWO_ATLAS_NAME = "results/non_power_of_two_atlas.xatlas";
const char* RESULT_EMPTY_ATLAS_NAME            = "results/empty_atlas.xatlas";
const char* RESULT_POWER_OF_TWO_IMAGE_NAME     = "results/power_of_two_image.bmp";
const char* RESULT_NON_POWER_OF_TWO_IMAGE_NAME = "results/non_power_of_two_image.bmp";
const char* RESULT_EMPTY_IMAGE_NAME            = "results/empty_image.bmp";

const size_t SOURCE_IMAGE_WIDTH  = 123;
const size_t SOURCE_IMAGE_HEIGHT = 61;

struct ImagePixel
{
  unsigned char color[3];
};

void dump_hash (const char* file_name)
{
  FileHash file_hash;

  FileSystem::GetFileHash (file_name,file_hash);

  printf ("File '%s' hash: {", file_name);

  for (size_t i=0;i<15;i++)
    printf ("%02x,",file_hash.md5 [i]);

  printf ("%02x}\n",file_hash.md5 [15]);
}

int main ()
{
  try
  {
    printf ("Results of atlas_builder1 test:\n");
    
    if (!FileSystem::IsDir (RESULTS_DIR))
      FileSystem::Mkdir (RESULTS_DIR);    

    AtlasBuilder atlas_builder;

    printf ("Atlas image name is '%s'\n", atlas_builder.AtlasImageName ());

    atlas_builder.SetAtlasImageName ("atlas_image");

    printf ("Atlas image name is '%s'\n", atlas_builder.AtlasImageName ());

    atlas_builder.Insert (SOURCE_IMAGE_NAME);

    Image image (SOURCE_IMAGE_WIDTH, SOURCE_IMAGE_HEIGHT, 1, PixelFormat_RGB8);

    ImagePixel *image_pixel = (ImagePixel*)image.Bitmap (), image_color = {255, 0, 0};

    for (size_t i = 0; i < SOURCE_IMAGE_WIDTH * SOURCE_IMAGE_HEIGHT; i++, image_pixel++)
      *image_pixel = image_color;

    atlas_builder.Insert (image);

    Atlas result_atlas;
    Image result_image;

    atlas_builder.Build (result_atlas, result_image, AtlasPackFlag_PowerOfTwoEdges);

    result_atlas.Save (RESULT_POWER_OF_TWO_ATLAS_NAME);
    result_image.Save (RESULT_POWER_OF_TWO_IMAGE_NAME);

    atlas_builder.Build (result_atlas, result_image, 0);

    result_atlas.Save (RESULT_NON_POWER_OF_TWO_ATLAS_NAME);
    result_image.Save (RESULT_NON_POWER_OF_TWO_IMAGE_NAME);

    atlas_builder.Reset ();

    atlas_builder.Build (result_atlas, result_image, AtlasPackFlag_PowerOfTwoEdges);

    result_atlas.Save (RESULT_EMPTY_ATLAS_NAME);
    result_image.Save (RESULT_EMPTY_IMAGE_NAME);

    dump_hash (RESULT_POWER_OF_TWO_ATLAS_NAME);
    dump_hash (RESULT_NON_POWER_OF_TWO_ATLAS_NAME);
    dump_hash (RESULT_EMPTY_ATLAS_NAME);
    dump_hash (RESULT_POWER_OF_TWO_IMAGE_NAME);
    dump_hash (RESULT_NON_POWER_OF_TWO_IMAGE_NAME);
//    dump_hash (RESULT_EMPTY_IMAGE_NAME);
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }

  return 0;
}
