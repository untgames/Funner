#include "shared.h"

using namespace common;

int main ()
{
  printf ("Results of unit_texture1d_fail_test:\n");

  try
  {
    Test test;

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
    printf ("exception.\n");
  }

  return 0;
}
