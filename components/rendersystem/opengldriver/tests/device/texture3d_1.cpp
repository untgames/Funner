#include "shared.h"
#include <common/hash.h>

using namespace common;

const size_t LAYERS_COUNT = 4;
const size_t TEX_SIZE     = 32;

int main ()
{
  printf ("Results of texture3d_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (texture3d_test)");//, "max_version=1.1 disable=* GL_EXT_texture3D=1");

    TextureDesc desc;
    memset (&desc, 0, sizeof (desc));

    xtl::uninitialized_storage<char> image_data (TEX_SIZE * TEX_SIZE);
    
    unsigned char hash[2][16];
    
    desc.dimension            = TextureDimension_3D;
    desc.width                = TEX_SIZE;
    desc.height               = TEX_SIZE;
    desc.layers               = LAYERS_COUNT;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = false;

    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);

    for (size_t i=0; i<image_data.size (); i++)
      image_data.data () [i]   = char (0xfa);

    for (size_t i = 0; i < LAYERS_COUNT; i++)
      texture->SetData (i, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_L8, image_data.data ());
      
    md5 (hash[0], image_data.data (), image_data.size ());
    
    texture->GetData (1, 0, 0, 0, TEX_SIZE, TEX_SIZE, PixelFormat_L8, image_data.data ());
    
    md5 (hash[1], image_data.data (), image_data.size ());

    if (memcmp (hash[0], hash[1], 16))
      printf ("RGB non power of two 3d texture data operations works incorrect!\n");
    else
      printf ("RGB non power of two 3d texture data operations works correct!\n");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
