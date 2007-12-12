#include "shared.h"

int main ()
{
  printf ("Results of texture2d_1_test:\n");
  
  try
  {
    Test test ("OpenGL device test window (texture2d_1_test)");

    TextureDesc desc;

    memset (&desc, 0, sizeof (desc));
    
    desc.dimension            = TextureDimension_2D;
    desc.width                = 512;
    desc.height               = 512;
    desc.layers               = 0;
    desc.format               = PixelFormat_RGB8;
    desc.generate_mips_enable = false;
    
    xtl::com_ptr<ITexture> texture (test.device->CreateTexture (desc), false);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
