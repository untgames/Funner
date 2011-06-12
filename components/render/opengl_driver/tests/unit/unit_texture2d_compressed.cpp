#include "shared.h"

const char* IMAGE_NAME = "data/pic1.dds";

int main ()
{
  printf ("Results of unit_texture2d_compressed_test:\n");
  
  try
  {
    Test test ("disable=*");

    media::CompressedImage image (IMAGE_NAME);

    PixelFormat pixel_format;

    const char* image_format = image.Format ();

    if (!xtl::xstricmp (image_format, "dxt1"))
      pixel_format = PixelFormat_DXT1;
    else if (!xtl::xstricmp (image_format, "dxt3"))
      pixel_format = PixelFormat_DXT3;
    else if (!xtl::xstricmp (image_format, "dxt5"))
      pixel_format = PixelFormat_DXT5;
    else
      throw xtl::format_operation_exception ("", "Unsupported dds texture format '%s'", image_format);

    TextureDesc texture_desc;
    
    memset (&texture_desc, 0, sizeof texture_desc);
    
    texture_desc.dimension            = TextureDimension_2D;
    texture_desc.width                = image.Width ();
    texture_desc.height               = image.Height ();
    texture_desc.layers               = 1;
    texture_desc.format               = pixel_format;
    texture_desc.bind_flags           = BindFlag_Texture;
    texture_desc.access_flags         = AccessFlag_ReadWrite;
    texture_desc.generate_mips_enable = false;

    stl::vector<size_t> sizes (image.BlocksCount ());

    const media::CompressedImageBlockDesc* blocks = image.Blocks ();
    
    size_t mips_count = image.BlocksCount ();    
    
    printf ("Load %u mips:\n", mips_count);    

    for (size_t i=0, count=sizes.size (); i<count; i++)
    {
      sizes [i] = blocks [i].size;
      
      printf ("  level%02u: size=%u\n", i, sizes [i]);
    }

    TextureData data;

    memset (&data, 0, sizeof (TextureData));

    data.data  = image.Data ();
    data.sizes = &sizes [0];

    TexturePtr texture (test.device->CreateTexture (texture_desc, data), false);    
    
    xtl::uninitialized_storage<char> buffer (get_image_size (image.Width (), image.Height (), PixelFormat_RGB8));
    
    size_t width = image.Width (), height = image.Height ();    
    
    printf ("Set %u mips:\n", mips_count);
    
    for (size_t i=0; i<mips_count; i++)
    {            
      size_t size = get_image_size (width, height, PixelFormat_RGB8);
      
      texture->GetData (0, i, 0, 0, width, height, PixelFormat_RGB8, buffer.data ());
           
      printf ("  level%02u: width=%u, height=%u, hash=%08x\n", i, width, height, common::crc32 (buffer.data (), size));
      
      width  = width > 1 ? width / 2 : width;
      height = height > 1 ? height / 2 : height;      
    }
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
