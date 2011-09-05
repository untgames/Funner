#include "shared.h"

using namespace render;

namespace
{

const char* RENDERER_COMPONENTS_MASK = "render.scene.*"; //маска имён компонентов путей рендеринга

}

/*
    Описание реализации менеджера путей рендеринга
*/

typedef stl::hash_map<stl::hash_key<const char*>, SceneRenderManagerImpl::RenderCreator> CreatorMap;

struct SceneRenderManagerImpl::Impl
{
  CreatorMap creators;
};

/*
    Конструкторы / деструктор
*/

SceneRenderManagerImpl::SceneRenderManagerImpl ()
  : impl (new Impl)
{
}

SceneRenderManagerImpl::~SceneRenderManagerImpl ()
{
}

/*
    Регистрация рендеров
*/

void SceneRenderManagerImpl::RegisterRender (const char* renderer, const RenderCreator& creator)
{
  static const char* METHOD_NAME = "render::SceneRenderManagerImpl::RegisterRender";

  if (!renderer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "renderer");
    
  CreatorMap::iterator iter = impl->creators.find (renderer);
  
  if (iter != impl->creators.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "renderer", renderer, "Renderer has already registered");
    
  impl->creators.insert_pair (renderer, creator);
}

void SceneRenderManagerImpl::UnregisterRender (const char* renderer)
{
  if (!renderer)
    return;
    
  impl->creators.erase (renderer);
}

void SceneRenderManagerImpl::UnregisterAllRenders ()
{
  impl->creators.clear ();
}

/*
    Создание рендера
*/

ISceneRenderPtr SceneRenderManagerImpl::CreateRender (RenderManager& manager, const char* renderer)
{
  try
  {
    if (!renderer)
      throw xtl::make_null_argument_exception ("", "renderer");
      
    static common::ComponentLoader loader (RENDERER_COMPONENTS_MASK);
      
    CreatorMap::iterator iter = impl->creators.find (renderer);
    
    if (iter == impl->creators.end ())
      throw xtl::make_argument_exception ("", "renderer", renderer, "Renderer has not registered");
      
    return ISceneRenderPtr (iter->second (manager, renderer), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::SceneRenderManagerImpl::CreateRender");
    throw;
  }
}

/*
    SceneRenderManager
*/

void SceneRenderManager::RegisterRender (const char* renderer, const RenderCreator& creator)
{
  SceneRenderManagerSingleton::Instance ()->RegisterRender (renderer, creator);
}

void SceneRenderManager::UnregisterRender (const char* renderer)
{
  SceneRenderManagerSingleton::Instance ()->UnregisterRender (renderer);
}

void SceneRenderManager::UnregisterAllRenders ()
{
  SceneRenderManagerSingleton::Instance ()->UnregisterAllRenders ();
}
