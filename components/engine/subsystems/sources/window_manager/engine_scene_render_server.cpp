#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/parser.h>
#include <common/property_map.h>
#include <common/strlib.h>

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

namespace
{

///Преобразователь
struct ParseNodeConverter
{
  common::PropertyMap result; //результирующая карта свойств
  
///Чтение свойства
  void ReadValue (common::Parser::AttributeIterator iter, const char* name)
  {
    stl::string value;

    for (bool first=true; iter; ++iter, first=false)
    {
      value += *iter;

      if (!first)
        value += ' ';
    }

    result.SetProperty (name, value.c_str ());
  }
  
///Преобразование
  void Convert (const common::ParseNode& node, const char* prefix = "")
  {
    stl::string name;  
    
    if (*prefix) name = common::format ("%s.%s", prefix, node.Name ());
    else         name = node.Name (); 
    
    if (node.First ())
    {
      for (common::Parser::Iterator iter=node.First (); iter; ++iter)
        Convert (*iter, name.c_str ());
    }
    else
    {
      ReadValue (make_attribute_iterator (node), name.c_str ());
    }
  }
};

///Преобразование узла конфигурации в карту свойств
common::PropertyMap to_properties (const common::ParseNode& node)
{
  ParseNodeConverter converter;
  
  for (common::Parser::Iterator iter=node.First (); iter; ++iter)
    converter.Convert (*iter);

  return converter.result;  
}

}

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

      const char *driver_mask  = get<const char*> (node, "DriverMask", (const char*)0),
                 *adapter_mask = get<const char*> (node, "AdapterMask", (const char*)0),
                 *init_string  = get<const char*> (node, "InitString", (const char*)0);

      server.reset (new Server (name, threading_model));

      for (Parser::NamesakeIterator iter=node.First ("RenderTarget"); iter; ++iter)
      {
        const char*         attachment = get<const char*> (*iter, "Window");
        const char*         name       = get<const char*> (*iter, "Id", attachment);
        syslib::Window&     window     = AttachmentRegistry::Get<syslib::Window> (attachment);
        common::PropertyMap properties = to_properties (*iter);

        if (driver_mask)  properties.SetProperty ("DriverMask", driver_mask);
        if (adapter_mask) properties.SetProperty ("AdapterMask", adapter_mask);
        if (init_string)  properties.SetProperty ("InitString", init_string);

        server->AttachWindow (name, window, properties);
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
