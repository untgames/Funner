#include "shared.h"

//слушатель событий рабочего стола
class MyScreenListener: public IScreenListener
{
  public:
    void OnChangeName (const char* new_name)
    {
      printf ("OnChangeName(%s)\n", new_name);
    }
    
    void OnChangeArea (const Rect& rect)
    {
      printf ("OnChangeArea(%d, %d, %u, %u)\n", rect.left, rect.top, rect.width, rect.height);
    }
    
    void OnChangeBackground (bool new_state, const math::vec4f& new_color)
    {
      printf ("OnChangeBackground(%s, %.2f, %.2f, %.2f, %.2f)\n", new_state ? "true" : "false", new_color.x, new_color.y, new_color.z, new_color.w);
    }

    void OnAttachViewport (Viewport& viewport)
    {
      printf ("OnAttachViewport(%s)\n", viewport.Name ());
    }

    void OnDetachViewport (Viewport& viewport)
    {
      printf ("OnDetachViewport(%s)\n", viewport.Name ());
    }
    
    void OnDestroy ()
    {
      printf ("OnDestroy()\n");
    }
};

void print_viewports (const Screen& Screen)
{
  printf ("Screen '%s' viewports (count=%u):", Screen.Name (), Screen.ViewportsCount ());

  for (size_t i=0; i<Screen.ViewportsCount (); i++)
    printf (" '%s'", Screen.Viewport (i).Name ());

  printf ("\n");          
}

int main ()
{
  printf ("Results of Screen_test:\n");

  try
  {
    MyScreenListener listener;
    
      //создание рабочего стола
      
    Screen screen1;
    
      //регистрация слушателя
      
    screen1.AttachListener (&listener);
    
      //изменение базовых параметров
      
    screen1.SetArea            (10, 20, 300, 40);
    screen1.SetBackgroundColor (0.5f, 0.25f, 1.0f, 0.3f);
    screen1.DisableBackground  ();
    screen1.SetName            ("Screen1");

      //присоединение областей вывода

    Viewport viewport1, viewport2;

    viewport1.SetName ("Viewport1");
    viewport2.SetName ("Viewport2");

    screen1.Attach (viewport1);
    screen1.Attach (viewport2);

      //перебор областей вывода

    print_viewports (screen1);

      //копирование рабочего стола

    Screen screen2 = screen1;

      //удаление областей вывода

    screen2.Detach (viewport1);

      //перебор областей вывода

    print_viewports (screen1);

      //очистка оставшихся областей вывода с оповещением об удалении

    screen1.DetachAllViewports ();        
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
