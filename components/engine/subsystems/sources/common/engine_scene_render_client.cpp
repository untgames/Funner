#include <stl/hash_map>
#include <stl/hash_set>

#include <xtl/connection.h>
#include <xtl/iterator.h>

#include <syslib/application.h>

#include <media/font_library.h>
#include <media/particles/particle_system_library.h>

#include <render/scene_render_client.h>

#include "shared.h"

using namespace engine;
using namespace common;
using namespace render::scene::client;

//TODO: dynamic render targets

namespace components
{

namespace scene_render_client_subsystem
{

/*
    Константы
*/

const char*  COMPONENT_NAME            = "engine.subsystems.SceneRenderClientSubsystem"; //имя компонента
const char*  LOG_NAME                  = COMPONENT_NAME;                                 //имя протокола
const char*  SUBSYSTEM_NAME            = "SceneRenderClient";                            //подсистема
const size_t DEFAULT_IDLE_RENDER_COUNT = 8;                                              //резервируемое количество целей отрисовки

/*
   Подсистема рендера сцены
*/

class SceneRenderClientSubsystem : public ISubsystem, public IAttachmentRegistryListener<scene_graph::Screen>, public IAttachmentRegistryListener<media::FontLibrary>,
  public IAttachmentRegistryListener<media::particles::ParticleSystemLibrary>, public media::rms::ICustomServer, public xtl::reference_counter
{
  public:
///Конструктор
    SceneRenderClientSubsystem (common::ParseNode& node, SubsystemManager& in_manager)
      : log (LOG_NAME)
      , client (get<const char*> (node, "ConnectionName"), get<const char*> (node, "InitString", ""), get<size_t> (node, "LogonTimeout", size_t (-1)))
      , on_app_pause_connection (syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnPause, xtl::bind (&SceneRenderClientSubsystem::OnPause, this)))
      , on_app_resume_connection (syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnResume, xtl::bind (&SceneRenderClientSubsystem::OnResume, this)))
      , manager (in_manager)
    {
      try
      {
        SubscribeToOnIdle ();

        const char* log_name = get<const char*> (node, "Log", "");

        if (*log_name)
          Log (log_name).Swap (log);

        client.SetMaxDrawDepth (get<size_t> (node, "MaxDrawDepth", client.MaxDrawDepth ()));

        idle_render_targets.reserve (DEFAULT_IDLE_RENDER_COUNT);

        size_t render_target_index = 0;

        for (Parser::NamesakeIterator iter=node.First ("RenderTarget"); iter; ++iter)
        {
          if (get<bool> (*iter, "IdleRender", true))  //TODO create render target without IdleRender enabled
          {
            const char* attachment        = get<const char*> (*iter, "Screen");
            const char* target_attachment = get<const char*> (*iter, "Id");
            const char* init_string       = get<const char*> (*iter, "InitString", "");

            screen_map.insert_pair (attachment, render_target_index);

            size_t      update_frequency = get<size_t> (*iter, "RenderFrequency", 0);
            stl::string on_after_update  = get<const char*> (*iter, "OnAfterUpdate", "");
            stl::string on_before_update = get<const char*> (*iter, "OnBeforeUpdate", "");

            RenderTargetPtr target (new RenderTargetDesc (client.CreateRenderTarget (target_attachment, init_string), update_frequency, on_before_update, on_after_update, target_attachment), false);

            idle_render_targets.push_back (target);

            render_target_index++;
          }
        }

        for (Parser::NamesakeIterator iter=node.First ("FontLibrary"); iter; ++iter)
        {
          const char* name = get<const char*> (*iter, "Id");

          font_libraries.insert (name);
        }

        for (Parser::NamesakeIterator iter=node.First ("ParticleSystemLibrary"); iter; ++iter)
        {
          const char* name = get<const char*> (*iter, "Id");

          particle_system_libraries.insert (name);
        }

        try
        {
          AttachmentRegistry::Attach<scene_graph::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);
          AttachmentRegistry::Attach<media::FontLibrary> (this, AttachmentRegistryAttachMode_ForceNotify);
          AttachmentRegistry::Attach<media::particles::ParticleSystemLibrary> (this, AttachmentRegistryAttachMode_ForceNotify);
          
          resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);
        }
        catch (...)
        {
          AttachmentRegistry::Detach<media::particles::ParticleSystemLibrary> (this, AttachmentRegistryAttachMode_ForceNotify);
          AttachmentRegistry::Detach<media::FontLibrary> (this, AttachmentRegistryAttachMode_ForceNotify);
          AttachmentRegistry::Detach<scene_graph::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);
          throw;
        }
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("engine::SceneRenderClientSubsystem::SceneRenderClientSubsystem");
        throw;
      }
    }

///Деструктор
    ~SceneRenderClientSubsystem ()
    {
      resource_server = 0;      

      AttachmentRegistry::Detach<media::particles::ParticleSystemLibrary> (this, AttachmentRegistryAttachMode_ForceNotify);
      AttachmentRegistry::Detach<media::FontLibrary> (this, AttachmentRegistryAttachMode_ForceNotify);      
      AttachmentRegistry::Detach<scene_graph::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);
    }

/// События установки/удаления экрана
    void OnRegisterAttachment (const char* attachment_name, scene_graph::Screen& screen)
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      RenderTarget& render_target = idle_render_targets [iter->second]->target;
      
      render_target.SetScreen (&screen);
    }

    void OnUnregisterAttachment (const char* attachment_name, scene_graph::Screen&)
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      RenderTarget& render_target = idle_render_targets [iter->second]->target;

      render_target.SetScreen (0);
    }

