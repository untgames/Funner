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

RenderTargetManager::RenderTargetManager ()
  : render_path_manager (0),
    draw_depth (0)
{
}

/*
    Регистрация целей рендеринга
*/

void RenderTargetManager::Register (RenderTargetBase& target)
{
  render_targets.insert (&target);
}

void RenderTargetManager::Unregister (RenderTargetBase& target)
{
  render_targets.erase (&target);
}

/*
    Сброс ресурсов
*/

void RenderTargetManager::FlushResources ()
{
  for (RenderTargetSet::iterator iter=render_targets.begin (), end=render_targets.end (); iter!=end; ++iter)
    (*iter)->FlushResources ();
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
      renderer->DrawFrames ();
  }
}

/*
===================================================================================================
    RenderTargetBase
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

RenderTargetBase::RenderTargetBase (RenderTargetManager& in_manager)
  : manager (&in_manager)
{
  manager->Register (*this);
}

RenderTargetBase::~RenderTargetBase ()
{
  manager->Unregister (*this);
}
