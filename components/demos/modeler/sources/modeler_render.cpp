#include <cstdio>
#include <exception>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/iterator.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>

#include <sg/camera.h>
#include <sg/scene.h>

#include <render/low_level/device.h>

#include <render/mid_level/low_level_renderer.h>

#include <render/scene_render.h>
#include <render/custom_render.h>
#include <render/mid_level/renderer.h>

using namespace render;

namespace
{

/*
===================================================================================================
    Константы
===================================================================================================
*/

const char* COMPONENT_NAME   = "render.scene_render.ModelerRender"; //имя компонента
const char* RENDER_PATH_NAME = "ModelerRender";                     //имя пути рендеринга

/*
===================================================================================================
    Область вывода рендера
===================================================================================================
*/

class ModelerView: public IRenderView, public xtl::reference_counter
{
  public:
    ModelerView ()
    {
      printf ("Create view\n");
    }
    
    ~ModelerView ()
    {
      printf ("Destroy view\n");
    }

///Целевые буферы рендеринга
    void SetRenderTargets (mid_level::IRenderTarget* render_target, mid_level::IRenderTarget* depth_stencil_target)
    {
      printf ("SetRenderTargets(%s, %s)\n", render_target ? "render-target" : "null",
        depth_stencil_target ? "depth-stencil-target" : "null");
    }
    
    mid_level::IRenderTarget* GetRenderTarget ()
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetRenderTarget");
    }
    
    mid_level::IRenderTarget* GetDepthStencilTarget ()
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetDepthStencilTarget");
    }
  
///Установка области вывода
    void SetViewport (const Rect& rect)
    {
      printf ("SetViewport(%d, %d, %u, %u)\n", rect.left, rect.top, rect.width, rect.height);
    }
    
    void GetViewport (Rect&)
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetViewport");
    }
    
///Установка камеры
    void SetCamera (scene_graph::Camera* camera)
    {
      printf ("SetCamera(%s)\n", camera ? camera->Name () : "null");
    }

    scene_graph::Camera* GetCamera ()
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetCamera");
    }

///Установка / чтение свойств
    void SetProperty (const char* name, const char* value)
    {
      printf ("SetProperty(%s, %s)\n", name, value);
    }

    void GetProperty (const char*, size_t, char*)
    {
      throw xtl::make_not_implemented_exception ("MyRenderView::GetProperty");      
    }

///Рисование
    void Draw ()
    {
      printf ("!!!!!!!!!!!!!!Draw view\n");
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
};

/*
===================================================================================================
    Рендер сцены
===================================================================================================
*/

class ModelerRender: public ICustomSceneRender, public xtl::reference_counter
{
  public:
    ModelerRender (mid_level::IRenderer* in_renderer)
    {
      mid_level::ILowLevelRenderer* renderer = dynamic_cast<mid_level::ILowLevelRenderer*> (in_renderer);

      if (!renderer)
        throw xtl::format_exception<xtl::bad_argument> ("ModelerRender::ModelerRender", "Renderer is not castable to render::mid_level::ILowLevelRenderer");

      device = &renderer->GetDevice ();

      printf ("MySceneRender::MySceneRender\n");
    }
    
    ~ModelerRender ()
    {
      printf ("MySceneRender::~MySceneRender\n");
    }    
  
///Создание областей вывода
    IRenderView* CreateRenderView (scene_graph::Scene* scene)
    {
      printf ("CreateRenderView('%s')\n", scene ? scene->Name () : "null");
      
      return new ModelerView;
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
    
    void SetQueryHandler (const QueryFunction& in_handler)
    {
      query_handler = in_handler;
    }
    
    const QueryFunction& GetQueryHandler () { return query_handler; }

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
      
      return new ModelerRender (renderer);
    }

  private:
    low_level::IDevice* device;
    QueryFunction       query_handler;
};

/*
===================================================================================================
    Регистрация компонента
===================================================================================================
*/

class ModelerRenderComponent
{
  public:
    ModelerRenderComponent ()
    {
      SceneRenderManager::RegisterRender (RENDER_PATH_NAME, &CreateRender);
    }

  private:
    static ICustomSceneRender* CreateRender (mid_level::IRenderer* renderer, const char*)
    {
      return new ModelerRender (renderer);
    }
};

}

extern "C" 
{

common::ComponentRegistrator<ModelerRenderComponent> ModelerRender (COMPONENT_NAME);

}
