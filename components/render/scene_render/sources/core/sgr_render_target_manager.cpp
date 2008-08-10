#include "scene_render_shared.h"

using namespace render;

/*
===================================================================================================
    RenderTargetManager
===================================================================================================
*/

/*
    Конструктор
*/

RenderTargetManager::RenderTargetManager (const SceneRender::LogFunction& in_log_handler)
  : render_path_manager (0),
    draw_depth (0),
    log_handler (in_log_handler)
{
}

/*
    Регистрация целей рендеринга
*/

void RenderTargetManager::Register (IRenderTargetImpl* target)
{
  render_targets.push_back (target);
}

void RenderTargetManager::Unregister (IRenderTargetImpl* target)
{
  render_targets.erase (stl::remove (render_targets.begin (), render_targets.end (), target), render_targets.end ());
}

/*
    Перебор доступных целей рендеринга
*/

IRenderTargetImpl* RenderTargetManager::RenderTarget (size_t index)
{
  if (index >= render_targets.size ())
    throw xtl::make_range_exception ("render::RenderTargetManager::RenderTarget", "index", index, render_targets.size ());

  return render_targets [index];
}

/*
    Сброс ресурсов
*/

void RenderTargetManager::FlushResources ()
{
  for (RenderTargetList::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
    (*iter)->FlushResources ();

  UnregisterAllAttachments ();
}

/*
    Установка текущего менеджера путей рендеринга
*/

void RenderTargetManager::SetRenderPathManager (render::RenderPathManager* in_render_path_manager)
{
  FlushResources ();

  render_path_manager = in_render_path_manager;
}

/*
    Получение системы рендеринга
*/

mid_level::IRenderer& RenderTargetManager::Renderer ()
{
  static const char* METHOD_NAME = "render::RenderPathManager::Renderer";

  if (!render_path_manager)
    throw xtl::format_operation_exception (METHOD_NAME, "Null render path manager");

  render::mid_level::IRenderer* renderer = render_path_manager->Renderer ();

  if (!renderer)
    throw xtl::format_operation_exception (METHOD_NAME, "Null renderer");

  return *renderer;
}

/*
    Получение пути рендеринга
*/

ICustomSceneRender& RenderTargetManager::GetRenderPath (const char* render_path_name)
{
  try
  {
    if (!render_path_manager)
      throw xtl::format_operation_exception ("", "Null render path manager");

    return render_path_manager->GetRenderPath (render_path_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetManager::GetRenderPath");
    throw;
  }
}

/*
    Регистрация ассоциированных целей ренедринга
*/

void RenderTargetManager::RegisterAttachment (const char* name, mid_level::IRenderTarget* render_target)
{
  static const char* METHOD_NAME = "render::RenderTargetManager::RegisterAttachment";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  AttachmentMap::iterator iter = attachments.find (name);
  
  if (iter != attachments.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Attachment already registered");

  attachments.insert_pair (name, render_target);
}

void RenderTargetManager::UnregisterAttachment (const char* name)
{
  if (!name)
    return;
    
  attachments.erase (name);
}

void RenderTargetManager::UnregisterAllAttachments ()
{
  attachments.clear ();
}

mid_level::IRenderTarget* RenderTargetManager::GetAttachment (const char* name)
{
  static const char* METHOD_NAME = "render::RenderTargetManager::GetAttachment";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  AttachmentMap::iterator iter = attachments.find (name);

  if (iter == attachments.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Attachment not found");

  return iter->second.get ();
}

/*
    Начало / конец транзакции отрисовки 
*/

//результат показывает допустимость отрисовки (введено для будущего контроля глубины вложенности рендеринга)
bool RenderTargetManager::BeginDraw ()
{
  if (draw_depth == ~0)
    return false;
    
  draw_depth++;
  
  return true;
}

void RenderTargetManager::EndDraw ()
{
  if (!draw_depth)
    return;
    
  if (!--draw_depth && render_path_manager)
  {
      //визуализация кадра
          
    mid_level::IRenderer* renderer = render_path_manager->Renderer ();
    
    if (renderer)
    {
      try
      {
        renderer->DrawFrames ();
      }
      catch (std::exception& exception)
      {
        LogMessage (exception.what ());
      }
      catch (...)
      {
        LogMessage ("Unknown exception\n    at render::mid_level::IRenderer::DrawFrames");
      }
    }
  }
}

/*
    Протоколирование
*/

void RenderTargetManager::LogMessage (const char* message)
{
  if (!message || !log_handler)
    return;
    
  log_handler (message);
}
