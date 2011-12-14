#include "shared.h"

const char* IMAGE_NAME = "data/pic1.dds";

int main ()
{
  printf ("Results of compressed_texture1_test:\n");
  
  try
  {
    Test test (L"Compressed texture test", false);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource (IMAGE_NAME);

    Texture texture = render_manager.CreateSharedTexture (IMAGE_NAME);

    printf ("Texture:\n");
    printf ("  width:  %u\n", texture.Width ());
    printf ("  height: %u\n", texture.Height ());
    printf ("  depth:  %u\n", texture.Depth ());
    printf ("  format: %s\n", get_name (texture.Format ()));
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
