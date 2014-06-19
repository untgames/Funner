#include "shared.h"

using namespace media;
using namespace common;

const char* SOURCE_IMAGE_NAME                  = "data/pic1.jpg";
const char* RESULTS_DIR                        = "results";
const char* RESULT_POWER_OF_TWO_ATLAS_NAME     = "/io/stdout/power_of_two_atlas.xatlas";
const char* RESULT_NON_POWER_OF_TWO_ATLAS_NAME = "/io/stdout/non_power_of_two_atlas.xatlas";
const char* RESULT_POWER_OF_TWO_IMAGE_NAME     = "results/power_of_two_image.bmp";
const char* RESULT_NON_POWER_OF_TWO_IMAGE_NAME = "results/non_power_of_two_image.bmp";

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

    printf ("Atlas margin is %d\n", atlas_builder.Margin ());
    printf ("Atlas max image size is %d\n", atlas_builder.MaxImageSize ());
    printf ("Atlas pack flags is %d\n", atlas_builder.PackFlags ());

    atlas_builder.SetMargin (0);
    atlas_builder.SetMaxImageSize (4096);
    atlas_builder.SetPackFlags (AtlasPackFlag_PowerOfTwoEdges);

    printf ("Atlas margin is %d\n", atlas_builder.Margin ());
    printf ("Atlas max image size is %d\n", atlas_builder.MaxImageSize ());
    printf ("Atlas pack flags is %d\n", atlas_builder.PackFlags ());

    atlas_builder.Insert (SOURCE_IMAGE_NAME);

    Image image (SOURCE_IMAGE_WIDTH, SOURCE_IMAGE_HEIGHT, 1, PixelFormat_RGB8);

    ImagePixel *image_pixel = (ImagePixel*)image.Bitmap (), image_color = {{255, 0, 0}};

    for (size_t i = 0; i < SOURCE_IMAGE_WIDTH * SOURCE_IMAGE_HEIGHT; i++, image_pixel++)
      *image_pixel = image_color;

    atlas_builder.Insert (image, AtlasBuilderInsertMode_Default, 1);

    Image image2 (SOURCE_IMAGE_WIDTH, SOURCE_IMAGE_HEIGHT, 1, PixelFormat_RGB8);

    ImagePixel *image2_pixel = (ImagePixel*)image2.Bitmap (), image2_color = {{0, 255, 0}};

    for (size_t i = 0; i < SOURCE_IMAGE_WIDTH * SOURCE_IMAGE_HEIGHT; i++, image2_pixel++)
      *image2_pixel = image2_color;

    atlas_builder.Insert (image2.Width (), image2.Height (), image2.Format (), image2.Bitmap (), false, "data", 1);

    printf ("Atlases count = %u\n", atlas_builder.AtlasesCount ());

    Atlas result_atlas;
    Image result_image;

    atlas_builder.BuildAtlas (0, RESULT_POWER_OF_TWO_IMAGE_NAME, result_atlas);
    atlas_builder.BuildAtlasImage (0, result_image);

    printf ("Saving power of two atlas\n");

    result_atlas.Save (RESULT_POWER_OF_TWO_ATLAS_NAME);
    result_image.Save (RESULT_POWER_OF_TWO_IMAGE_NAME);

    atlas_builder.SetPackFlags (0);

    atlas_builder.BuildAtlas (0, RESULT_NON_POWER_OF_TWO_IMAGE_NAME, result_atlas);
    atlas_builder.BuildAtlasImage (0, result_image);

    printf ("Saving non power of two atlas\n");
    result_atlas.Save (RESULT_NON_POWER_OF_TWO_ATLAS_NAME);
    result_image.Save (RESULT_NON_POWER_OF_TWO_IMAGE_NAME);

    atlas_builder.Reset ();

    atlas_builder.SetPackFlags (AtlasPackFlag_PowerOfTwoEdges);

    printf ("Atlases count = %u\n", atlas_builder.AtlasesCount ());

    dump_hash (RESULT_POWER_OF_TWO_IMAGE_NAME);
    dump_hash (RESULT_NON_POWER_OF_TWO_IMAGE_NAME);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }

  return 0;
}
