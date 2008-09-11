#include "shared.h"

using namespace client;

/*
   Реализация движка
*/

struct Engine::Impl : public IEngineEventListener, public xtl::trackable
{
  public:
///Конструктор/деструктор
    Impl (const char* in_configuration_branch_name, IEngineStartupParams* engine_startup_params)
      : configuration_branch_name (in_configuration_branch_name)
      {}

    ~Impl ()
    {
      Detach ();
      OnDestroyNotify ();
    }

///Установка точек привязки
    void Attach (const EngineAttachments& new_attachments)
    {
      Detach ();
      
      const_cast<EngineAttachments&> (new_attachments).Attach (this);

      attachments = new_attachments;

      for (size_t i = 0; i < attachments.ScreensCount (); i++)
        OnSetScreen (attachments.ScreenName (i), attachments.Screen (i));

      for (size_t i = 0; i < attachments.ListenersCount (); i++)
        OnSetListener (attachments.ListenerName (i), attachments.Listener (i));
    }

    void Detach ()
    {
      RemoveAllScreens ();
      RemoveAllListeners ();

      attachments.Detach (this);
      
      EngineAttachments ().Swap (attachments);
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
        attachments.ProcessInputEvent (attachment_name, event);
      }
      catch (xtl::exception& e)
      {
        e.touch ("client::Engine::ProcessInputEvent");
        throw;
      }
    }

///Работа со слушателями событий
    void Attach (IEngineEventListener* listener)
    {
      static const char* METHOD_NAME = "client::Engine::Attach(IEngineEventListener*)";

      if (!listener)
        throw xtl::make_null_argument_exception (METHOD_NAME, "listener");

      ListenerSet::iterator iter = listeners.find (listener);

      if (iter != listeners.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Listener already attached");

      listeners.insert (listener);
    }

    void Detach (IEngineEventListener* listener)
    {
      if (!listener)      
        return;

      listeners.erase (listener);
    }

///События установки/удаления экрана
    void OnSetScreen (const char* attachment_name, render::Screen* screen) 
    {
      screen_attachments.insert (attachment_name);
      OnSetScreenNotify (attachment_name, screen);
    }

    void OnRemoveScreen (const char* attachment_name) 
    {
      screen_attachments.erase (attachment_name);
      OnRemoveScreenNotify (attachment_name);
    }

///События установки/удаления слушателя
    void OnSetListener (const char* attachment_name, scene_graph::Listener* listener) 
    {
      listener_attachments.insert (attachment_name);
      OnSetListenerNotify (attachment_name, listener);
    }

    void OnRemoveListener (const char* attachment_name) 
    {
      listener_attachments.erase (attachment_name);
      OnRemoveListenerNotify (attachment_name);
    }

///Событие удаления клиента
    void OnDestroy () 
    {
      OnDestroyNotify ();
    }

  private:
    void RemoveAllScreens ()
    {
      for (AttachmentsSet::iterator iter = screen_attachments.begin (), end = screen_attachments.end (); iter != end; ++iter)
        OnRemoveScreenNotify (iter->c_str ());

      screen_attachments.clear ();
    }

    void RemoveAllListeners ()
    {
      for (AttachmentsSet::iterator iter = listener_attachments.begin (), end = listener_attachments.end (); iter != end; ++iter)
        OnRemoveListenerNotify (iter->c_str ());

      listener_attachments.clear ();
    }

//Оповещение слушателей
    void OnSetScreenNotify (const char* attachment_name, render::Screen* screen)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnSetScreen (attachment_name, screen);
    }
    
    void OnRemoveScreenNotify (const char* attachment_name)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnRemoveScreen (attachment_name);
    }
    
    void OnSetListenerNotify (const char* attachment_name, scene_graph::Listener* listener)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnSetListener (attachment_name, listener);
    }
    
    void OnRemoveListenerNotify (const char* attachment_name)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnRemoveListener (attachment_name);
    }

    void OnDestroyNotify ()
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnDestroy ();
    }

  private:
    typedef Engine::SubsystemPointer SubsystemPointer;

    typedef stl::vector<SubsystemPointer>   Subsystems;
    typedef stl::set<IEngineEventListener*> ListenerSet;
    typedef stl::hash_set<stl::string>      AttachmentsSet;

  private:
    EngineAttachments attachments;
    stl::string       configuration_branch_name;
    Subsystems        subsystems;
    AttachmentsSet    screen_attachments;
    AttachmentsSet    listener_attachments;
    ListenerSet       listeners;
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
   Установка точек привязки
*/

void Engine::Attach (const EngineAttachments& attachments)
{
  impl->Attach (attachments);
}

void Engine::Detach ()
{
  impl->Detach ();
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

void Engine::Attach (IEngineEventListener* listener)
{
  impl->Attach (listener);
}

void Engine::Detach (IEngineEventListener* listener)
{
  impl->Detach (listener);
}

namespace client
{

xtl::trackable& get_trackable (const Engine& engine)
{
  return engine.GetTrackable ();
}

}
