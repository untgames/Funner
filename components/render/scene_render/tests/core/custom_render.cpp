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
      printf ("MySceneRender::MySceneRender\n");
    }
    
///Деструктор
    ~MySceneRender ()
    {
      printf ("MySceneRender::~MySceneRender\n");
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
      printf ("MySceneRender::UpdateFrame(%s):\n", camera ? camera->Name () : "0");
      
      render::Viewport viewport = frame.Viewport ("default_color");
      
      printf ("  viewport: x=%u, y=%u, width=%u, height=%u\n", viewport.X (), viewport.Y (), viewport.Width (), viewport.Height ());
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
    
///Проверяющая функция
    static bool Check (RenderManager&, const char*)
    {
      return true;
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
    SceneRender render;    
    
    {
      Test test (L"Window1 test");
      
      SceneRenderManager::RegisterRender ("MySceneRender", &MySceneRender::Check, &MySceneRender::Create);    
      
      RenderManager manager = test.RenderManager ();
      
      render.SetRenderManager (&manager);
      
      render.SetRenderTarget ("default_color", test.Window ().ColorBuffer ());
      render.SetRenderTarget ("default_depth_stencil", test.Window ().DepthStencilBuffer ());

      scene_graph::Screen screen;        
      
      render.SetScreen (&screen);
      
      screen.SetBackgroundState (false);    
      
      scene_graph::OrthoCamera::Pointer camera1 = scene_graph::OrthoCamera::Create (), camera2 = scene_graph::OrthoCamera::Create ();
      
        //настройка области вывода

      camera1->SetName ("Camera1");
      camera2->SetName ("Camera2");
      
      scene_graph::Viewport vp1;

      vp1.SetArea      (0, 0, 100, 100);
      vp1.SetCamera    (&*camera1);
      vp1.SetTechnique ("Technique1");
      vp1.SetZOrder    (2);
      
      screen.Attach (vp1);

      scene_graph::Viewport vp2;

      vp2.SetArea      (10, 10, 50, 50);
      vp2.SetCamera    (&*camera2);
      vp2.SetTechnique ("Technique2");
      vp2.SetZOrder    (1);
      
      screen.Attach (vp2);
      
      render.Update ();
      
      printf ("Changing screen...\n");
      
      render.SetScreen (0);
      
      render.Update ();
      
      render.SetScreen (&screen);
      
      render.Update ();
      
      printf ("Destroy render manager...\n");
    }
    
    printf ("Destroy scene render...\n");

    return 0;
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
