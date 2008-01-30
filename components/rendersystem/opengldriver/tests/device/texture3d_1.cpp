#include "shared.h"
#include <common/hash.h>

using namespace common;

const size_t layers_count    = 4;
const size_t image_data_size = 768 * 768 * 3;

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
    desc.width                = 768;
    desc.height               = 768;
    desc.layers               = layers_count;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = false;
    
    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);
    
    memset (image_data, 0XFF, image_data_size);
    
    for (size_t i = 0; i < layers_count; i++)
      texture->SetData (i, 0, 0, 0, 768, 768, PixelFormat_RGB8, image_data);
      
//    md5 (hash[0], image_data, image_data_size);
    
    texture->GetData (0, 0, 0, 0, 768, 768, PixelFormat_RGB8, image_data);
    
//    md5 (hash[1], image_data, image_data_size);

/*    if (memcmp (hash[0], hash[1], 16))
      printf ("RGB non power of two 3d texture data operations works incorrect!\n");
    else
      printf ("RGB non power of two 3d texture data operations works correct!\n");

    delete [] image_data;
*/  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
