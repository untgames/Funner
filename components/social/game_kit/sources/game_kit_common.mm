#include "shared.h"

namespace social
{

namespace game_kit
{

void release_ns_object (const void* handle)
{
  [(NSObject*)handle release];
}

void fill_user (GKPlayer* player, User& user)
{
  user.SetId       ([player.playerID UTF8String]);
  user.SetNickname ([player.alias UTF8String]);
  user.SetHandle   (player, &release_ns_object);
}

media::Image convert_image (UIImage* image)
{
  static const char* METHOD_NAME = "social::game_kit::convert_image";

  CGImageRef   cg_image   = image.CGImage;
  CGBitmapInfo image_info = CGImageGetBitmapInfo (cg_image);

  size_t bpp                = CGImageGetBitsPerPixel (cg_image),
         bits_per_component = CGImageGetBitsPerComponent (cg_image);

  if (bits_per_component != 8)
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported bits per component %u", bits_per_component);

  if (bpp != 24 && bpp != 32)
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported bits per pixel %u", bpp);

  if ((CGColorSpaceGetModel (CGImageGetColorSpace (cg_image)) != kCGColorSpaceModelRGB) ||
      (bpp == 24 && ((image_info & kCGBitmapAlphaInfoMask) != kCGImageAlphaNone)) ||
      (bpp == 32 && ((image_info & kCGBitmapAlphaInfoMask) != kCGImageAlphaLast)))
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported color space");

  media::PixelFormat pixel_format = bpp == 24 ? media::PixelFormat_RGB8 : media::PixelFormat_RGBA8;

  size_t row_bytes    = CGImageGetBytesPerRow (cg_image),
         image_width  = CGImageGetWidth (cg_image),
         image_height = CGImageGetHeight (cg_image);

  if (row_bytes != image_width * bpp / 8)
    throw xtl::format_operation_exception (METHOD_NAME, "Returned image has padding");

  CGDataProviderRef image_data_provider = CGImageGetDataProvider (cg_image);
  CFDataRef         image_data          = CGDataProviderCopyData (image_data_provider);

  try
  {
    media::Image image (image_width, image_height, 1, pixel_format, CFDataGetBytePtr (image_data));

    CFRelease (image_data);

    return image;
  }
  catch (...)
  {
    if (image_data)
      CFRelease (image_data);

    throw;
  }
}

}

}
