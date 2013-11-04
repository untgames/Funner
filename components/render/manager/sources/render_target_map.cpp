#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

/*
    Вспомогательные структуры
*/

typedef stl::hash_map<stl::hash_key<const char*>, RenderTargetDescPtr> RenderTargetDescMap;

/*
    Описание реализации кадра
*/

struct RenderTargetMapImpl::Impl
{
  RenderTargetDescMap render_targets; //целевые буферы отрисовки
};

/*
   Конструктор / деструктор
*/

RenderTargetMapImpl::RenderTargetMapImpl ()
  : impl (new Impl)
{
}

RenderTargetMapImpl::~RenderTargetMapImpl ()
{
}

/*
    Регистрация целевых буферов отрисовки
*/

void RenderTargetMapImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target)
{
  SetRenderTarget (name, target, ViewportPtr (), RectAreaPtr ());
}

void RenderTargetMapImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const ViewportPtr& viewport)
{
  SetRenderTarget (name, target, viewport, RectAreaPtr ());
}

void RenderTargetMapImpl::SetRenderTarget (const char* name, const RenderTargetPtr& target, const ViewportPtr& viewport, const RectAreaPtr& scissor)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!target)
      throw xtl::make_null_argument_exception ("", "target");

    RenderTargetDescMap::iterator iter = impl->render_targets.find (name);

    if (iter != impl->render_targets.end ())
      throw xtl::make_argument_exception ("", "name", name, "Render target has already registered");

    impl->render_targets.insert_pair (name, RenderTargetDescPtr (new render::manager::RenderTargetDesc (target, viewport, scissor), false));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::RenderTargetMapImpl::SetRenderTarget(const char*,const RenderTargetPtr&,const RectAreaPtr&,const RectAreaPtr&)");
    throw;
  }
}

void RenderTargetMapImpl::RemoveRenderTarget (const char* name)
{
  if (!name)
    return;
    
  impl->render_targets.erase (name);
}

void RenderTargetMapImpl::RemoveAllRenderTargets ()
{
  impl->render_targets.clear ();
}

/*
    Получение целевых буферов отрисовки
*/

RenderTargetPtr RenderTargetMapImpl::FindRenderTarget (const char* name)
{
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);
  
  if (!desc)
    return RenderTargetPtr ();
  
  return desc->render_target;
}

ViewportPtr RenderTargetMapImpl::FindViewport (const char* name)
{
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);
  
  if (!desc)
    return ViewportPtr ();
    
  return desc->viewport;
}

RectAreaPtr RenderTargetMapImpl::FindScissor (const char* name)
{
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);

  if (!desc)
    return RectAreaPtr ();

  return desc->scissor;
}

RenderTargetDescPtr RenderTargetMapImpl::FindRenderTargetDesc (const char* name)
{
  if (!name)
    return RenderTargetDescPtr ();

  RenderTargetDescMap::iterator iter = impl->render_targets.find (name);

  if (iter == impl->render_targets.end ())
    return RenderTargetDescPtr ();
    
  return iter->second;
}

RenderTargetPtr RenderTargetMapImpl::RenderTarget (const char* name)
{
  static const char* METHOD_NAME = "render::manager::RenderTargetMapImpl::RenderTarget";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RenderTargetPtr target = FindRenderTarget (name);
  
  if (target)
    return target;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Render target not found");
}

ViewportPtr RenderTargetMapImpl::Viewport (const char* name)
{
  static const char* METHOD_NAME = "render::manager::RenderTargetMapImpl::Viewport";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  ViewportPtr viewport = FindViewport (name);
  
  if (viewport)
    return viewport;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Viewport not found");
}

RectAreaPtr RenderTargetMapImpl::Scissor (const char* name)
{
  static const char* METHOD_NAME = "render::manager::RenderTargetMapImpl::Scissor";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RectAreaPtr area = FindScissor (name);
  
  if (area)
    return area;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Scissor not found");
}

RenderTargetDescPtr RenderTargetMapImpl::RenderTargetDesc (const char* name)
{
  static const char* METHOD_NAME = "render::manager::RenderTargetMapImpl::RenderTargetDesc";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  RenderTargetDescPtr desc = FindRenderTargetDesc (name);
  
  if (desc)
    return desc;
    
  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Render target not found");
}
