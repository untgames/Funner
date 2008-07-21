#include "scene_render_shared.h"

using namespace render;

/*
    Описание реализации SceneRender
*/

typedef xtl::intrusive_ptr<RenderTargetManager> RenderTargetManagerPtr;

struct SceneRender::Impl
{  
  RenderPathManager        render_path_manager;   //менеджер путей рендеринга
  RenderTargetManagerPtr   render_target_manager; //менеджер целей рендеринга
  SceneRender::LogFunction log_handler;           //функция отладочного протоколирования

    //конструктор
  Impl () : render_target_manager (new RenderTargetManager, false)
  {
  }
  
  ~Impl ()
  {
    render_target_manager->SetRenderPathManager (0);
  }

    //отладочное протоколирование
  void LogMessage (const char* message)
  {
    if (!log_handler)
      return;

    try
    {
      log_handler (message);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
  
  void LogPrintf (const char* format, ...)
  {
    if (!log_handler)
      return;
    
    va_list args;
    
    va_start (args, format);
    
    try
    {
      log_handler (common::vformat (format, args).c_str ());
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
    Конструктор / деструктор
*/

SceneRender::SceneRender ()
  : impl (new Impl)
{
}

SceneRender::SceneRender (const char* driver_name_mask, const char* renderer_name_mask, const char* render_path_masks)
  : impl (new Impl)
{
  try
  {
    SetRenderer (driver_name_mask, renderer_name_mask, render_path_masks);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::SceneRender");
    throw;
  }
}

SceneRender::~SceneRender ()
{
}

/*
    Установка/получение системы рендеринга
*/

void SceneRender::SetRenderer
 (const char* driver_name_mask,   //маска имени драйвера системы рендеринга
  const char* renderer_name_mask, //маска имени системы рендеринга в драйвере
  const char* render_path_masks)  //список масок имён требуемых путей рендеринга (разделитель - пробел)
{
  try
  {
    RenderPathManager new_manager (driver_name_mask, renderer_name_mask, render_path_masks,
      xtl::bind (&Impl::LogMessage, &*impl, _1));      

    new_manager.Swap (impl->render_path_manager);

    impl->render_target_manager->SetRenderPathManager (&impl->render_path_manager);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::SetRenderer");
    throw;
  }
}

void SceneRender::ResetRenderer ()
{
  RenderPathManager new_manager;

  new_manager.Swap (impl->render_path_manager);

  impl->render_target_manager->SetRenderPathManager (0);
}

const char* SceneRender::RendererDescription () const
{
  mid_level::IRenderer* renderer = impl->render_path_manager.Renderer ();
  
  return renderer ? renderer->GetDescription () : "";
}

const char* SceneRender::RenderPaths () const
{
  return impl->render_path_manager.RenderPaths ();
}

bool SceneRender::HasRenderPath (const char* path_name) const
{
  return impl->render_path_manager.HasRenderPath (path_name);
}

/*
    Работа с ресурсами
*/

void SceneRender::LoadResource (const char* tag, const char* file_name)
{
  impl->render_path_manager.LoadResource (tag, file_name, impl->log_handler);
}

/*
    Создание цели рендеринга
*/

RenderTarget SceneRender::CreateRenderTarget (const char* color_attachment_name, const char* depth_stencil_attachment_name)
{
  try
  {
    return RenderTarget (*impl->render_target_manager, color_attachment_name, depth_stencil_attachment_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRender::CreateRenderTarget");
    throw;
  }
}

/*
    Установка функции отладочного протоколирования
*/

void SceneRender::SetLogHandler (const LogFunction& log)
{
  impl->log_handler = log;
}

const SceneRender::LogFunction& SceneRender::LogHandler () const
{
  return impl->log_handler;
}

/*
    Отладочное протоколирование
*/

namespace render
{

void log_printf (const SceneRender::LogFunction& log_handler, const char* format, ...)
{
  if (!log_handler || !format)
    return;

  va_list args;

  va_start (args, format);

  try
  {
    log_handler (common::vformat (format, args).c_str ());
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

}
