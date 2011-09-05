#include "shared.h"

///Пользовательский рендер
class MySceneRender: public ISceneRender, public xtl::reference_counter
{
  public:
///Конструктор
    MySceneRender (RenderManager& manager)
      : frame (manager.CreateFrame ())
      , camera ()
    {
    }
  
///Кадр
    render::Frame& Frame ()
    {
      return frame;
    }
    
///Обновление камеры
    void UpdateCamera (scene_graph::Camera* in_camera)
    {
      printf ("MySceneRender::UpdateCamera(%s)\n", camera ? camera->Name () : "0");

      camera = in_camera;
    }

///Обновление свойств ренедринга
    void UpdateProperties (const common::PropertyMap& new_properties)
    {
      printf ("MySceneRender::UpdateProperties()\n");
      
      properties = new_properties;
    }

///Обновление содержимого кадра
    void UpdateFrame ()
    {
      printf ("MySceneRender::UpdateFrame()\n");
    }

///Подсчёт ссылок
    void AddRef  ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }
    
///Порождающая функция
    static ISceneRender* Create (RenderManager& manager, const char*)
    {
      return new MySceneRender (manager);
    }

  private:
    render::Frame        frame;
    scene_graph::Camera* camera;
    common::PropertyMap  properties;
};

int main ()
{
  printf ("Results of custom_render_test:\n");
  
  try
  {
    Test test (L"Window1 test");
    
    SceneRenderManager::RegisterRender ("MySceneRender", &MySceneRender::Create);
    
    SceneRender render;
    
    RenderManager manager = test.RenderManager ();
    
    render.SetRenderManager (&manager);
    
    scene_graph::Screen screen;        
    
    render.SetScreen (&screen);
    
    screen.SetBackgroundState (false);    
    
    scene_graph::OrthoCamera::Pointer camera1 = scene_graph::OrthoCamera::Create (), camera2 = scene_graph::OrthoCamera::Create ();
    
      //настройка области вывода

    camera1->SetName ("Camera1");
    camera2->SetName ("Camera2");
    
    scene_graph::Viewport vp1;

    vp1.SetArea     (0, 0, 100, 100);
    vp1.SetCamera   (&*camera1);
    vp1.SetRenderer ("MySceneRender");
    
    screen.Attach (vp1);

    scene_graph::Viewport vp2;

    vp2.SetArea     (10, 10, 50, 50);
    vp2.SetCamera   (&*camera2);
    vp2.SetRenderer ("MySceneRender");
    
    screen.Attach (vp2); 

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
