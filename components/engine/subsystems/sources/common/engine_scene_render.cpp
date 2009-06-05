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

namespace
{

/*
     онстанты
*/

const char*  COMPONENT_NAME            = "engine.subsystems.SceneRenderSubsystem"; //им€ компонента
const char*  LOG_NAME                  = COMPONENT_NAME;                           //им€ протокола
const char*  SUBSYSTEM_NAME            = "SceneRender";                            //подсистема
const size_t DEFAULT_IDLE_RENDER_COUNT = 8;                                        //количество резервируемых обновл€емых индексов рендер-таргетов

/*
   ѕодсистема рендера сцены
*/

class SceneRenderSubsystem : public ISubsystem, public IAttachmentRegistryListener<render::Screen>,
                             public IAttachmentRegistryListener<media::rms::ResourceManager>,
                             public media::rms::ICustomServer, public xtl::reference_counter
{
  public:
/// онструктор
    SceneRenderSubsystem (common::ParseNode& node)
      : resource_manager_name (get<const char*> (node, "ResourceManager", "")),
        log (LOG_NAME),
        render (get<const char*> (node, "DriverMask"), get<const char*> (node, "RendererMask"), get<const char*> (node, "RenderPathMasks", "*")),
        resource_server (*this),
        attached_resource_manager (0),
        idle_connection (syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnIdle, xtl::bind (&SceneRenderSubsystem::OnIdle, this))),
        screen (0)
    {
      try
      {
        resource_server.SetFilters (get<const char*> (node, "ResourceMask", "*"));
        resource_server.SetCacheState (get<bool> (node, "CacheState", true));

        render.SetMaxDrawDepth (get<size_t> (node, "MaxDrawDepth", render.MaxDrawDepth ()));

        const char* log_name = get<const char*> (node, "Log", "");

        if (*log_name)
          Log (log_name).Swap (log);

        render.SetLogHandler (xtl::bind (&Log::Print, &log, _1));

        size_t render_target_index = 0;

        idle_render_targets.reserve (DEFAULT_IDLE_RENDER_COUNT);

        for (Parser::NamesakeIterator iter=node.First ("RenderTarget"); iter; ++iter, ++render_target_index)
        {
          const char* attachment = get<const char*> (*iter, "Screen");

          if (render_target_index >= render.RenderTargetsCount ())
          {
            iter->Log ().Error (*iter, "Wrong <RenderTarget>=%u (RenderTargetsCount=%u)", render_target_index, render.RenderTargetsCount ());

            throw xtl::make_range_exception ("", "RenderTarget", render_target_index, render.RenderTargetsCount ());
          }

          screen_map.insert_pair (attachment, render_target_index);

          if (get<bool> (node, "IdleRender", true))
            idle_render_targets.push_back (render.RenderTarget (render_target_index));
        }

        AttachmentRegistry::Attach<render::Screen> (this, AttachmentRegistryAttachMode_ForceNotify);

        try
        {
          AttachmentRegistry::Attach<media::rms::ResourceManager> (this, AttachmentRegistryAttachMode_ForceNotify);
        }
        catch (...)
        {
          AttachmentRegistry::Detach<render::Screen> (this);
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
      AttachmentRegistry::Detach<media::rms::ResourceManager> (this, AttachmentRegistryAttachMode_ForceNotify);
      AttachmentRegistry::Detach<render::Screen>              (this, AttachmentRegistryAttachMode_ForceNotify);
    }

///—обыти€ установки / удалени€ менеджера ресурсов
    void OnRegisterAttachment (const char* attachment_name, media::rms::ResourceManager& resource_manager)
    {
      if (!resource_manager_name.empty () && resource_manager_name == attachment_name)
      {
        resource_manager.Attach (resource_server);

        DetachServer ();

        attached_resource_manager = &resource_manager;
      }
    }

    void OnUnregisterAttachment (const char* attachment_name, media::rms::ResourceManager&)
    {
      if (!resource_manager_name.empty () && resource_manager_name == attachment_name)
        DetachServer ();
    }

/// —обыти€ установки/удалени€ экрана
    void OnRegisterAttachment (const char* attachment_name, render::Screen& screen)
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (&screen);

      this->screen = &screen;
    }

    void OnUnregisterAttachment (const char* attachment_name, render::Screen&)
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (0);

      this->screen = 0;
    }

///”правление ресурсами
    void PrefetchResources (size_t count, const char** resource_names)
    {
      //??????: do this !!!!!!!!!
    }

    void LoadResources (size_t count, const char** resource_names)
    {
      static const char* METHOD_NAME = "engine::subsystems::SceneRenderSubsystem::LoadResources";

      if (!resource_names)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_names");

      for (size_t i = 0; i < count; i++)
      {
        if (!resource_names[i])
          throw xtl::make_null_argument_exception (METHOD_NAME, "resource_names");

        try
        {
          render.LoadResource (resource_names [i]);
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
      static const char* METHOD_NAME = "engine::subsystems::SceneRenderSubsystem::UnloadResources";

      if (!resource_names)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_names");

      for (size_t i = 0; i < count; i++)
      {
        if (!resource_names[i])
          throw xtl::make_null_argument_exception (METHOD_NAME, "resource_names");

        try
        {
          render.UnloadResource (resource_names [i]);
        }
        catch (xtl::exception& exception)
        {
          log.Printf ("Can't unload resource '%s', exception '%s'", resource_names[i], exception.what ());

          exception.touch (METHOD_NAME);

          throw;
        }
        catch (...)
        {
          log.Printf ("Can't unload resource '%s', unknown exception", resource_names[i]);
          throw;
        }
      }
    }

/// ѕодсчЄт ссылок
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

    void OnIdle ()
    {
      for (RenderTargetArray::iterator iter=idle_render_targets.begin (); iter!=idle_render_targets.end ();)
      {
        if (!iter->IsBindedToRender ()) //если цель рендеринга удалена - удал€ем еЄ из списка отрисовки
        {
          idle_render_targets.erase (iter);

          continue;
        }

          //обновление цели рендеринга

        iter->Update ();

        ++iter;
      }

        //если все цели рендеринга удалены - перерисовывать нечего

      if (idle_render_targets.empty ())
        idle_connection.disconnect ();
    }

  private:
    SceneRenderSubsystem (const SceneRenderSubsystem&);             //no impl
    SceneRenderSubsystem& operator = (const SceneRenderSubsystem&); //no impl

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, size_t> ScreenMap;
    typedef stl::vector<render::RenderTarget>                 RenderTargetArray;

  private:
    stl::string                  resource_manager_name;      //им€ точки прив€зки менеджера ресурсов
    Log                          log;                        //лог
    render::SceneRender          render;                     //рендер
    media::rms::Server           resource_server;            //сервер ресурсов
    media::rms::ResourceManager* attached_resource_manager;  //присоединненный менеджер ресурсов
    ScreenMap                    screen_map;                 //соответствие экранов и рендер-таргетов
    xtl::auto_connection         idle_connection;            //соединение обновлени€ рендер-таргетов
    RenderTargetArray            idle_render_targets;        //список автоматически обновл€емых целей рендеринга
    render::Screen*              screen;
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
        xtl::com_ptr<SceneRenderSubsystem> subsystem (new SceneRenderSubsystem (node), false);

        manager.AddSubsystem (subsystem.get ());
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
