#include <stl/hash_map>

#include <xtl/connection.h>
#include <xtl/iterator.h>

#include <syslib/application.h>

#include <render/scene_render.h>

#include "shared.h"

using namespace engine;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace components
{

namespace scene_render_subsystem
{

/*
     онстанты
*/

const char*  COMPONENT_NAME            = "engine.subsystems.SceneRenderSubsystem"; //им€ компонента
const char*  LOG_NAME                  = COMPONENT_NAME;                           //им€ протокола
const char*  SUBSYSTEM_NAME            = "SceneRender";                            //подсистема
const size_t DEFAULT_IDLE_RENDER_COUNT = 8;                                        //количество резервируемых обновл€емых индексов рендер-таргетов
const char*  QUERY_PREFIX              = "screen ";                                //префикс маски запросов рендеринга экранов

/*
   ѕодсистема рендера сцены
*/

class SceneRenderSubsystem : public ISubsystem, public IAttachmentRegistryListener<scene_graph::Screen>,
  public media::rms::ICustomServer, public xtl::reference_counter
{
  public:
/// онструктор
    SceneRenderSubsystem (common::ParseNode& node, SubsystemManager& in_manager)
      : log (LOG_NAME)
      , render (get<const char*> (node, "DriverMask"), get<const char*> (node, "RendererMask"), get<const char*> (node, "RenderPathMasks", "*"))
      , on_app_stop_connection (syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnStop, xtl::bind (&SceneRenderSubsystem::OnStop, this)))
      , on_app_start_connection (syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnStart, xtl::bind (&SceneRenderSubsystem::OnStart, this)))
      , manager (in_manager)
    {
      try
      {
        SubscribeToOnIdle ();

        const char* log_name = get<const char*> (node, "Log", "");

        if (*log_name)
          Log (log_name).Swap (log);

        render.SetMaxDrawDepth (get<size_t> (node, "MaxDrawDepth", render.MaxDrawDepth ()));

        stl::string query_mask = common::format ("%s*", QUERY_PREFIX);

        render.RegisterQueryHandler (query_mask.c_str (), xtl::bind (&SceneRenderSubsystem::SetupDynamicRenderTarget, this, _1, _2));

        render.SetLogHandler (xtl::bind (&Log::Print, &log, _1));

        size_t render_target_index = 0;

        idle_render_targets.reserve (DEFAULT_IDLE_RENDER_COUNT);

        for (Parser::NamesakeIterator iter=node.First ("RenderTarget"); iter; ++iter, ++render_target_index)
        {
          const char* attachment        = get<const char*> (*iter, "Screen");
          const char* target_attachment = get<const char*> (*iter, "Id", "");

          if (render_target_index >= render.RenderTargetsCount ())
          {
            iter->Log ().Error (*iter, "Wrong <RenderTarget>=%u (RenderTargetsCount=%u)", render_target_index, render.RenderTargetsCount ());

            throw xtl::make_range_exception ("", "RenderTarget", render_target_index, render.RenderTargetsCount ());
          }

          screen_map.insert_pair (attachment, render_target_index);

          if (get<bool> (*iter, "IdleRender", true))
          {
            size_t      update_frequency = get<size_t> (*iter, "RenderFrequency", 0);
            stl::string on_after_update  = get<const char*> (*iter, "OnAfterUpdate", "");
            stl::string on_before_update = get<const char*> (*iter, "OnBeforeUpdate", "");

            RenderTargetPtr target (new RenderTargetDesc (render.RenderTarget (render_target_index), update_frequency, on_before_update, on_after_update, target_attachment), false);

            idle_render_targets.push_back (target);
          }          
        }

        AttachmentRegistry::Attach<scene_graph::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);
        
        try
        {
          resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);
        }
        catch (...)
        {
          AttachmentRegistry::Detach<scene_graph::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);
          throw;
        }
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("engine::SceneRenderSubsystem::SceneRenderSubsystem");
        throw;
      }
    }

///ƒеструктор
    ~SceneRenderSubsystem ()
    {
      resource_server = 0;      
      
      AttachmentRegistry::Detach<scene_graph::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);      
    }

/// —обыти€ установки/удалени€ экрана
    void OnRegisterAttachment (const char* attachment_name, scene_graph::Screen& screen)
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (&screen);
    }

    void OnUnregisterAttachment (const char* attachment_name, scene_graph::Screen&)
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (0);
    }

///”правление ресурсами
    void PrefetchResource (const char* resource_name)
    {
      //??????: do this !!!!!!!!!
    }

    void LoadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::SceneRenderSubsystem::LoadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        log.Printf ("Loading renderer resource '%s'...", resource_name);

        render.LoadResource (resource_name);
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
      static const char* METHOD_NAME = "engine::subsystems::SceneRenderSubsystem::UnloadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        log.Printf ("Unloading renderer resource '%s'...", resource_name);

        render.UnloadResource (resource_name);
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("Can't unload resource '%s', exception: %s", resource_name, exception.what ());

        exception.touch (METHOD_NAME);

        throw;
      }
    }

