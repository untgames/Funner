#include "shared.h"

using namespace render;

namespace
{

/*
    Константы
*/

const char* RENDERER_COMPONENTS_MASK = "render.scene.*"; //маска имён компонентов путей рендеринга

/*
    Описание рендера
*/

struct RenderDesc
{
  SceneRenderManagerImpl::RenderChecker checker; //функция, проверящая возможность создания рендера с указанной техникой
  SceneRenderManagerImpl::RenderCreator creator; //порождающая функция рендера
  
///Конструктор
  RenderDesc (const SceneRenderManagerImpl::RenderChecker& in_checker, const SceneRenderManagerImpl::RenderCreator& in_creator)
    : checker (in_checker)
    , creator (in_creator)
  {
  }
};

}

/*
    Описание реализации менеджера путей рендеринга
*/

typedef stl::hash_map<stl::hash_key<const char*>, RenderDesc> RenderDescMap;
typedef stl::list<RenderDescMap::iterator>                    RenderDescList;

struct SceneRenderManagerImpl::Impl
{
  RenderDescMap  desc_map;  //описатели доступных рендеров
  RenderDescList desc_list; //список доступных рендеров в порядке регистрации
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

void SceneRenderManagerImpl::RegisterRender (const char* renderer, const RenderChecker& checker, const RenderCreator& creator)
{
  static const char* METHOD_NAME = "render::SceneRenderManagerImpl::RegisterRender";

  if (!renderer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "renderer");
    
  RenderDescMap::iterator iter = impl->desc_map.find (renderer);
  
  if (iter != impl->desc_map.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "renderer", renderer, "Renderer has already registered");
    
  stl::pair<RenderDescMap::iterator, bool> result = impl->desc_map.insert_pair (renderer, RenderDesc (checker, creator));
  
  try
  {
    impl->desc_list.push_back (result.first);
  }
  catch (...)
  {
    impl->desc_map.erase (result.first);
    throw;
  }
}

void SceneRenderManagerImpl::UnregisterRender (const char* renderer)
{
  if (!renderer)
    return;
    
  RenderDescMap::iterator iter = impl->desc_map.find (renderer);
  
  if (iter == impl->desc_map.end ())
    return;
    
  impl->desc_map.erase (iter);
  impl->desc_list.remove (iter);
}

void SceneRenderManagerImpl::UnregisterAllRenders ()
{
  impl->desc_map.clear ();
  impl->desc_list.clear ();
}

/*
    Создание рендера
*/

ISceneRenderPtr SceneRenderManagerImpl::CreateRender (RenderManager& manager, const char* technique)
{
  try
  {
    if (!technique)
      throw xtl::make_null_argument_exception ("", "technique");
      
    static common::ComponentLoader loader (RENDERER_COMPONENTS_MASK);
    
    for (RenderDescList::reverse_iterator iter=impl->desc_list.rbegin (), end=impl->desc_list.rend (); iter!=end; ++iter)
    {
      RenderDesc& desc = (*iter)->second;
      
      try
      {
        if (!desc.checker (manager, technique))
          continue;
      }
      catch (...)
      {
        continue;
      }
      
      return ISceneRenderPtr (desc.creator (manager, technique), false);
    }
    
    throw xtl::format_operation_exception ("", "Can't create scene render for technique '%s'", technique);
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

void SceneRenderManager::RegisterRender (const char* renderer, const RenderChecker& checker, const RenderCreator& creator)
{
  SceneRenderManagerSingleton::Instance ()->RegisterRender (renderer, checker, creator);
}

void SceneRenderManager::UnregisterRender (const char* renderer)
{
  SceneRenderManagerSingleton::Instance ()->UnregisterRender (renderer);
}

void SceneRenderManager::UnregisterAllRenders ()
{
  SceneRenderManagerSingleton::Instance ()->UnregisterAllRenders ();
}
