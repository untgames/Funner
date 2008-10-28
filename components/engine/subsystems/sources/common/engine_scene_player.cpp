#include <stl/string>

#include <xtl/any.h>
#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <common/component.h>

#include <sound/scene_player.h>

#include <client/engine.h>

using namespace client;
using namespace common;

namespace
{

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

stl::string get (VarRegistry& registry, const char* name, const char* default_value)
{
  if (registry.HasVariable (name))
    return get_string (registry, name);

  return stl::string (default_value);
}

/*
   Подсистема рендера сцены               
*/

class ScenePlayerSubsystem : public IEngineSubsystem, public IEngineEventListener, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    ScenePlayerSubsystem (common::VarRegistry& var_registry, Engine& in_engine)
      : engine (&in_engine),
        attachment (get_string (var_registry, "Attachment")),
        sound_manager (get (var_registry, "DriverMask", "*").c_str (), get (var_registry, "DeviceMask", "*").c_str (), 
                       get (var_registry, "InitString", "").c_str ())
    {
      scene_player.SetManager (&sound_manager);

      try
      {
        engine->Attach (this);
      }
      catch (...)
      {
        engine->Detach (this);
      }
    }

    ~ScenePlayerSubsystem ()
    {
      engine->Detach (this);
    }
    
/// Получение имени
    const char* Name () { return "ScenePlayer"; }

/// Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

///События установки / удаления слушателя
    void OnSetListener (const char* attachment_name, scene_graph::Listener& listener)
    {
      if (!xtl::xstrcmp (attachment.c_str (), attachment_name))
        scene_player.SetListener (&listener);
    }

    void OnRemoveListener (const char* attachment_name)
    {
      if (!xtl::xstrcmp (attachment.c_str (), attachment_name))
        scene_player.SetListener (0);
    }

  private:
    ScenePlayerSubsystem (const ScenePlayerSubsystem&);             //no impl
    ScenePlayerSubsystem& operator = (const ScenePlayerSubsystem&); //no impl

  private:
    Engine*             engine;
    stl::string         attachment;
    sound::SoundManager sound_manager;
    sound::ScenePlayer  scene_player;
};



/*
   Создание проигрывателя
*/

void scene_player_startup (common::VarRegistry& var_registry, IEngineStartupParams*, Engine& engine)
{
  try
  {
    test_registry_variable (var_registry, "Attachment");

    xtl::com_ptr<ScenePlayerSubsystem> new_subsystem (new ScenePlayerSubsystem (var_registry, engine), false);

    engine.AddSubsystem (new_subsystem.get ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_player_startup");
    throw;
  }
}

/*
   Компонент регистрации проигрывателя сцены
*/

class ScenePlayerComponent
{
  public:
    //загрузка компонента
    ScenePlayerComponent () 
    {
      StartupManager::RegisterStartupHandler ("ScenePlayer", &scene_player_startup, StartupGroup_Level3);
    }
};

extern "C"
{

ComponentRegistrator<ScenePlayerComponent> ScenePlayer ("client.subsystems.scene_player");

}

}
