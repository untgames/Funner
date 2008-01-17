#include "shared.h"
#include <common/hash.h>

using namespace common;

const size_t image_data_size = 512 * 512 * 6 * 4;

int main ()
{
  printf ("Results of texture_cubemap_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (texture_cubemap_test)");

    TextureDesc desc;
    memset (&desc, 0, sizeof (desc));

    char* image_data = new char [image_data_size];
    unsigned char hash[2][16];
    
    desc.dimension            = TextureDimension_Cubemap;
    desc.width                = 508;
    desc.height               = 508;
    desc.layers               = 6;
    desc.format               = PixelFormat_DXT5;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = false;
    
    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);
    memset (image_data, 17, image_data_size);
    texture->SetData (3, 0, 0, 0, 508, 508, PixelFormat_DXT5, image_data + desc.width * desc.height * 3);
    md5 (hash[0], image_data, image_data_size);
    texture->GetData (3, 0, 0, 0, 508, 508, PixelFormat_DXT5, image_data + desc.width * desc.height * 3);
    md5 (hash[1], image_data, image_data_size);

    if (memcmp (hash[0], hash[1], 16))
      printf ("DXT non power of two cubemap texture data operations works incorrect!\n");
    else
      printf ("DXT non power of two cubemap texture data operations works correct!\n");

    desc.generate_mips_enable = true;
    desc.width                = 512;
    desc.height               = 512;
    desc.format               = PixelFormat_RGBA8;

    xtl::com_ptr<ITexture> texture2 (test.device->CreateTexture (desc), false);

    texture2->SetData (2, 0, 0, 0, 512, 512, PixelFormat_RGBA8, image_data + desc.width * desc.height * 2);
    md5 (hash[0], image_data, image_data_size);
    texture2->GetData (2, 0, 0, 0, 512, 512, PixelFormat_RGBA8, image_data + desc.width * desc.height * 2);
    md5 (hash[1], image_data, image_data_size);

    if (memcmp (hash[0], hash[1], 16))
      printf ("RGBA cubemap texture data operations works incorrect!\n");
    else
      printf ("RGBA cubemap texture data operations works correct!\n");

    delete [] image_data;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
