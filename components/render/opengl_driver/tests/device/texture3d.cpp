#include "shared.h"

using namespace common;

const size_t layers_count    = 4;
const size_t image_data_size = 512 * 512 * 4;

int main ()
{
  printf ("Results of texture3d_test:\n");

  try
  {
    Test test (L"OpenGL device test window (texture3d_test)");//, "max_version=1.1 disable=* GL_EXT_texture3D=1");

    TextureDesc desc;
    memset (&desc, 0, sizeof (desc));

    char* image_data = new char [image_data_size];
    unsigned char hash[2][16];

    desc.dimension            = TextureDimension_3D;
    desc.width                = 508;
    desc.height               = 504;
    desc.layers               = layers_count;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = false;
    desc.access_flags         = AccessFlag_ReadWrite;

    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);

    memset (image_data, 0, image_data_size);

    for (size_t i = 0; i < layers_count; i++)
      texture->SetData (i, 0, 0, 0, 508, 504, PixelFormat_RGB8, image_data);

    md5 (image_data, image_data_size, hash[0]);

    texture->GetData (0, 0, 0, 0, 508, 504, PixelFormat_RGB8, image_data);

    md5 (image_data, image_data_size, hash[1]);

    if (memcmp (hash[0], hash[1], 16))
      printf ("RGB non power of two 3d texture data operations works incorrect!\n");
    else
      printf ("RGB non power of two 3d texture data operations works correct!\n");

    desc.generate_mips_enable = true;
    desc.width                = 512;
    desc.height               = 512;
    desc.format               = PixelFormat_RGBA8;

    xtl::com_ptr<ITexture> texture2 (test.device->CreateTexture (desc), false);

    for (size_t i=0; i < layers_count; i++)
      texture2->SetData (i, 0, 0, 0, 512, 512, PixelFormat_RGBA8, image_data);

    md5 (image_data, image_data_size, hash[0]);
    texture2->GetData (2, 0, 0, 0, 512, 512, PixelFormat_RGBA8, image_data);

    md5 (image_data, image_data_size, hash[1]);

    if (memcmp (hash[0], hash[1], 16))
      printf ("RGBA 3d texture data operations works incorrect!\n");
    else
      printf ("RGBA 3d texture data operations works correct!\n");

    delete [] image_data;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
