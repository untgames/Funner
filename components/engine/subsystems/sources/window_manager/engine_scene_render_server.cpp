#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/parser.h>

#include <render/scene_render_server.h>

#include <engine/attachments.h>
#include <engine/subsystem_manager.h>

using namespace engine;
using namespace common;
using namespace render::scene::server;

namespace components
{

namespace scene_render_server_subsystem
{

/*
    Константы
*/

const char* COMPONENT_NAME = "engine.subsystems.SceneRenderServer"; //имя компонента
const char* SUBSYSTEM_NAME = "SceneRenderServer";                   //имя подсистемы

/*
    Подсистема инициализации оконной системы рендеринга
*/

class SceneRenderServerSubsystem: public ISubsystem, public xtl::reference_counter
{
  public:
///Конструктор
    SceneRenderServerSubsystem (ParseNode& node)    
    {
      const char* name                 = get<const char*> (node, "Id");
      const char* threading_model_name = get<const char*> (node, "ThreadingModel", "SingleThreaded");
      
      ServerThreadingModel threading_model = ServerThreadingModel_SingleThreaded;

      if      (!strcmp (threading_model_name, "MultiThreaded"))  threading_model = ServerThreadingModel_MultiThreaded;
      else if (!strcmp (threading_model_name, "SingleThreaded")) threading_model = ServerThreadingModel_SingleThreaded;
      else                                                       common::raise_parser_exception (node.First ("ThreadingModel"), "Invalid threading model '%s'", threading_model_name);

      server.reset (new Server (name, threading_model));

      for (Parser::NamesakeIterator iter=node.First ("Window"); iter; ++iter)
      {
        const char*     attachment = get<const char*> (*iter, "Id");
        const char*     name       = get<const char*> (*iter, "Name", attachment);
        syslib::Window& window     = AttachmentRegistry::Get<syslib::Window> (attachment);

        server->AttachWindow (name, window, *iter);
      }
    }

///Подсчёт ссылок
    void AddRef () { addref (this); }
    void Release () { release (this); }

  private:
    stl::auto_ptr<Server> server; //сервер систмы рендеринга
};

/*
   Компонент регистрации оконной системы рендеринга
*/

class SceneRenderServerComponent
{
  public:
    SceneRenderServerComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ISubsystem> subsystem (new SceneRenderServerSubsystem (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::SceneRenderServerComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<SceneRenderServerComponent> SceneRenderServerSubsystem (COMPONENT_NAME);

}

}

}
