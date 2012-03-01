#include <sg/scene_manager.h>
#include <sg/controllers/animation.h>

#include "shared.h"

using namespace engine;
using namespace common;

namespace components
{

namespace scene_manager_subsystem
{

/*
    Константы
*/

const char*  COMPONENT_NAME = "engine.subsystems.SceneManagerSubsystem"; //имя компонента
const char*  LOG_NAME       = COMPONENT_NAME;                            //имя протокола
const char*  SUBSYSTEM_NAME = "SceneManager";                            //подсистема

/*
   Подсистема менеджера сцены
*/

class SceneManagerSubsystem : public ISubsystem, public media::rms::ICustomServer, public xtl::reference_counter, public xtl::noncopyable
{
  public:
///Конструктор
    SceneManagerSubsystem (common::ParseNode& node)
      : log (LOG_NAME)
      , log_handler (xtl::bind (&Log::Print, &log, _1))
    {
      try
      {
        const char* log_name = get<const char*> (node, "Log", "");
        
        scene_manager_attachment_name = get<const char*> (node, "Id", "");

        if (*log_name)
          Log (log_name).Swap (log);
                
        resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);

        if (!scene_manager_attachment_name.empty ())
          AttachmentRegistry::Register (scene_manager_attachment_name.c_str (), xtl::make_const_ref (xtl::ref (scene_manager)));
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("engine::SceneManagerSubsystem::SceneManagerSubsystem");
        throw;
      }
    }

///Деструктор
    ~SceneManagerSubsystem ()
    {
      if (!scene_manager_attachment_name.empty ())
        AttachmentRegistry::Unregister (scene_manager_attachment_name.c_str (), xtl::make_const_ref (xtl::ref (scene_manager)));

      resource_server = 0;
    }

///Управление ресурсами
    void PrefetchResource (const char* resource_name)
    {
    }

    void LoadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::SceneManagerSubsystem::LoadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        animation_manager.LoadResource (resource_name);
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("Can't load resource '%s', exception: %s", resource_name, exception.what ());

        exception.touch (METHOD_NAME);

        throw;
      }
    }

    void UnloadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::SceneManagerSubsystem::UnloadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        animation_manager.UnloadResource (resource_name);
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("Can't unload resource '%s', exception: %s", resource_name, exception.what ());

        exception.touch (METHOD_NAME);

        throw;
      }
    }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }
    
  private:
    void OnSceneContextCreated (scene_graph::SceneContext& context)
    {
      try
      {
          //подключение менеджера анимаций
          
        context.Attach (animation_manager);
        
          //установка функтора протоколирования

        context.SetLogHandler (log_handler);
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::SceneManagerSubsystem::OnSceneContextCreated");
        throw;
      }
    }

  private:
    Log                                               log;                           //лог
    scene_graph::SceneManager::LogHandler             log_handler;                   //функтор протоколирования
    stl::auto_ptr<media::rms::ServerGroupAttachment>  resource_server;               //сервер ресурсов рендеринга
    scene_graph::SceneManager                         scene_manager;                 //менеджер сцены
    scene_graph::controllers::AnimationManager        animation_manager;             //менеджер анимаций
    stl::string                                       scene_manager_attachment_name; //имя менеджера сцен
};

/*
   Компонент регистрации менеджера сцены
*/

class SceneManagerComponent
{
  public:
    SceneManagerComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (common::ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<SceneManagerSubsystem> subsystem (new SceneManagerSubsystem (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::SceneManagerComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<SceneManagerComponent> SceneManagerSubsystem (COMPONENT_NAME);

}

}

}
