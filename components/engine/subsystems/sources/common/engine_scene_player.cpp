#include <stl/string>

#include <xtl/string.h>

#include <sound/scene_player.h>

#include "shared.h"

using namespace engine;
using namespace common;

namespace
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME = "ScenePlayer";                   //имя подсистемы
const char* COMPONENT_NAME = "engine.subsystems.ScenePlayer"; //имя компонента
const char* LOG_NAME       = COMPONENT_NAME;                  //имя лога

/*
   Подсистема рендера сцены
*/

class ScenePlayerSubsystem : public ISubsystem, public IAttachmentRegistryListener<scene_graph::Listener>,
                             public IAttachmentRegistryListener<media::rms::ResourceManager>,
                             public media::rms::ICustomServer, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    ScenePlayerSubsystem (common::ParseNode& node)
      : attachment (get<const char*> (node, "Listener")),
        resource_manager_name (get<const char*> (node, "ResourceManager")),
        sound_manager (get<const char*> (node, "DriverMask", "*"), get<const char*> (node, "DeviceMask", "*"), get<const char*> (node, "InitString", "")),
        resource_server (*this),
        attached_resource_manager (0),
        log (LOG_NAME)
    {
      scene_player.SetManager (&sound_manager);

      resource_server.SetFilters (get<const char*> (node, "ResourceMask", "*"));
      resource_server.SetCacheState (get<bool> (node, "CacheState", true));

      AttachmentRegistry::Attach<scene_graph::Listener> (this, AttachmentRegistryAttachMode_ForceNotify);

      try
      {
        AttachmentRegistry::Attach<media::rms::ResourceManager> (this, AttachmentRegistryAttachMode_ForceNotify);
      }
      catch (xtl::exception& exception)
      {
        AttachmentRegistry::Detach<scene_graph::Listener> (this);
        exception.touch ("engine::subsystems::ScenePlayerSubsystem::ScenePlayerSubsystem");
        throw;
      }
      catch (...)
      {
        AttachmentRegistry::Detach<scene_graph::Listener> (this);
        throw;
      }
    }

    ~ScenePlayerSubsystem ()
    {
      AttachmentRegistry::Detach<media::rms::ResourceManager> (this);
      AttachmentRegistry::Detach<scene_graph::Listener>       (this);
    }

///События установки / удаления менеджера ресурсов
    void OnRegisterAttachment (const char* attachment_name, media::rms::ResourceManager& resource_manager)
    {
      if (resource_manager_name == attachment_name)
      {
        resource_manager.Attach (resource_server);

        DetachServer ();

        attached_resource_manager = &resource_manager;
      }
    }

    void OnUnregisterAttachment (const char* attachment_name, media::rms::ResourceManager&)
    {
      if (resource_manager_name == attachment_name)
        DetachServer ();
    }

///События установки / удаления слушателя
    void OnRegisterAttachment (const char* attachment_name, scene_graph::Listener& listener)
    {
      if (attachment == attachment_name)
        scene_player.SetListener (&listener);
    }

    void OnUnregisterAttachment (const char* attachment_name, scene_graph::Listener&)
    {
      if (attachment == attachment_name)
        scene_player.SetListener (0);
    }

///Управление ресурсами
    void PrefetchResources (size_t count, const char** resource_names)
    {
      //??????: do this !!!!!!!!!
    }

    void LoadResources (size_t count, const char** resource_names)
    {
      static const char* METHOD_NAME = "engine::subsystems::ScenePlayerSubsystem::LoadResources";

      if (!resource_names)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_names");

      for (size_t i = 0; i < count; i++)
      {
        if (!resource_names[i])
          throw xtl::make_null_argument_exception (METHOD_NAME, "resource_names");

        try
        {
          sound_manager.LoadSoundLibrary (resource_names [i]);
        }
        catch (xtl::exception& exception)
        {
          log.Printf ("Can't load resource '%s', exception '%s'", resource_names[i], exception.what ());

          exception.touch (METHOD_NAME);

          throw;
        }
        catch (...)
        {
          log.Printf ("Can't load resource '%s', unknown exception", resource_names[i]);
          throw;
        }
      }
    }

    void UnloadResources (size_t count, const char** resource_names)
    {
      //?????: do this !!!!!!!!!
    }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    void DetachServer ()
    {
      if (!attached_resource_manager)
        return;

      attached_resource_manager->Detach (resource_server);
      attached_resource_manager = 0;
    }

  private:
    ScenePlayerSubsystem (const ScenePlayerSubsystem&);             //no impl
    ScenePlayerSubsystem& operator = (const ScenePlayerSubsystem&); //no impl

  private:
    stl::string                  attachment;
    stl::string                  resource_manager_name;
    sound::SoundManager          sound_manager;
    sound::ScenePlayer           scene_player;
    media::rms::Server           resource_server;
    media::rms::ResourceManager* attached_resource_manager;
    common::Log                  log;
};

/*
   Компонент регистрации проигрывателя сцены
*/

class ScenePlayerComponent
{
  public:
    //загрузка компонента
    ScenePlayerComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ScenePlayerSubsystem> subsystem (new ScenePlayerSubsystem (node), false);

        manager.AddSubsystem (subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::ScenePlayerComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<ScenePlayerComponent> ScenePlayerSubsystem (COMPONENT_NAME);

}

}
