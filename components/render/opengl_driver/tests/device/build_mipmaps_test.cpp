#include "shared.h"

using namespace common;

const size_t TEX_SIZE        = 128;
const size_t IMAGE_DATA_SIZE = TEX_SIZE * TEX_SIZE * 4;

int main ()
{
  printf ("Results of build_mipmaps_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (build_mipmaps_test)", "max_version=1.1 disable=*");

    TextureDesc desc;
    
    memset (&desc, 0, sizeof (desc));
  
    char* image_data = new char [IMAGE_DATA_SIZE];
    
    desc.dimension            = TextureDimension_2D;
    desc.width                = TEX_SIZE;
    desc.height               = TEX_SIZE;
    desc.layers               = 1;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = true;
    desc.access_flags         = AccessFlag_ReadWrite;    
    
    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);

    texture->SetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGB8, image_data);

    delete [] image_data;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
