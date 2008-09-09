#include "shared.h"

using namespace client;

/*
   Реализация движка
*/

struct Engine::Impl : public xtl::trackable
{
  public:
///Конструктор/деструктор
    Impl (const char* in_configuration_branch_name, IEngineStartupParams* engine_startup_params)
      : configuration_branch_name (in_configuration_branch_name)
      {}

///Установка клиента
    void AttachClient (const Client& new_client)
    {
      DisableAllListeners ();

      client = new_client;

      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        client.AttachEventListener (*iter);
    }

    void DetachClient ()
    {
      DisableAllListeners ();
      
      client::Client ().Swap (client);
    }

///Получение данных
    const char* ConfigurationBranch () const
    {
      return configuration_branch_name.c_str ();
    }

///Перебор подсистем
    size_t SubsystemsCount () const
    {
      return subsystems.size ();
    }

    IEngineSubsystem& Subsystem (size_t index) const
    {
      if (index >= subsystems.size ())
        throw xtl::make_range_exception ("client::Engine::Subsystem", "index", index, 0u, subsystems.size ());

      return *subsystems[index];
    }

///Добавление/удаление подсистем
    void AddSubsystem (const SubsystemPointer subsystem)
    {
      static const char* METHOD_NAME = "client::Engine::AddSubsystem";

      if (!subsystem)
        throw xtl::make_null_argument_exception (METHOD_NAME, "subsystem");

      Subsystems::iterator iter = stl::find (subsystems.begin (), subsystems.end (), subsystem);

      if (iter != subsystems.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Can't add subsystem '%s', already added", subsystem->Name ());

      subsystems.push_back (subsystem);
    }

    void RemoveSubsystem (const SubsystemPointer subsystem)
    {
      if (!subsystem)
        throw xtl::make_null_argument_exception ("client::Engine::RemoveSubsystem", "subsystem");

      Subsystems::iterator iter = stl::find (subsystems.begin (), subsystems.end (), subsystem);

      if (iter != subsystems.end ())
        subsystems.erase (iter);
    }

///Обработка событий ввода
    void ProcessInputEvent (const char* attachment_name, const char* event) const
    {
      try
      {
        client.ProcessInputEvent (attachment_name, event);
      }
      catch (xtl::exception& e)
      {
        e.touch ("client::Engine::ProcessInputEvent");
        throw;
      }
    }

///Работа со слушателями событий
    void AttachEventListener (IClientEventListener* listener)
    {
      static const char* METHOD_NAME = "client::Engine::AttachEventListener";

      if (!listener)
        throw xtl::make_null_argument_exception (METHOD_NAME, "listener");

      ListenerSet::iterator iter = listeners.find (listener);

      if (iter != listeners.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Listener already attached");

      listeners.insert (listener);
      client.AttachEventListener (listener);
    }

    void DetachEventListener (IClientEventListener* listener)
    {
      if (!listener)
        throw xtl::make_null_argument_exception ("client::Engine::DetachEventListener", "listener");

      listeners.erase (listener);
      client.DetachEventListener (listener);
    }

  private:
    void DisableAllListeners ()
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        client.DetachEventListener (*iter);
    }

  private:
    typedef Engine::SubsystemPointer SubsystemPointer;

    typedef stl::vector<SubsystemPointer>   Subsystems;
    typedef stl::set<IClientEventListener*> ListenerSet;

  private:
    client::Client client;
    stl::string    configuration_branch_name;
    Subsystems     subsystems;
    ListenerSet    listeners;
};

/*
   Движок
*/

/*
   Конструктор/деструктор
*/

Engine::Engine (const char* configuration_branch_name, IEngineStartupParams* engine_startup_params)
{
  if (!configuration_branch_name)
    throw xtl::make_null_argument_exception ("client::Engine::Engine", "configuration_branch_name");

  impl = new Impl (configuration_branch_name, engine_startup_params);

  StartupManagerSingleton::Instance ().Startup (*this, engine_startup_params);
}

Engine::~Engine ()
{
}

/*
   Установка клиента
*/

void Engine::AttachClient (const Client& client)
{
  impl->AttachClient (client);
}

void Engine::DetachClient ()
{
  impl->DetachClient ();
}

/*
   Получение данных
*/

const char* Engine::ConfigurationBranch () const
{
  return impl->ConfigurationBranch ();
}

/*
   Получение тракейбла
*/

xtl::trackable& Engine::GetTrackable () const
{
  return *impl;
}

/*
   Перебор подсистем
*/

size_t Engine::SubsystemsCount () const
{
  return impl->SubsystemsCount ();
}

IEngineSubsystem& Engine::Subsystem (size_t index) const
{
  return impl->Subsystem (index);
}

/*
   Добавление/удаление подсистем
*/

void Engine::AddSubsystem (const SubsystemPointer subsystem)
{
  impl->AddSubsystem (subsystem);
}

void Engine::RemoveSubsystem (const SubsystemPointer subsystem)
{
  impl->RemoveSubsystem (subsystem);
}

/*
   Обработка событий ввода
*/

void Engine::ProcessInputEvent (const char* attachment_name, const char* event) const
{
  impl->ProcessInputEvent (attachment_name, event);
}

/*
   Работа со слушателями событий
*/

void Engine::AttachEventListener (IClientEventListener* listener)
{
  impl->AttachEventListener (listener);
}

void Engine::DetachEventListener (IClientEventListener* listener)
{
  impl->DetachEventListener (listener);
}

namespace client
{

xtl::trackable& get_trackable (const Engine& engine)
{
  return engine.GetTrackable ();
}

}
