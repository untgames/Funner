#include "shared.h"

using namespace common;

int main ()
{
  printf ("Results of scaled_texture_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (scaled_texture_test)", "max_version=1.1 disable=*");
    
//    media::Image src_image ("pic2.tga");
    media::Image src_image ("data/pic1.jpg");
    
    size_t tex_width  = src_image.Width (),
           tex_height = src_image.Height ();

    TextureDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.dimension            = TextureDimension_2D;
    desc.width                = tex_width;
    desc.height               = tex_height;
    desc.layers               = 1;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = true;

    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);    

    texture->SetData (0, 0, 0, 0, tex_width, tex_height, PixelFormat_RGB8, src_image.Bitmap ());
    
    media::Image test_image = src_image;
    
    test_image.Save ("src.tga");

    media::Image dst_image (tex_width, tex_height, 1, media::PixelFormat_RGB8);

    texture->GetData (0, 0, 0, 0, tex_width, tex_height, PixelFormat_RGB8, dst_image.Bitmap ());

    dst_image.Save ("dst.tga");

    media::Image diff_image (tex_width, tex_height, 1, media::PixelFormat_RGB8);
    
    char *src = (char*)src_image.Bitmap (), *dst = (char*)dst_image.Bitmap (), *diff = (char*)diff_image.Bitmap ();
    
    for (size_t i=0; i<tex_height; i++)
      for (size_t j=0; j<tex_width; j++)
        for (size_t k=0; k<3; k++, src++, dst++, diff++)
          *diff = *dst - *src;

    diff_image.Save ("diff.tga");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