/// События установки/удаления библиотеки шрифтов
    void OnRegisterAttachment (const char* name, media::FontLibrary& library)
    {
      if (!font_libraries.count (name))
        return;

      client.AttachFontLibrary (library);
    }

    void OnUnregisterAttachment (const char* name, media::FontLibrary& library)
    {
      if (!font_libraries.count (name))
        return;

      client.DetachFontLibrary (library);
    }

/// События установки/удаления библиотеки систем частиц
    void OnRegisterAttachment (const char* name, media::particles::ParticleSystemLibrary& library)
    {
      if (!particle_system_libraries.count (name))
        return;

      client.AttachParticleSystemLibrary (library);
    }

    void OnUnregisterAttachment (const char* name, media::particles::ParticleSystemLibrary& library)
    {
      if (!particle_system_libraries.count (name))
        return;

      client.DetachParticleSystemLibrary (library);
    }

///Управление ресурсами
    void PrefetchResource (const char* resource_name)
    {
    }

    void LoadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::SceneRenderClientSubsystem::LoadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        log.Printf ("Loading renderer resource '%s'...", resource_name);

        client.LoadResource (resource_name);
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
      static const char* METHOD_NAME = "engine::subsystems::SceneRenderClientSubsystem::UnloadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        log.Printf ("Unloading renderer resource '%s'...", resource_name);

        client.UnloadResource (resource_name);
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
    void OnIdle ()
    {
      size_t current_time = common::milliseconds ();
      
      for (RenderTargetArray::iterator iter=idle_render_targets.begin (); iter!=idle_render_targets.end ();)
      {
        RenderTargetDesc& desc = **iter;

          //проверка синхронизации с частотой обновления кадров
          
        if (current_time - desc.last_update_time < desc.update_delay)
        {
          ++iter;
          continue;
        }

          //обновление цели рендеринга
          
        desc.last_update_time = current_time;          
        if (!desc.on_before_update.empty ())
          manager.Execute (desc.on_before_update.c_str ());

        desc.target.Update ();
        
        if (!desc.on_after_update.empty ())
          manager.Execute (desc.on_after_update.c_str ());
        
        size_t end_time = common::milliseconds ();        
        
        desc.last_update_time -= (end_time - current_time);

        ++iter;
      }

        //если все цели рендеринга удалены - перерисовывать нечего

      if (idle_render_targets.empty ())
        idle_connection.disconnect ();
    }

      //пауза приложения
    void OnPause ()
    {
      resource_server->DisableNotifications ();

      idle_connection.disconnect ();

      client.Finish ();
    }

      //восстановление приложения
    void OnResume ()
    {
      resource_server->EnableNotifications ();

      SubscribeToOnIdle ();
    }

      //подспика на событие OnIdle
    void SubscribeToOnIdle ()
    {
      idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&SceneRenderClientSubsystem::OnIdle, this));
    }

  private:
    SceneRenderClientSubsystem (const SceneRenderClientSubsystem&);             //no impl
    SceneRenderClientSubsystem& operator = (const SceneRenderClientSubsystem&); //no impl

  private:
    struct RenderTargetDesc: public xtl::reference_counter
    {
      RenderTarget target;
      stl::string  on_before_update;
      stl::string  on_after_update;
      stl::string  target_attachment;
      size_t       update_delay;
      size_t       last_update_time;
      
      RenderTargetDesc (const RenderTarget& in_target, size_t in_update_frequency, const stl::string& in_on_before_update, const stl::string& in_on_after_update, const char* in_target_attachment)
        : target (in_target)
        , on_before_update (in_on_before_update)
        , on_after_update (in_on_after_update)
        , target_attachment (in_target_attachment)
        , update_delay (in_update_frequency ? 1000 / in_update_frequency : 0)
        , last_update_time (0)
      {
        if (!target_attachment.empty ())
        {
          xtl::reference_wrapper<RenderTarget> target_ref (target);

          AttachmentRegistry::Register (target_attachment.c_str (), target_ref);
        }
      }
      
      ~RenderTargetDesc ()
      {
        if (!target_attachment.empty ())
          AttachmentRegistry::Unregister (target_attachment.c_str (), target);
      }
    };

    typedef stl::hash_set<stl::hash_key<const char*> >        FontLibrarySet;
    typedef stl::hash_set<stl::hash_key<const char*> >        ParticleSystemLibrarySet;
    typedef stl::hash_map<stl::hash_key<const char*>, size_t> ScreenMap;
    typedef xtl::intrusive_ptr<RenderTargetDesc>              RenderTargetPtr;
    typedef stl::vector<RenderTargetPtr>                      RenderTargetArray;

  private:
    Log                                              log;                       //лог
    Client                                           client;                    //клиент рендеринга
    stl::auto_ptr<media::rms::ServerGroupAttachment> resource_server;           //сервер ресурсов рендеринга
    ScreenMap                                        screen_map;                //соответствие экранов и рендер-таргетов
    FontLibrarySet                                   font_libraries;            //библиотеки шрифтов
    ParticleSystemLibrarySet                         particle_system_libraries; //библиотеки систем частиц
    xtl::auto_connection                             idle_connection;           //соединение обновления рендер-таргетов
    xtl::auto_connection                             on_app_pause_connection;   //соединение паузы приложения
    xtl::auto_connection                             on_app_resume_connection;  //соединение восстановления приложения
    RenderTargetArray                                idle_render_targets;       //список автоматически обновляемых целей рендеринга
    SubsystemManager&                                manager;
};

/*
   Компонент регистрации рендера сцены
*/

class SceneRenderClientComponent
{
  public:
    SceneRenderClientComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (common::ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<SceneRenderClientSubsystem> subsystem (new SceneRenderClientSubsystem (node, manager), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::SceneRenderClientComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<SceneRenderClientComponent> SceneRenderClientSubsystem (COMPONENT_NAME);

}

}

}
