#include "shared.h"

///Слушатель событий области вывода
class MyViewportListener: public IViewportListener, public xtl::reference_counter
{
  public:
    MyViewportListener ()
    {
      printf ("MyViewportListener::MyViewportListener\n");
    }

    ~MyViewportListener ()
    {
      printf ("MyViewportListener::~MyViewportListener\n");
    }

    void OnChangeName (const char* new_name)
    {
      printf ("OnChangeName(%s)\n", new_name);
    }

    void OnChangeArea (const Rect& new_area)
    {
      printf ("OnChangeArea(%d, %d, %u, %u)\n", new_area.left, new_area.top, new_area.width, new_area.height);
    }

    void OnChangeCamera (scene_graph::Camera* new_camera)
    {      
      printf ("OnChangeCamera(%s)\n", new_camera ? new_camera->Name () : "null");
    }

    void OnChangeZOrder (int new_z_order)
    {
      printf ("OnChangeZOrder(%d)\n", new_z_order);
    }

    void OnChangeActive (bool new_state)
    {
      printf ("OnChangeActive(%s)\n", new_state ? "true" : "false");
    }

    void OnChangeRenderPath (const char* new_path_name)
    {
      printf ("OnChangeRenderPath(%s)\n", new_path_name);
    }

    void OnChangeProperty (const char* name, const char* new_value)
    {
      printf ("OnChangeProperty(%s, %s)\n", name, new_value);
    }

    void OnDestroy ()
    {
      printf ("OnDestroy()\n");
    }

///Подсчёт ссылок
    void AddRef  () { addref (this); }
    void Release () { release (this); }
};

int main ()
{
  printf ("Results of viewport_test:\n");
  
  try
  {
      //создание камеры и области вывода
    
    Viewport viewport;
    Camera::Pointer camera = OrthoCamera::Create ();

      //присоединение слушателя к области вывода

    viewport.AttachListener (xtl::com_ptr<MyViewportListener> (new MyViewportListener, false).get ());

      //изменение параметров области вывода

    viewport.SetName ("viewport1");
    viewport.SetZOrder (12);
    viewport.SetRenderPath ("RenderPath1");
    viewport.SetArea (0, 0, 100, 200);
    viewport.Deactivate ();
    viewport.SetCamera (camera.get ());
    
      //удаление камеры (проверка weak-reference области вывода)

    printf ("Delete camera\n");      

    camera = 0;

      //изменение свойств

    viewport.SetProperty ("prop1", "value1");
    viewport.SetProperty ("prop2", "value2");
    
      //печать свойств области вывода
      
    printf ("Viewport '%s':\n", viewport.Name ());
    printf ("  z-order:     %d\n", viewport.ZOrder ());
    printf ("  render-path: '%s'\n", viewport.RenderPath ());
    
    const Rect& vp_rect = viewport.Area ();
    
    printf ("  area:        (%d, %d, %u, %u)\n", vp_rect.left, vp_rect.top, vp_rect.width, vp_rect.height);
    printf ("  state:       %s\n", viewport.IsActive () ? "on" : "off");
    printf ("  camera:      '%s'\n", viewport.Camera () ? viewport.Camera ()->Name () : "null");

      //печать свойств

    printf ("Viewport properties:\n");

    for (Viewport::PropertyIterator iter = viewport.CreatePropertyIterator (); iter; ++iter)
      printf ("'%s'='%s'\n", iter->Name (), iter->Value ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
