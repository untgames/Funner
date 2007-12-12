#include <exception>
#include <stdio.h>
#include <shared/texture_manager.h>

using namespace render::low_level;
using namespace render::low_level::opengl;

int main ()
{
  printf ("Ressults of texture manager test:\n");

  try
  {
    TextureManager manager;
    TextureDesc    tex_desc = {TextureDimension_2D, 1024, 1024, 0, PixelFormat_RGB8, false};
    ITexture* texture (manager.CreateTexture (tex_desc));
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}