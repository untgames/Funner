#include "shared.h"

using namespace common;

const size_t TEX_SIZE        = 128;
const size_t IMAGE_DATA_SIZE = TEX_SIZE * TEX_SIZE * 4;

int main ()
{
  printf ("Results of texture2d_1_test:\n");

  try
  {
    Test test (L"OpenGL device test window (texture2d_1_test)");
//    Test test (L"OpenGL device test window (texture2d_1_test)", "max_version=1.1 disable=*");
//    Test test (L"OpenGL device test window (texture2d_1_test)", "disable='GL_ARB_texture_non_power_of_two' max_version=1.1");
//    Test test (L"OpenGL device test window (texture2d_1_test)", "disable='GL_ARB_texture_non_power_of_two GL_EXT_texture_compression_s3tc GL_EXT_texture_rectangle' max_version=1.1");

    TextureDesc desc;
    memset (&desc, 0, sizeof (desc));

    char* image_data = new char [IMAGE_DATA_SIZE];
    unsigned char hash[2][16];

    desc.dimension            = TextureDimension_2D;
    desc.width                = TEX_SIZE;
    desc.height               = TEX_SIZE;
    desc.layers               = 1;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = false;
    desc.access_flags         = AccessFlag_ReadWrite;

    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);

    memset (image_data, 17, IMAGE_DATA_SIZE);

    texture->SetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGB8, image_data);

    md5 (image_data, IMAGE_DATA_SIZE, hash[0]);

    texture->GetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGB8, image_data);

    md5 (image_data, IMAGE_DATA_SIZE, hash[1]);

    if (memcmp (hash[0], hash[1], 16))
      printf ("RGB non power of two 2d texture data operations works incorrect!\n");
    else
      printf ("RGB non power of two 2d texture data operations works correct!\n");

    desc.generate_mips_enable = false;
    desc.format               = PixelFormat_DXT3;
    desc.width                = TEX_SIZE;
    desc.height               = TEX_SIZE;

    xtl::com_ptr<ITexture> texture2 (test.device->CreateTexture (desc), false);

    memset (image_data, 0x01010101, IMAGE_DATA_SIZE);
    texture2->SetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGBA8, image_data);
    md5 (image_data, IMAGE_DATA_SIZE, hash[0]);
    texture2->GetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGBA8, image_data);
    md5 (image_data, IMAGE_DATA_SIZE, hash[1]);

    if (memcmp (hash[0], hash[1], 16))
      printf ("RGB8 to DXT 2d texture data operations works incorrect!\n");
    else
      printf ("RGB8 to DXT 2d texture data operations works correct!\n");

    texture2->SetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_DXT3, image_data);
    md5 (image_data, IMAGE_DATA_SIZE, hash[0]);
    texture2->GetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_DXT3, image_data);
    md5 (image_data, IMAGE_DATA_SIZE, hash[1]);

/*    texture2->SetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGBA8, image_data);
    md5 (hash[0], image_data, IMAGE_DATA_SIZE);
    texture2->GetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_RGBA8, image_data);
    md5 (hash[1], image_data, IMAGE_DATA_SIZE);

    if (memcmp (hash[0], hash[1], 16))
      printf ("RGB8 to DXT 2d texture data operations works incorrect!\n");
    else
      printf ("RGB8 to DXT 2d texture data operations works correct!\n");

    texture2->SetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_DXT3, image_data);
    md5 (hash[0], image_data, IMAGE_DATA_SIZE);
    texture2->GetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_DXT3, image_data);
    md5 (hash[1], image_data, IMAGE_DATA_SIZE);*/

    if (memcmp (hash[0], hash[1], 16))
      printf ("DXT to DXT 2d texture data operations works incorrect!\n");
    else
      printf ("DXT to DXT 2d texture data operations works correct!\n");

    desc.format  = PixelFormat_D16;
    desc.width   = TEX_SIZE;
    desc.height  = TEX_SIZE;

    xtl::com_ptr<ITexture> texture3 (test.device->CreateTexture (desc), false);

    texture3->SetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_D16, image_data);
    md5 (image_data, IMAGE_DATA_SIZE, hash[0]);

    texture3->GetData (0, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_D16, image_data);
    md5 (image_data, IMAGE_DATA_SIZE, hash[1]);

    if (memcmp (hash[0], hash[1], 16))
      printf ("Depth 2d texture data operations works incorrect!\n");
    else
      printf ("Depth 2d texture data operations works correct!\n");

    delete [] image_data;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
