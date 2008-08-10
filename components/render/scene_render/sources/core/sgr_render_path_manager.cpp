#include "scene_render_shared.h"

using namespace render;

namespace
{

/*
    Константы
*/

const size_t RENDER_PATHS_STRING_RESERVE = 256; //резервируемый размер строки с имёна путей рендеринга

}

/*
    Конструкторы / деструктор
*/

RenderPathManager::RenderPathManager
 (const char*          driver_name_mask,
  const char*          renderer_name_mask,
  const char*          render_path_masks,
  const LogFunction&   log_handler,
  const QueryFunction& query_handler)
{
  try  
  {
    if (!driver_name_mask)
      throw xtl::make_null_argument_exception ("", "driver_name_mask");
      
    if (!renderer_name_mask)
      throw xtl::make_null_argument_exception ("", "renderer_name_mask");
      
    if (!render_path_masks)
      throw xtl::make_null_argument_exception ("", "render_path_masks");      

      //создание системы ренедринга      

    renderer = RendererPtr (mid_level::DriverManager::CreateRenderer (driver_name_mask, renderer_name_mask), false);

      //построение множества имён путей ренедринга
      
    typedef stl::vector<stl::string>   StringArray;
    typedef stl::hash_set<stl::string> StringSet;
      
    StringSet                    path_names;
    StringArray                  path_masks        = common::split (render_path_masks);
    SceneRenderManager::Iterator render_path_begin = SceneRenderManager::CreateIterator ();

    for (StringArray::iterator iter=path_masks.begin (), end=path_masks.end (); iter!=end; ++iter)
    {
      const char* render_path_mask = iter->c_str ();      
      
      if (!strchr (render_path_mask, '*') && !strchr (render_path_mask, '?'))
      {
        path_names.insert (render_path_mask);
      }
      else
      {
        for (SceneRenderManager::Iterator render_path_iter=render_path_begin; render_path_iter; ++render_path_iter)
        {
          if (common::wcmatch (render_path_iter->Name (), render_path_mask))
            path_names.insert (render_path_iter->Name ());
        }
      }
    }

      //создание путей рендеринга

    render_paths_string.reserve (RENDER_PATHS_STRING_RESERVE);
    
    for (StringSet::iterator iter=path_names.begin (), end=path_names.end (); iter!=end; ++iter)
    {
      const char* path_name = iter->c_str ();

        //создание пути рендеринга
        
      CustomSceneRenderPtr render_path = SceneRenderManagerImpl::Instance ().CreateRender (renderer.get (), path_name);
      
        //установка функций обратного вызова

      render_path->SetLogHandler   (log_handler);
      render_path->SetQueryHandler (query_handler);

        //регистрация пути рендеринга
        
      render_paths.insert_pair (path_name, RenderPath (render_path, path_name));

        //модификация строки имён путей рендеринга

      if (!render_paths_string.empty ())
        render_paths_string += ' ';

      render_paths_string += path_name;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::RenderPathManager::RenderPathManager");
    throw;
  }
}

/*
    Проверка наличия пути рендеринга / получение пути рендеринга
*/

bool RenderPathManager::HasRenderPath (const char* path_name) const
{
  if (!path_name)
    return false;

  RenderPathMap::const_iterator iter = render_paths.find (path_name);

  return iter != render_paths.end ();
}

ICustomSceneRender& RenderPathManager::GetRenderPath (const char* path_name) const
{
  static const char* METHOD_NAME = "render::RenderPathManager::GetRenderPath";

  if (!path_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "path_name");

  RenderPathMap::const_iterator iter = render_paths.find (path_name);

  if (iter == render_paths.end ())
    throw xtl::format_not_supported_exception (METHOD_NAME, "Unknown path '%s'", path_name); //путь не найден

  return *iter->second.render;
}

/*
    Загрузка ресурса
*/

void RenderPathManager::LoadResource (const char* tag, const char* file_name, const LogFunction& log_handler)
{
  static const char* METHOD_NAME = "render::RenderPathManager::LoadResource";

  if (!tag)
    throw xtl::make_null_argument_exception (METHOD_NAME, "tag");

  if (!file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "file_name");

  for (RenderPathMap::iterator iter=render_paths.begin (), end=render_paths.end (); iter!=end; ++iter)
  {
    RenderPath& render_path = iter->second;

    try
    {
      render_path.render->LoadResource (tag, file_name);
    }
    catch (std::exception& exception)
    {
      log_printf (log_handler, "%s\n    at load resource (file_name='%s', tag='%s', render_path='%s', renderer='%s')"
        "\n    at %s", exception.what (), file_name, tag, render_path.name.c_str (), renderer->GetDescription (), METHOD_NAME);
    }
    catch (...)
    {
      log_printf (log_handler, "Unknown exception\n    at SceneRender::LoadResource (file_name='%s', tag='%s', render_path='%s', renderer='%s')"
        "\n    at %s", file_name, tag, render_path.name.c_str (), renderer->GetDescription (), METHOD_NAME);
    }
  }
}

/*
    Обмен
*/

void RenderPathManager::Swap (RenderPathManager& manager)
{
  renderer.swap (manager.renderer);
  render_paths.swap (manager.render_paths);
  render_paths_string.swap (manager.render_paths_string);
}
