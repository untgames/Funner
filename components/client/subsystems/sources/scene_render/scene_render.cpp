#include <stl/hash_map>

#include <xtl/any.h>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/log.h>
#include <common/strlib.h>

#include <render/scene_render.h>

#include <client/engine.h>

using namespace client;
using namespace common;

namespace
{

const char* LOG_NAME = "scene_render_subsystem";

void test_registry_variable (VarRegistry& var_registry, const char* variable_name)
{
  if (!var_registry.HasVariable (variable_name))
    throw xtl::format_operation_exception ("", "There is no '%s' variable in the configuration registry branch '%s'",
      variable_name, var_registry.BranchName ());
}

stl::string get_string (VarRegistry& registry, const char* name)
{
  return registry.GetValue (name).cast<stl::string> ();
}

size_t get_size_t (VarRegistry& registry, const char* name)
{
  return atoi (get_string (registry, name).c_str ());
}

stl::string get (VarRegistry& registry, const char* name, const char* default_value)
{
  if (registry.HasVariable (name))
    return get_string (registry, name);

  return stl::string (default_value);
}

/*
   Подсистема рендера сцены               
*/

class SceneRenderSubsystem : public IEngineSubsystem, public IEngineEventListener, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    SceneRenderSubsystem (common::VarRegistry& var_registry, Engine& in_engine)
      : render (get_string (var_registry, "DriverMask").c_str (), 
                get_string (var_registry, "RendererMask").c_str (), 
                get (var_registry, "RenderPathMasks", "*").c_str ()),
        engine (&in_engine),
        render_log (""),
        subsystem_log (LOG_NAME)
    {
      if (var_registry.HasVariable ("MaxDrawDepth"))
        render.SetMaxDrawDepth (get_size_t (var_registry, "MaxDrawDepth"));

      if (var_registry.HasVariable ("Log"))
      {
        Log (get_string (var_registry, "Log").c_str ()).Swap (render_log);
        render.SetLogHandler (xtl::bind (&SceneRenderSubsystem::LogHandler, this, _1));
      }

      VarRegistry screen_registry;

      for (VarRegistry::Iterator iter = var_registry.CreateIterator (); iter; ++iter)
      {
        if (!wcmatch (iter->Name (), "Screen*"))
          continue;

        if (*var_registry.BranchName ())
          screen_registry.Open (common::format ("%s.%s", var_registry.BranchName (), iter->Name ()).c_str ());
        else
          screen_registry.Open (iter->Name ());

        if (!screen_registry.HasVariable ("Attachment"))
        {
          subsystem_log.Printf ("No variable 'Attachment' in branch '%s'", screen_registry.BranchName ());
          continue;
        }  

        if (!screen_registry.HasVariable ("RenderTarget"))
        {
          subsystem_log.Printf ("No variable 'RenderTarget' in branch '%s.%s'", screen_registry.BranchName ());
          continue;
        }  

        stl::string attachment_name = get_string (screen_registry, "Attachment");
        size_t      render_target_index = get_size_t (screen_registry, "RenderTarget");

        if (render_target_index >= render.RenderTargetsCount ())
        {
          subsystem_log.Printf ("Screen '%s' wasn't added: render target index (%u) out of range. Maximum possible index is %u.", 
                                attachment_name.c_str (), render_target_index, render.RenderTargetsCount ());
          continue;
        } 

        screen_map.insert_pair (attachment_name.c_str (), render_target_index);
      }

      engine->Attach (this);
    }

    ~SceneRenderSubsystem ()
    {
      engine->Detach (this);
    }
    
/// Получение имени
    const char* Name () { return "SceneRender"; }

/// События установки/удаления экрана
    void OnSetScreen (const char* attachment_name, render::Screen& screen) 
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (&screen);
    }

    void OnRemoveScreen (const char* attachment_name) 
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (0);
    }

/// Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:
    void LogHandler (const char* message)
    {
      render_log.Print (message);
    }

  private:
    SceneRenderSubsystem (const SceneRenderSubsystem&);             //no impl
    SceneRenderSubsystem& operator = (const SceneRenderSubsystem&); //no impl

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, size_t> ScreenMap;

  private:
    render::SceneRender render;
    ScreenMap           screen_map;
    Engine*             engine;
    Log                 render_log;
    Log                 subsystem_log;
};



/*
   Создание рендера
*/

void scene_render_startup (common::VarRegistry& var_registry, IEngineStartupParams*, Engine& engine)
{
  try
  {
    test_registry_variable (var_registry, "DriverMask");
    test_registry_variable (var_registry, "RendererMask");

    xtl::com_ptr<SceneRenderSubsystem> new_subsystem (new SceneRenderSubsystem (var_registry, engine), false);

    engine.AddSubsystem (new_subsystem.get ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_render_startup");
    throw;
  }
}

/*
   Компонент регистрации рендера сцены
*/

class SceneRenderComponent
{
  public:
    //загрузка компонента
    SceneRenderComponent () 
    {
      StartupManager::RegisterStartupHandler ("SceneRender", &scene_render_startup, StartupGroup_Level3);
    }
};

extern "C"
{

ComponentRegistrator<SceneRenderComponent> SceneRender ("client.subsystems.scene_render");

}

}
