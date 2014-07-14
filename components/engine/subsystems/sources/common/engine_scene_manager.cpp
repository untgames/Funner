#include <media/font_library.h>

#include <sg/scene_manager.h>
#include <sg/controllers/animation.h>

#include "shared.h"

using namespace engine;
using namespace common;

namespace components
{

namespace scene_manager_subsystem
{

namespace
{

/*
    Константы
*/

const char*  COMPONENT_NAME            = "engine.subsystems.SceneManagerSubsystem"; //имя компонента
const char*  LOG_NAME                  = COMPONENT_NAME;                            //имя протокола
const char*  SUBSYSTEM_NAME            = "SceneManager";                            //подсистема
const size_t RESERVED_SUBSYSTEMS_COUNT = 4;                                         //резервируемое количество вложенных подсистем

/*
    Вспомогательные классы
*/

///Интерфейс подсистемы менеджера сцен
class ISceneManagerSubsystem: public xtl::reference_counter
{
  public:
    virtual ~ISceneManagerSubsystem () {}
    
///Регистрация в контексте сцены
    virtual void OnSceneContextCreated (scene_graph::SceneContext& context) {}
};

///Анимационный менеджер
class AnimationManagerSubsystem: public ISceneManagerSubsystem, public media::rms::ICustomServer
{
  public:
///Конструктор
    AnimationManagerSubsystem (const common::ParseNode& node, const common::Log& in_log)
      : log (in_log)
    {
      try
      {
        attachment_name = get<const char*> (node, "AttachmentName", "");

        resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);

        if (!attachment_name.empty ())
          AttachmentRegistry::Register (attachment_name.c_str (), xtl::make_const_ref (xtl::ref (manager)));        
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::subsystems::AnimationManagerSubsystem");
        throw;
      }
    }
    
///Деструктор
    ~AnimationManagerSubsystem ()
    {
      if (!attachment_name.empty ())
        AttachmentRegistry::Unregister (attachment_name.c_str (), xtl::make_const_ref (xtl::ref (manager)));      
      
      resource_server = 0;
    }
    
///Управление ресурсами
    void PrefetchResource (const char* resource_name)
    {
    }

    void LoadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::AnimationManagerSubsystem::LoadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        manager.LoadResource (resource_name);
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
      static const char* METHOD_NAME = "engine::subsystems::AnimationManagerSubsystem::UnloadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        manager.UnloadResource (resource_name);
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("Can't unload resource '%s', exception: %s", resource_name, exception.what ());

        exception.touch (METHOD_NAME);

        throw;
      }
    }    
    
///Регистрация в контексте сцены
    void OnSceneContextCreated (scene_graph::SceneContext& context)
    {
      context.Attach (manager);
    }

  private:
    common::Log                                      log;              //поток протоколирования
    scene_graph::controllers::AnimationManager       manager;          //анимационный менеджер
    stl::auto_ptr<media::rms::ServerGroupAttachment> resource_server;  //сервер ресурсов рендеринга    
    stl::string                                      attachment_name;  //имя менеджера анимаций
};

///Менеджер шрифтов
class FontManagerSubsystem: public ISceneManagerSubsystem, public media::rms::ICustomServer
{
  public:
///Конструктор
    FontManagerSubsystem (const common::ParseNode& node, const common::Log& in_log)
      : log (in_log)
    {
      try
      {
        attachment_name = get<const char*> (node, "AttachmentName", "");

        resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);

        if (!attachment_name.empty ())
          AttachmentRegistry::Register (attachment_name.c_str (), xtl::make_const_ref (xtl::ref (library)));
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::subsystems::FontManagerSubsystem");
        throw;
      }
    }
    
///Деструктор
    ~FontManagerSubsystem ()
    {
      if (!attachment_name.empty ())
        AttachmentRegistry::Unregister (attachment_name.c_str (), xtl::make_const_ref (xtl::ref (library)));      
      
      resource_server = 0;
    }
    
///Управление ресурсами
    void PrefetchResource (const char* resource_name)
    {
    }

    void LoadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::FontManagerSubsystem::LoadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        library.LoadFonts (resource_name);
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
      static const char* METHOD_NAME = "engine::subsystems::FontManagerSubsystem::UnloadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        library.UnloadFonts (resource_name);
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("Can't unload resource '%s', exception: %s", resource_name, exception.what ());

        exception.touch (METHOD_NAME);

        throw;
      }
    }    
    
///Регистрация в контексте сцены
    void OnSceneContextCreated (scene_graph::SceneContext& context)
    {
      context.Attach (library);
    }

  private:
    common::Log                                      log;              //поток протоколирования
    media::FontLibrary                               library;          //библиотека шрифтов
    stl::auto_ptr<media::rms::ServerGroupAttachment> resource_server;  //сервер ресурсов рендеринга    
    stl::string                                      attachment_name;  //имя менеджера анимаций
};

}

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
        subsystems.reserve (RESERVED_SUBSYSTEMS_COUNT);
        
        const char* log_name = get<const char*> (node, "Log", "");
        
        attachment_name = get<const char*> (node, "AttachmentName", "");

        if (*log_name)
          Log (log_name).Swap (log);
                
        resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);

        if (!attachment_name.empty ())
          AttachmentRegistry::Register (attachment_name.c_str (), xtl::make_const_ref (xtl::ref (scene_manager)));
          
        for (common::Parser::Iterator iter=node.First (); iter; ++iter)
        {
          SubsystemPtr subsystem;

          const char* subsystem_name = iter->Name ();
          
          if (!strcmp (subsystem_name, "AnimationManager"))
          {
            subsystem = SubsystemPtr (new AnimationManagerSubsystem (*iter, log), false);
          }
          else if (!strcmp (subsystem_name, "FontLibrary"))
          {
            subsystem = SubsystemPtr (new FontManagerSubsystem (*iter, log), false);
          }

          if (subsystem)
            subsystems.push_back (subsystem);
        }
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
      if (!attachment_name.empty ())
        AttachmentRegistry::Unregister (attachment_name.c_str (), xtl::make_const_ref (xtl::ref (scene_manager)));

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
        scene_manager.LoadScene (resource_name, "", log_handler);
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
        scene_manager.UnloadScene (resource_name);
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
    typedef xtl::intrusive_ptr<ISceneManagerSubsystem> SubsystemPtr;
    typedef stl::vector<SubsystemPtr>                  SubsystemList;
  
    void OnSceneContextCreated (scene_graph::SceneContext& context)
    {
      try
      {        
          //установка функтора протоколирования

        context.SetLogHandler (log_handler);

          //оповещение подсистем

        for (SubsystemList::iterator iter=subsystems.begin (), end=subsystems.end (); iter!=end; ++iter)
          (*iter)->OnSceneContextCreated (context);
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::SceneManagerSubsystem::OnSceneContextCreated");
        throw;
      }
    }

  private:
    Log                                               log;              //лог
    scene_graph::SceneManager::LogHandler             log_handler;      //функтор протоколирования
    stl::auto_ptr<media::rms::ServerGroupAttachment>  resource_server;  //сервер ресурсов рендеринга
    scene_graph::SceneManager                         scene_manager;    //менеджер сцены
    stl::string                                       attachment_name;  //имя менеджера сцен
    SubsystemList                                     subsystems;       //подсистема
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
