#include "shared.h"
#include <common/hash.h>

using namespace common;

const size_t image_data_size = 512 * 512 * 4;

int main ()
{
  printf ("Results of texture2d_1_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (texture2d_1_test)");

    TextureDesc desc;
    memset (&desc, 0, sizeof (desc));

    char* image_data = new char [image_data_size];
    unsigned char hash[2][16];
    
    desc.dimension            = TextureDimension_2D;
    desc.width                = 508;
    desc.height               = 508;
    desc.layers               = 0;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = true;
    
    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);
    memset (image_data, 17, image_data_size);
    texture->SetData (0, 0, 0, 0, 508, 508, PixelFormat_RGB8, image_data);
    md5 (hash[0], image_data, image_data_size);
    texture->GetData (0, 0, 0, 0, 508, 508, PixelFormat_RGB8, image_data);
    md5 (hash[1], image_data, image_data_size);

    if (memcmp (hash[0], hash[1], 16))
      printf ("RGB non power of two texture data operations works incorrect!\n");
    else
      printf ("RGB non power of two texture data operations works correct!\n");

    desc.generate_mips_enable = false;
    desc.width                = 512;
    desc.height               = 512;
    desc.format               = PixelFormat_DXT5;

    xtl::com_ptr<ITexture> texture2 (test.device->CreateTexture (desc), false);

    texture2->SetData (0, 0, 0, 0, 512, 512, PixelFormat_RGB8, image_data);
    md5 (hash[0], image_data, image_data_size);
    texture2->GetData (0, 0, 0, 0, 512, 512, PixelFormat_RGB8, image_data);
    md5 (hash[1], image_data, image_data_size);

    if (memcmp (hash[0], hash[1], 16))
      printf ("DXT texture data operations works incorrect!\n");
    else
      printf ("DXT texture data operations works correct!\n");

    texture2->SetData (0, 0, 256, 256, 256, 256, PixelFormat_DXT5, image_data);
    texture2->GetData (0, 0, 0, 0, 512, 512, PixelFormat_DXT5, image_data);
  
    desc.format               = PixelFormat_D16;
    desc.width                = 512;
    desc.height               = 512;

    xtl::com_ptr<ITexture> texture3 (test.device->CreateTexture (desc), false);

    texture3->SetData (0, 0, 0, 0, 512, 512, PixelFormat_D16, image_data);
    md5 (hash[0], image_data, image_data_size);
    texture3->GetData (0, 0, 0, 0, 512, 512, PixelFormat_D16, image_data);
    md5 (hash[1], image_data, image_data_size);

    if (memcmp (hash[0], hash[1], 16))
      printf ("Depth texture data operations works incorrect!\n");
    else
      printf ("Depth texture data operations works correct!\n");

    delete [] image_data;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
