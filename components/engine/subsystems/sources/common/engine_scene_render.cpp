#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/strlib.h>

#include <render/scene_render.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;
using namespace common;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME = "engine.subsystems.SceneRenderSubsystem"; //имя компонента
const char* LOG_NAME       = COMPONENT_NAME;                           //имя протокола
const char* SUBSYSTEM_NAME = "SceneRender";                            //подсистема

/*
   Подсистема рендера сцены               
*/

class SceneRenderSubsystem : public ISubsystem, public IAttachmentRegistryListener<render::Screen>, public xtl::reference_counter
{
  public:
///Конструктор
    SceneRenderSubsystem (common::ParseNode& node)
      : render (get<const char*> (node, "DriverMask"), get<const char*> (node, "RendererMask"), get<const char*> (node, "RenderPathMasks", "*")),
        log (LOG_NAME)
    {
      try
      {
        render.SetMaxDrawDepth (get<size_t> (node, "MaxDrawDepth", render.MaxDrawDepth ()));
        
        const char* log_name = get<const char*> (node, "Log", "");

        if (*log_name)
          Log (log_name).Swap (log);

        render.SetLogHandler (xtl::bind (&Log::Print, &log, _1));
        
        size_t render_target_index = 0;

        for (Parser::NamesakeIterator iter=node.First ("RenderTarget"); iter; ++iter, ++render_target_index)
        {
          const char* attachment = get<const char*> (*iter, "Screen");

          if (render_target_index >= render.RenderTargetsCount ())
          {
            iter->Log ().Error (*iter, "Wrong <RenderTarget>=%u (RenderTargetsCount=%u)", render_target_index, render.RenderTargetsCount ());
            
            throw xtl::make_range_exception ("", "RenderTarget", render_target_index, render.RenderTargetsCount ());
          }

          screen_map.insert_pair (attachment, render_target_index);
        }

        AttachmentRegistry::Attach (this, AttachmentRegistryAttachMode_ForceNotify);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("engine::SceneRenderSubsystem::SceneRenderSubsystem");
        throw;
      }
    }

///Деструктор
    ~SceneRenderSubsystem ()
    {
      AttachmentRegistry::Detach (this);
    }

/// События установки/удаления экрана
    void OnRegisterAttachment (const char* attachment_name, render::Screen& screen) 
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (&screen);
    }

    void OnUnregisterAttachment (const char* attachment_name, render::Screen&)
    {
      ScreenMap::iterator iter = screen_map.find (attachment_name);

      if (iter == screen_map.end ())
        return;

      render.RenderTarget (iter->second).SetScreen (0);
    }

/// Подсчёт ссылок
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    SceneRenderSubsystem (const SceneRenderSubsystem&);             //no impl
    SceneRenderSubsystem& operator = (const SceneRenderSubsystem&); //no impl

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, size_t> ScreenMap;

  private:
    render::SceneRender render;
    ScreenMap           screen_map;
    Log                 log;
};

/*
   Компонент регистрации рендера сцены
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

ComponentRegistrator<SceneRenderComponent> SceneRender (COMPONENT_NAME);

}

}
