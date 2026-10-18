#include "shared.h"

unsigned int removed_count = 0;

void on_window_removed (RenderManager& manager, Window& window)
{
  ++removed_count;

  printf ("removed '%s' %ux%u, remaining windows: %u\n", window.Name (), window.Width (), window.Height (), (unsigned int)manager.WindowsCount ());
  printf ("color buffer %ux%u\n", window.ColorBuffer ().Width (), window.ColorBuffer ().Height ());
  printf ("depth buffer %ux%u\n", window.DepthStencilBuffer ().Width (), window.DepthStencilBuffer ().Height ());
}

void on_destroy_throw (WindowImpl* _)
{
  printf ("throwing destruction handler\n");
  throw 1;
}

void on_application_initialized ()
{
  try
  {
    syslib::Window native_window (syslib::WindowStyle_PopUp, WINDOW_WIDTH, WINDOW_HEIGHT);

    PropertyMap properties;

    properties.SetProperty ("ColorBits", 24);
    properties.SetProperty ("DepthBits", 16);

    printf ("--- Destroy window first ---\n");

    {
      RenderManager manager;

      manager.RegisterWindowEventHandler (RenderManagerWindowEvent_OnRemoved, &on_window_removed);

      {
        Window window = manager.CreateWindow (native_window, properties);

        window.SetName ("main");

        {
          Window copy (window);
        }

        printf ("before last reference: %u removals\n", removed_count);
      }

      printf ("after last reference: %u removals, %u windows\n", removed_count, (unsigned int)manager.WindowsCount ());
    }

    printf ("--- Destroy manager first ---\n");

    {
      stl::auto_ptr<Window> window;

      {
        RenderManager manager;

        manager.RegisterWindowEventHandler (RenderManagerWindowEvent_OnRemoved, &on_window_removed);

        window.reset (new Window (manager.CreateWindow (native_window, properties)));
        window->SetName ("survivor");
      }

      printf ("surviving window '%s' %ux%u\n", window->Name (), window->Width (), window->Height ());

      // No on_window_removed should be called for the surviving window because it is destroyed after the manager.
      window.reset ();

      printf ("after surviving window: %u removals\n", removed_count);
    }

    printf ("--- Throw from destruction handler ---\n");

    {
      RenderManager manager;

      {
        Window window = manager.CreateWindow (native_window, properties);

        Wrappers::Unwrap<WindowImpl> (window)->RegisterDestroyHandler (&on_destroy_throw);
      }

      printf ("after throwing handler: %u windows\n", (unsigned int)manager.WindowsCount ());
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of window_destroy_test:\n");

  try
  {
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialize, &on_application_initialized);

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
