#include "shared.h"

using namespace engine;
using namespace common;
using namespace physics;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

namespace
{

/*
    Константы
*/

const char* SUBSYSTEM_NAME = "PhysicsManager";                   //имя подсистемы
const char* COMPONENT_NAME = "engine.subsystems.PhysicsManager"; //имя компонента
const char* LOG_NAME       = COMPONENT_NAME;                     //имя лога

/*
   Подсистема управления физикой
*/

class PhysicsManagerSubsystem : public ISubsystem, public media::rms::ICustomServer, public xtl::reference_counter
{
  public:
/// Конструктор/деструктор
    PhysicsManagerSubsystem (common::ParseNode& node)
      : log (LOG_NAME),
        physics_manager (get<const char*> (node, "DriverName", "*"))
    {
      try
      {
        AttachmentRegistry::Register (get<const char*> (node, "AttachmentName"), xtl::make_const_ref (xtl::ref (physics_manager)));

        resource_server = new media::rms::ServerGroupAttachment (get<const char*> (node, "ResourceServer", SUBSYSTEM_NAME), *this);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("engine::PhysicsManagerSubsystem::PhysicsManagerSubsystem");
        throw;
      }
    }

    ~PhysicsManagerSubsystem ()
    {
      resource_server.reset ();
    }

///Управление ресурсами
    void PrefetchResource (const char* resource_name)
    {
      //??????: do this !!!!!!!!!
    }

    void LoadResource (const char* resource_name)
    {
      static const char* METHOD_NAME = "engine::subsystems::PhysicsManagerSubsystem::LoadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        physics_manager.LoadLibrary (resource_name);
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
      static const char* METHOD_NAME = "engine::subsystems::PhysicsManagerSubsystem::UnloadResource";

      if (!resource_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "resource_name");

      try
      {
        physics_manager.UnloadLibrary (resource_name);
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
    PhysicsManagerSubsystem (const PhysicsManagerSubsystem&);             //no impl
    PhysicsManagerSubsystem& operator = (const PhysicsManagerSubsystem&); //no impl

  private:
    common::Log                                      log;
    PhysicsManager                                   physics_manager;
    stl::auto_ptr<media::rms::ServerGroupAttachment> resource_server;
};

}

namespace components
{

namespace scene_player_subsystem
{

/*
   Компонент регистрации проигрывателя сцены
*/

class PhysicsManagerComponent
{
  public:
    //загрузка компонента
    PhysicsManagerComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<PhysicsManagerSubsystem> subsystem (new PhysicsManagerSubsystem (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::PhysicsManagerComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<PhysicsManagerComponent> PhysicsManagerSubsystem (COMPONENT_NAME);

}

}

}
