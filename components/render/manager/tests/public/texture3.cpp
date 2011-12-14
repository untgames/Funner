#include "shared.h"

int main ()
{
  printf ("Results of local_texture_test:\n");
  
  try
  {
    common::LogFilter log_filter ("render.manager*", &log_print);    
    
    Test test (L"Local texture test", false);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource ("data/render.rfx");

    Texture texture = render_manager.CreateTexture ("shadow_map");

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
