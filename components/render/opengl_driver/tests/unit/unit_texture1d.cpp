#include "shared.h"

using namespace common;

const size_t image_data_size = 512 * 3;

int main ()
{
  printf ("Results of unit_texture1d_test:\n");
  
  try
  {
    Test test;

    TextureDesc desc;
    memset (&desc, 0, sizeof (desc));

    char* image_data = new char [image_data_size];
    unsigned char hash[2][16];
    
    desc.dimension            = TextureDimension_1D;
    desc.width                = 512;
    desc.height               = 1;
    desc.layers               = 1;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = false;
    desc.access_flags         = AccessFlag_ReadWrite;
    
    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);

    memset (image_data, 17, image_data_size);    
    
    texture->SetData (0, 0, 0, 0, 512, 1, PixelFormat_RGB8, image_data);

    md5 (hash[0], image_data, image_data_size);
    
    texture->GetData (0, 0, 0, 0, 512, 1, PixelFormat_RGB8, image_data);
    
    md5 (hash[1], image_data, image_data_size);

    if (memcmp (hash[0], hash[1], 16))
      printf ("RGB 1d texture data operations works incorrect!\n");
    else
      printf ("RGB 1d texture data operations works correct!\n");

    delete [] image_data;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
