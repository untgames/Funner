#include "shared.h"

const char* IMAGE_NAME = "data/bottom.jpg";

int main ()
{
  printf ("Results of texture1_test:\n");
  
  try
  {
    Test test (L"Texture1 test");
    
    RenderManager render_manager = test.RenderManager ();
    
    media::Image image (IMAGE_NAME);
    
    Texture texture = render_manager.CreateTexture (image);
    
    printf ("Texture:\n");
    printf ("  width:  %u\n", texture.Width ());
    printf ("  height: %u\n", texture.Height ());
    printf ("  depth:  %u\n", texture.Depth ());
    printf ("  format: %s\n", get_name (texture.Format ()));

//    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
