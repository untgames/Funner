#include "scene_render_shared.h"

using namespace render;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_MAX_DRAW_DEPTH = 16; //максимальный уровень вложенности рендеринга по умолчанию

}

/*
    Конструктор
*/

RenderTargetManager::RenderTargetManager (const SceneRender::LogFunction& in_log_handler)
  : render_path_manager (0),
    draw_context (0),
    max_draw_depth (DEFAULT_MAX_DRAW_DEPTH),
    log_handler (in_log_handler),
    anonymous_attachment_counter (0)
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

RenderTarget RenderTargetManager::RenderTarget (size_t index)
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
  
  anonymous_attachment_counter = 0;
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
    Создание цели рендеринга
*/

RenderTarget RenderTargetManager::CreateRenderTarget
 (const char* color_attachment_name,
  const char* depth_stencil_attachment_name)
{
  try
  {
    return render::RenderTarget (*this, color_attachment_name, depth_stencil_attachment_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetManager::CreateRenderTarget(const char*, const char*)");
    throw;
  }
}

RenderTarget RenderTargetManager::CreateRenderTarget
 (const char*               prefix,
  mid_level::IRenderTarget* color_attachment,
  mid_level::IRenderTarget* depth_stencil_attachment)
{
  try
  {
    if (!prefix)
      throw xtl::make_null_argument_exception ("", "prefix");

    stl::string color_attachment_name         = common::format ("%s.Color", prefix),
                depth_stencil_attachment_name = common::format ("%s.DepthStencil", prefix);

    RegisterAttachment (color_attachment_name.c_str (), color_attachment);
    
    try
    {
      RegisterAttachment (depth_stencil_attachment_name.c_str (), depth_stencil_attachment);
      
      try
      {
        return CreateRenderTarget (color_attachment_name.c_str (), depth_stencil_attachment_name.c_str ());
      }
      catch (...)
      {
        UnregisterAttachment (depth_stencil_attachment_name.c_str ());
        throw;
      }
    }
    catch (...)
    {
      UnregisterAttachment (color_attachment_name.c_str ());
      throw;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetManager::CreateRenderTarget(const char*, mid_level::IRenderTarget*, mid_level::IRenderTarget*)");
    throw;
  }
}

RenderTarget RenderTargetManager::CreateRenderTarget
 (mid_level::IRenderTarget* color_attachment,
  mid_level::IRenderTarget* depth_stencil_attachment)
{
  try
  {
    size_t id = anonymous_attachment_counter++;
    
    try
    {
      if (!anonymous_attachment_counter)
        throw xtl::format_operation_exception ("", "Too many anonymous attachments");

      return CreateRenderTarget (common::format ("RenderTarget%u", id).c_str (), color_attachment, depth_stencil_attachment);
    }
    catch (...)
    {
      --anonymous_attachment_counter;
      throw;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderTargetManager::CreateRenderTarget(mid_level::IRenderTarget*, mid_level::IRenderTarget*)");
    throw;
  }
}

/*
    Текущий контекст рендеринга
*/

void RenderTargetManager::SetDrawContext (render::DrawContext* in_draw_context)
{
  draw_context = in_draw_context;
}

/*
    Максимальный уровень вложенности рендеринга
*/

void RenderTargetManager::SetMaxDrawDepth (size_t level)
{
  max_draw_depth = level;
}

/*
    Рисование кадров
*/

void RenderTargetManager::DrawFrames ()
{
  if (!render_path_manager)
    return;
        
  mid_level::IRenderer* renderer = render_path_manager->Renderer ();
  
  if (!renderer)
    return;
    
    //визуализация кадров    
    
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

/*
    Протоколирование
*/

void RenderTargetManager::LogMessage (const char* message)
{
  if (!message || !log_handler)
    return;
    
  log_handler (message);
}
