#include "shared.h"

///Область вывода рендера
class MyRenderView: public IRenderView, public xtl::reference_counter
{
  public:
    MyRenderView ()
    {
      static size_t global_id = 1;
      
      id = global_id++;
      
      printf ("MyRenderView #%u: Create view\n", id);
    }
    
    ~MyRenderView ()
    {
      printf ("MyRenderView #%u: Destroy view\n", id);
    }
  
///Установка области вывода
    void SetViewport (const Rect& rect)
    {
      printf ("MyRenderView #%u: SetViewport(%d, %d, %u, %u)\n", id, rect.left, rect.top, rect.width, rect.height);
    }
    
    void GetViewport (Rect&)
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetViewport");
    }
    
///Установка камеры
    void SetCamera (scene_graph::Camera* camera)
    {
      printf ("MyRenderView #%u: SetCamera(%s)\n", id, camera ? camera->Name () : "null");
    }

    scene_graph::Camera* GetCamera ()
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetCamera");
    }

///Установка / чтение свойств
    void SetProperty (const char* name, const char* value)
    {
      printf ("MyRenderView #%u: SetProperty(%s, %s)\n", id, name, value);
    }

    void GetProperty (const char*, size_t, char*)
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetProperty");      
    }

///Рисование
    void Draw ()
    {
      printf ("MyRenderView #%u: Draw view\n", id);
    }

///Подсчёт ссылок
    void AddRef ()    
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:    
    size_t id;
};

///Рендер сцены
class MySceneRender: public ICustomSceneRender, public xtl::reference_counter
{
  public:
    MySceneRender ()
    {
      printf ("MySceneRender::MySceneRender\n");
    }
    
    ~MySceneRender ()
    {
      printf ("MySceneRender::~MySceneRender\n");
    }    
  
///Создание областей вывода
    IRenderView* CreateRenderView (scene_graph::Scene* scene)
    {
      printf ("CreateRenderView('%s')\n", scene ? scene->Name () : "null");
      
      return new MyRenderView;
    }

///Работа с ресурсами
    void LoadResource (const char* tag, const char* file_name)
    {
      printf ("LoadResource(%s, %s)\n", tag, file_name);
    }

///Установка функции отладочного протоколирования
    void SetLogHandler (const ICustomSceneRender::LogFunction&) {}

    const ICustomSceneRender::LogFunction& GetLogHandler ()
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetLogHandler");
    }

///Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }    
    
    void Release ()
    {
      release (this);
    }
    
///Создание рендера
    static ICustomSceneRender* Create (mid_level::IRenderer* renderer, const char* path_name)
    {
      printf ("MySceneRender::Create(%s, %s)\n", renderer ? renderer->GetDescription () : "null", path_name);
      
      return new MySceneRender;
    }
};

void log_print (const char* message)
{
  printf ("%s\n", message);
}

int main ()
{
  printf ("Results of custom_render_test:\n");
  
  try
  {
      //регистрация пользовательского рендера
    
    SceneRenderManager::RegisterRender ("MySceneRender", &MySceneRender::Create);

      //создание рендера сцены

    SceneRender render ("*", "*", "MySceneRender");

      //создание сцены

    OrthoCamera::Pointer camera1 = OrthoCamera::Create (), camera2 = OrthoCamera::Create ();
    
      //настройка области вывода

    camera1->SetName ("Camera1");
    camera2->SetName ("Camera2");

    render.SetLogHandler (&log_print);
    
    Viewport vp1;
    Desktop desktop;

    vp1.SetArea       (0, 0, 100, 100);
    vp1.SetCamera     (&*camera1);
    vp1.SetRenderPath ("MySceneRender");
    
    desktop.Attach (vp1);
    
    RenderTarget render_target = render.CreateRenderTarget ("default", "default");
    
    render_target.SetDesktop (&desktop);
    
      //загрузка ресурсов

    render.LoadResource ("data/materials.xmtl");

      //отрисовка

    render_target.Update ();
    
      //присоединение камеры к сцене
      
    printf ("Bind camera to scene\n");
      
    Scene scene1, scene2;

    scene1.SetName ("Scene1");
    scene2.SetName ("Scene2");

    camera1->BindToScene (scene1);
    camera2->BindToScene (scene2);

    render_target.Update ();
    
      //смена сцены
      
    printf ("Change camera scene\n");

    camera1->BindToScene (scene2);

    render_target.Update ();
    
      //изменение камеры
      
    printf ("Change viewport camera\n");
    
    vp1.SetCamera (&*camera2);

    camera1->BindToScene (scene1);

    render_target.Update ();    
    
      //отсоединение камеры от сцены
      
    printf ("Unbind camera from scene\n");
    
    camera2->Unbind ();
    
    render_target.Update ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
