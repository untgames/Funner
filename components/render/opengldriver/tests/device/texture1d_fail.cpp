#include "shared.h"

using namespace common;

int main ()
{
  printf ("Results of texture1d_test:\n");
  
  try
  {
    Test test (L"OpenGL device test window (texture1d_test)");

    TextureDesc desc;

    memset (&desc, 0, sizeof (desc));

    desc.dimension            = TextureDimension_1D;
    desc.width                = 20000;
    desc.height               = 1;
    desc.layers               = 1;
    desc.format               = PixelFormat_RGB8;
    desc.bind_flags           = BindFlag_Texture;
    desc.generate_mips_enable = true;
    desc.access_flags         = AccessFlag_ReadWrite;    

    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