/// ѕодсчЄт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    void OnIdle ()
    {
      size_t current_time = common::milliseconds ();
      
      for (RenderTargetArray::iterator iter=idle_render_targets.begin (); iter!=idle_render_targets.end ();)
      {
        RenderTargetDesc& desc = **iter;

        if (!desc.target.IsBindedToRender ()) //если цель рендеринга удалена - удал€ем еЄ из списка отрисовки
        {
          idle_render_targets.erase (iter);

          continue;
        }
        
          //проверка синхронизации с частотой обновлени€ кадров
          
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

      //настройка динамического целевого буфера рендеринга
    void SetupDynamicRenderTarget (render::obsolete::RenderTarget& render_target, const char* query_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::SceneRenderSubsystem::SetupDynamicRenderTarget";

      if (!query_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "query_name");

      size_t prefix_length = xtl::xstrlen (QUERY_PREFIX);

      if (xtl::xstrlen (query_name) < prefix_length)
        throw xtl::make_argument_exception (METHOD_NAME, "query_name", query_name, "Query name is shorter than prefix");

      const char* screen_name = query_name + prefix_length;

      scene_graph::Screen *screen = AttachmentRegistry::Find<scene_graph::Screen> (screen_name);

      if (!screen)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't find screen '%s' needed for query '%s'", screen_name, query_name);

      render_target.SetScreen (screen);
    }

      //пауза приложени€
    void OnStop ()
    {
      resource_server->DisableNotifications ();

      idle_connection.disconnect ();
    }

      //восстановление приложени€
    void OnStart ()
    {
      resource_server->EnableNotifications ();

      SubscribeToOnIdle ();
    }

      //подспика на событие OnIdle
    void SubscribeToOnIdle ()
    {
      idle_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&SceneRenderSubsystem::OnIdle, this));
    }

  private:
    SceneRenderSubsystem (const SceneRenderSubsystem&);             //no impl
    SceneRenderSubsystem& operator = (const SceneRenderSubsystem&); //no impl

  private:
    struct RenderTargetDesc: public xtl::reference_counter
    {
      render::obsolete::RenderTarget target;
      stl::string                    on_before_update;
      stl::string                    on_after_update;
      stl::string                    target_attachment;
      size_t                         update_delay;
      size_t                         last_update_time;
      
      RenderTargetDesc (const render::obsolete::RenderTarget& in_target, size_t in_update_frequency, const stl::string& in_on_before_update, const stl::string& in_on_after_update, const char* in_target_attachment)
        : target (in_target)
        , on_before_update (in_on_before_update)
        , on_after_update (in_on_after_update)
        , target_attachment (in_target_attachment)
        , update_delay (in_update_frequency ? 1000 / in_update_frequency : 0)
        , last_update_time (0)
      {
        if (!target_attachment.empty ())
          AttachmentRegistry::Register (target_attachment.c_str (), target);
      }
      
      ~RenderTargetDesc ()
      {
        if (!target_attachment.empty ())
          AttachmentRegistry::Unregister (target_attachment.c_str (), target);
      }
    };
  
    typedef stl::hash_map<stl::hash_key<const char*>, size_t> ScreenMap;
    typedef xtl::intrusive_ptr<RenderTargetDesc>              RenderTargetPtr;
    typedef stl::vector<RenderTargetPtr>                      RenderTargetArray;

  private:
    Log                                              log;                      //лог
    render::obsolete::SceneRender                    render;                   //рендер
    stl::auto_ptr<media::rms::ServerGroupAttachment> resource_server;          //сервер ресурсов рендеринга
    ScreenMap                                        screen_map;               //соответствие экранов и рендер-таргетов
    xtl::auto_connection                             idle_connection;          //соединение обновлени€ рендер-таргетов
    xtl::auto_connection                             on_app_stop_connection;   //соединение остановки приложени€
    xtl::auto_connection                             on_app_start_connection;  //соединение восстановлени€ приложени€
    RenderTargetArray                                idle_render_targets;      //список автоматически обновл€емых целей рендеринга
    SubsystemManager&                                manager;
};

/*
    омпонент регистрации рендера сцены
*/

class SceneRenderComponent
{
  public:
    SceneRenderComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (common::ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<SceneRenderSubsystem> subsystem (new SceneRenderSubsystem (node, manager), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::SceneRenderComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<SceneRenderComponent> SceneRenderSubsystem (COMPONENT_NAME);

}

}

}
