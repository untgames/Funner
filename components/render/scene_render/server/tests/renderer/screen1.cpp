#include "shared.h"

void log_print (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
  fflush (stdout);
}

int main ()
{
  printf ("Results of screen1_test:\n");

  try
  {
    common::LogFilter log_filter ("render.*", &log_print);

    syslib::Window window (syslib::WindowStyle_Overlapped, 400, 300);

    const char* SERVER_NAME = "MyServer";

    Server server (SERVER_NAME);
    Client client (SERVER_NAME);

    common::PropertyMap window_properties;
    
    window_properties.SetProperty ("ColorBits", 24);
    window_properties.SetProperty ("DepthBits", 16);

    server.AttachWindow ("my_window", window, window_properties);

    RenderTarget target = client.CreateRenderTarget ("my_window");

    scene_graph::Screen screen;
    
    target.SetScreen (&screen);
    
    screen.SetBackgroundState (false);    
    
    scene_graph::Viewport vp;
    
    common::PropertyMap vp_properties;

    vp.SetName       ("viewport1");
    vp.SetArea       (0, 0, 100, 100);
    vp.SetTechnique  ("default");
    vp.SetProperties (vp_properties);    
    
    screen.Attach (vp);

    target.Update ();

    server.Finish ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }
}
