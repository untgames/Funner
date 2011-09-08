#include "shared.h"

///Слушатель событий области вывода
class MyViewportListener: public IViewportListener
{
  public:
    void OnViewportChangeName (const char* new_name)
    {
      printf ("OnChangeName(%s)\n", new_name);
    }

    void OnViewportChangeArea (const Rect& new_area)
    {
      printf ("OnChangeArea(%d, %d, %u, %u)\n", new_area.x, new_area.y, new_area.width, new_area.height);
    }

    void OnViewportChangeCamera (scene_graph::Camera* new_camera)
    {      
      printf ("OnChangeCamera(%s)\n", new_camera ? new_camera->Name () : "null");
    }

    void OnViewportChangeZOrder (int new_z_order)
    {
      printf ("OnChangeZOrder(%d)\n", new_z_order);
    }

    void OnViewportChangeActive (bool new_state)
    {
      printf ("OnChangeActive(%s)\n", new_state ? "true" : "false");
    }
    
    void OnViewportChangeBackground (bool new_state, const math::vec4f& new_color)
    {
      printf ("OnChangeBackground(%s, %.2f, %.2f, %.2f, %.2f)\n", new_state ? "true" : "false", new_color.x, new_color.y, new_color.z, new_color.w);
    }        

    void OnViewportChangeTechnique (const char* new_technique)
    {
      printf ("OnChangeTechnique(%s)\n", new_technique);
    }

    void OnViewportChangeProperties (const common::PropertyMap&)
    {
      printf ("OnChangeProperties()\n");
    }    

    void OnViewportDestroy ()
    {
      printf ("OnDestroy()\n");
    }
};

int main ()
{
  printf ("Results of viewport_test:\n");
  
  try
  {
    MyViewportListener listener;
    
      //создание камеры и области вывода

    Viewport viewport;
    Camera::Pointer camera = OrthoCamera::Create ();

      //присоединение слушателя к области вывода      

    viewport.AttachListener (&listener);

      //изменение параметров области вывода

    viewport.SetName ("viewport1");
    viewport.SetZOrder (12);
    viewport.SetTechnique ("RenderPath1");
    viewport.SetArea (0, 0, 100, 200);
    viewport.Deactivate ();
    viewport.SetCamera (camera.get ());
    viewport.SetBackgroundColor (1, 1, 0, 1);
    viewport.EnableBackground ();
    
      //удаление камеры (проверка weak-reference области вывода)

    printf ("Delete camera\n");      

    camera = 0;

      //изменение свойств
      
    viewport.SetProperties (common::PropertyMap ());

      //печать свойств области вывода
      
    printf ("Viewport '%s':\n", viewport.Name ());
    printf ("  z-order:     %d\n", viewport.ZOrder ());
    printf ("  technique:   '%s'\n", viewport.Technique ());
    
    const Rect& vp_rect = viewport.Area ();
    
    printf ("  area:        (%d, %d, %u, %u)\n", vp_rect.x, vp_rect.y, vp_rect.width, vp_rect.height);
    printf ("  state:       %s\n", viewport.IsActive () ? "on" : "off");
    printf ("  camera:      '%s'\n", viewport.Camera () ? viewport.Camera ()->Name () : "null");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
