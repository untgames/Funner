#include "shared.h"

int main ()
{
  printf ("Results of texture2d_1_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (texture2d_1_test)");

    TextureDesc desc;
    memset (&desc, 0, sizeof (desc));

    char* image_data = new char [512 * 512 * 3];
    
    desc.dimension            = TextureDimension_2D;
    desc.width                = 512;
    desc.height               = 512;
    desc.layers               = 0;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = true;
    
    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);
    texture->SetData (0, 0, 256, 256, 256, 256, PixelFormat_RGB8, image_data);
    texture->GetData (0, 0, 0, 0, 512, 512, PixelFormat_RGB8, image_data);

    desc.generate_mips_enable = false;
    desc.format               = PixelFormat_DXT5;

    xtl::com_ptr<ITexture> texture2 (test.device->CreateTexture (desc), false);

    texture2->SetData (0, 0, 256, 256, 256, 256, PixelFormat_DXT5, image_data);
    texture2->GetData (0, 0, 0, 0, 512, 512, PixelFormat_DXT5, image_data);
  
    delete [] image_data;

    desc.dimension            = TextureDimension_1D;
    desc.width                = 4096;
  
    xtl::com_ptr<ITexture> error_texture (test.device->CreateTexture (desc), false);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
