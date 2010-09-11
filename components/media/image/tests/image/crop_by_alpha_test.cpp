#include "shared.h"

using namespace media;
using namespace common;

const size_t TEST_IMAGE_DIMENSION   = 4;
const size_t IMAGE_DATA_BUFFER_SIZE = TEST_IMAGE_DIMENSION * TEST_IMAGE_DIMENSION * 16;

struct Rect
{
  size_t x;
  size_t y;
  size_t width;
  size_t height;
};

void print (const Rect& rect)
{
  printf ("coords is %u %u, size is %u %u\n", rect.x, rect.y, rect.width, rect.height);
}

void set_pixel_alpha (Image& image, size_t x, size_t y)
{
  char* pixel_data = (char*)image.Bitmap () + get_bytes_per_pixel (image.Format ()) * (y * image.Width () + x);

  size_t alpha_offset = 0;

  switch (image.Format ())
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGB16:
    case PixelFormat_BGR8:
    case PixelFormat_L8:
      return;
    case PixelFormat_RGBA8:
    case PixelFormat_BGRA8:
      alpha_offset = 3;
      break;
    case PixelFormat_A8:
      break;
    case PixelFormat_LA8:
      alpha_offset = 1;
      break;
    case PixelFormat_RGBA16:
      alpha_offset = 6;
      break;
    default:
      break;
  }

  *(pixel_data + alpha_offset) = 127;
}

int main ()
{
  try
  {
    printf ("Results of crop by alpha image test:\n");

    char empty_image_data [IMAGE_DATA_BUFFER_SIZE];

    memset (empty_image_data, 0, IMAGE_DATA_BUFFER_SIZE);

    for (int format = 1; format < PixelFormat_Num; format++)
    {
      printf ("testing crop for format '%s'\n", get_format_name ((PixelFormat)format));

      Image image (TEST_IMAGE_DIMENSION, TEST_IMAGE_DIMENSION, 1, (PixelFormat)format, empty_image_data);

      Rect crop_rect;

      memset (&crop_rect, 0, sizeof (crop_rect));

      crop_by_alpha (image, 1, crop_rect.x, crop_rect.y, crop_rect.width, crop_rect.height);

      printf ("empty image crop ");
      print  (crop_rect);

      set_pixel_alpha (image, 0, 0);

      memset (&crop_rect, 0, sizeof (crop_rect));

      crop_by_alpha (image, 1, crop_rect.x, crop_rect.y, crop_rect.width, crop_rect.height);

      printf ("0-0 pixel crop ");
      print  (crop_rect);

      set_pixel_alpha (image, TEST_IMAGE_DIMENSION / 2, TEST_IMAGE_DIMENSION / 2);

      memset (&crop_rect, 0, sizeof (crop_rect));

      crop_by_alpha (image, 1, crop_rect.x, crop_rect.y, crop_rect.width, crop_rect.height);

      printf ("0-0 and %u-%u image crop ", TEST_IMAGE_DIMENSION / 2, TEST_IMAGE_DIMENSION / 2);
      print  (crop_rect);

      crop_rect.x      = 1;
      crop_rect.y      = 1;
      crop_rect.width  = 2;
      crop_rect.height = 2;

      crop_by_alpha (image, 1, crop_rect.x, crop_rect.y, crop_rect.width, crop_rect.height);

      printf ("0-0 and %u-%u growing image crop ", TEST_IMAGE_DIMENSION / 2, TEST_IMAGE_DIMENSION / 2);
      print  (crop_rect);

      memset (image.Bitmap (), 0, image.Width () * image.Height () * get_bytes_per_pixel (image.Format ()));

      set_pixel_alpha (image, TEST_IMAGE_DIMENSION / 2, TEST_IMAGE_DIMENSION / 2);

      memset (&crop_rect, 0, sizeof (crop_rect));

      crop_by_alpha (image, 1, crop_rect.x, crop_rect.y, crop_rect.width, crop_rect.height);

      printf ("%u-%u image crop ", TEST_IMAGE_DIMENSION / 2, TEST_IMAGE_DIMENSION / 2);
      print  (crop_rect);
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
  return 0;
}
