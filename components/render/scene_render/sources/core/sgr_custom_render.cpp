#include "shared.h"

using namespace render;

/*
    Константы
*/

namespace
{

const char* RENDER_PATH_COMPONENTS_MASK = "render.scene_render.*"; //маска имён компонентов путей рендеринга

}

/*
===================================================================================================
    Реализация менеджера путей рендеринга
===================================================================================================
*/

class SceneRenderManagerImpl
{
  public:
    typedef SceneRenderManager::RenderCreater RenderCreater;
    typedef SceneRenderManager::Iterator      Iterator;
  
///Регистрация путей рендеринга
    void RegisterRender (const char* path_name, const RenderCreater& creater)
    {
      static const char* METHOD_NAME = "render::SceneRenderManagerImpl::RegisterRender";

      if (!path_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "path_name");
        
      RenderPathMap::iterator iter = paths.find (path_name);
      
      if (iter != paths.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "path_name", path_name, "Render path has already registered");
        
      paths.insert_pair (path_name, RenderPathPtr (new RenderPath (path_name, creater), false));      
    }

///Отмена регистрации пути рендеринга
    void UnregisterRender (const char* path_name)
    {
      if (!path_name)
        return;
        
      paths.erase (path_name);
    }

///Отмена регистрации всех путей рендеринга
    void UnregisterAllRenders ()
    {
      paths.clear ();
    }

///Перебор путей рендеринга
    Iterator CreateIterator ()
    {
      LoadDefaultComponents ();

      return Iterator (paths.begin (), paths.begin (), paths.end (), RenderPathSelector ());        
    }

///Создание рендера
    CustomSceneRenderPtr CreateRender (mid_level::IRenderer* renderer, const char* path_name)
    {
      try
      {
        if (!renderer)
          throw xtl::make_null_argument_exception ("", "renderer");
          
        if (!path_name)
          throw xtl::make_null_argument_exception ("", "path_name");
          
        LoadDefaultComponents ();

        RenderPathMap::iterator iter = paths.find (path_name);

        if (iter == paths.end ())
          throw xtl::make_argument_exception ("", "path_name", path_name, "Render path has not registered");
          
        try
        {          
          return CustomSceneRenderPtr (iter->second->CreateRender (renderer), false);
        }
        catch (xtl::exception& exception)
        {
          exception.touch ("create render path '%s'", path_name);
          throw;
        }
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::SceneRenderManagerImpl::CreateRender");
        throw;
      }
    }
        
  private:
///Загрузка компонентов по умолчанию
    void LoadDefaultComponents ()
    {
      static common::ComponentLoader loader (RENDER_PATH_COMPONENTS_MASK);
    }

  private:
///Реализация пути рендеринга
    struct RenderPath: public SceneRenderManager::IRenderPath, public xtl::reference_counter
    {
      RenderPath (const char* in_name, const RenderCreater& in_creater) : name (in_name), creater (in_creater) { }
      
///Получение имени пути
      const char* Name () { return name.c_str (); }
      
///Создание пути рендеринга
      ICustomSceneRender* CreateRender (mid_level::IRenderer* renderer)
      {
        return creater (renderer, name.c_str ());
      }

      stl::string   name;    //имя пути рендеринга
      RenderCreater creater; //функтор создания рендера
    };

    typedef xtl::intrusive_ptr<RenderPath>                           RenderPathPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, RenderPathPtr> RenderPathMap;
    
///Селектор интерфейса пути рендеринга
    struct RenderPathSelector
    {
      SceneRenderManager::IRenderPath& operator () (RenderPathMap::value_type& v) const { return *v.second; }
    };    

  private:
    RenderPathMap paths; //карта путей рендеринга
};

//синглтон для менеджера путей рендеринга
typedef common::Singleton<SceneRenderManagerImpl> SceneRenderManagerSingleton;

/*
===================================================================================================
    SceneRenderManager
===================================================================================================
*/

/*
    Регистрация путей рендеринга
*/

void SceneRenderManager::RegisterRender (const char* path_name, const RenderCreater& creater)
{
  try
  {
    SceneRenderManagerSingleton::Instance ().RegisterRender (path_name, creater);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRenderManager::RegisterRender");
    throw;
  }
}

void SceneRenderManager::UnregisterRender (const char* path_name)
{
  SceneRenderManagerSingleton::Instance ().UnregisterRender (path_name);
}

void SceneRenderManager::UnregisterAllRenders ()
{
  SceneRenderManagerSingleton::Instance ().UnregisterAllRenders ();
}

/*
    Перебор путей рендеринга
*/

SceneRenderManager::Iterator SceneRenderManager::CreateIterator ()
{
  try
  {
    return SceneRenderManagerSingleton::Instance ().CreateIterator ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::SceneRenderManager::CreateIterator");
    throw;
  }
}

/*
    Создание пути рендеринга
*/

namespace render
{

CustomSceneRenderPtr create_render_path (mid_level::IRenderer* renderer, const char* path_name)
{
  return SceneRenderManagerSingleton::Instance ().CreateRender (renderer, path_name);
}

}
