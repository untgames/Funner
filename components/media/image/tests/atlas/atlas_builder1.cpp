#include "shared.h"

using namespace media;
using namespace common;

const char* SOURCE_IMAGE_NAME                  = "data/pic1.jpg";
const char* RESULTS_DIR                        = "results";
const char* RESULT_FAST_ATLAS_NAME             = "/io/stdout/fast.xatlas";
const char* RESULT_NON_POWER_OF_TWO_ATLAS_NAME = "/io/stdout/non_power_of_two_atlas.xatlas";
const char* RESULT_POWER_OF_TWO_ATLAS_NAME     = "/io/stdout/power_of_two_atlas.xatlas";
const char* RESULT_FAST_IMAGE_NAME             = "results/fast_image.bmp";
const char* RESULT_NON_POWER_OF_TWO_IMAGE_NAME = "results/non_power_of_two_image.bmp";
const char* RESULT_POWER_OF_TWO_IMAGE_NAME     = "results/power_of_two_image.bmp";

const size_t SOURCE_IMAGE_WIDTH  = 123;
const size_t SOURCE_IMAGE_HEIGHT = 61;

struct ImagePixel
{
  unsigned char color[3];
};

struct FastImageTestDesc
{
  unsigned int  width, height;
  unsigned char color;
};

const FastImageTestDesc FAST_IMAGE_TEST_DESCS [] = {
  { 23, 35, 1}, { 16, 35, 2}, { 24, 35, 3}, { 21, 35, 4}, { 24, 35, 5},
  { 20, 35, 6}, { 23, 35, 7}, { 25, 35, 8}, { 21, 35, 9}, { 23, 35, 10},
  { 19, 37, 11}, { 19, 37, 12}, { 19, 37, 13}, { 19, 37, 14}, { 19, 37, 15},
  { 19, 37, 14}, { 19, 37, 15}, { 38, 36, 14}, { 32, 35, 15}, { 31, 37, 14},
  { 36, 35, 15}, { 32, 35, 14}, { 27, 35, 15}, { 37, 37, 15}, { 38, 35, 14},
  { 17, 35, 15}, { 20, 36, 15}, { 37, 35, 14}, { 32, 35, 15}, { 46, 35, 15},
  { 39, 36, 14}, { 34, 37, 15}, { 28, 35, 15}, { 34, 45, 14}, { 37, 35, 15},
  { 23, 37, 15}, { 29, 35, 14}, { 38, 36, 15}, { 38, 36, 15}, { 50, 36, 14},
  { 39, 35, 15}, { 38, 35, 15}, { 31, 35, 14}, { 19, 37, 15}, { 19, 37, 15},
  { 19, 37, 14}, { 19, 37, 15}, { 19, 37, 15}, { 19, 37, 14}, { 22, 26, 15},
  { 25, 38, 14}, { 22, 26, 15}, { 26, 38, 15}, { 22, 26, 14}, { 22, 37, 15},
  { 25, 36, 14}, { 26, 37, 15}, { 13, 37, 15}, { 14, 48, 14}, { 27, 37, 15},
  { 13, 37, 14}, { 39, 25, 15}, { 26, 25, 15}, { 24, 26, 14}, { 25, 36, 15},
  { 24, 36, 14}, { 19, 25, 15}, { 17, 26, 15}, { 16, 32, 14}, { 27, 25, 15},
  { 28, 25, 14}, { 39, 25, 15}, { 26, 24, 15}, { 27, 35, 14}, { 22, 24, 15},
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
    printf ("Atlas alignment is %d\n", atlas_builder.Alignment ());
    printf ("Atlas max image size is %d\n", atlas_builder.MaxImageSize ());
    printf ("Atlas pack flags is %d\n", atlas_builder.PackFlags ());

    atlas_builder.SetMargin (0);
    atlas_builder.SetAlignment (1);
    atlas_builder.SetMaxImageSize (4096);
    atlas_builder.SetPackFlags (AtlasPackFlag_PowerOfTwoEdges);

    printf ("Atlas margin is %d\n", atlas_builder.Margin ());
    printf ("Atlas alignment is %d\n", atlas_builder.Alignment ());
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

    atlas_builder.Reset ();

    //test fast pack
    for (unsigned int i = 0, count = sizeof (FAST_IMAGE_TEST_DESCS) / sizeof (*FAST_IMAGE_TEST_DESCS); i < count; i++)
    {
      const FastImageTestDesc& desc = FAST_IMAGE_TEST_DESCS [i];

      Image fast_image (desc.width, desc.height, 1, PixelFormat_RGB8);

      ImagePixel *fast_image_pixel = (ImagePixel*)fast_image.Bitmap (), fast_image_color = {{desc.color, desc.color, desc.color}};

      for (size_t i = 0; i < desc.width * desc.height; i++, fast_image_pixel++)
      {
        *fast_image_pixel = fast_image_color;
      }

      atlas_builder.Insert (fast_image, AtlasBuilderInsertMode_Default, 1);
    }

    atlas_builder.SetMargin (15);
    atlas_builder.SetMaxImageSize (256);
    atlas_builder.SetPackFlags (AtlasPackFlag_Fast | AtlasPackFlag_PowerOfTwoEdges);

    for (unsigned int i = 0, count = atlas_builder.AtlasesCount (); i < count; i++)
    {
      printf ("Saving fast atlas %u\n", i);
      atlas_builder.BuildAtlas (i, RESULT_FAST_IMAGE_NAME, result_atlas);
      atlas_builder.BuildAtlasImage (i, result_image);

      result_atlas.Save (RESULT_FAST_ATLAS_NAME);
      result_image.Save (RESULT_FAST_IMAGE_NAME);

      dump_hash (RESULT_FAST_IMAGE_NAME);
    }

    common::FileSystem::Remove (RESULTS_DIR);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }

  return 0;
}
