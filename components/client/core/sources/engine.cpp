#include "shared.h"

using namespace client;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_ENGINE_SUBSYSTEMS_RESERVE_SIZE = 128; //резервируемое количество подсистем

}

/*
    Реализация движка
*/

struct Engine::Impl : public IEngineEventListener, public xtl::trackable
{
  public:
///Конструктор
    Impl (const char* in_name, const char* in_configuration_branch_name, IEngineStartupParams* engine_startup_params)
      : name (in_name),
        configuration_branch_name (in_configuration_branch_name),
        start_level (0),
        log (common::format ("client.engines.%s", in_name).c_str ())
    {

      subsystems.reserve (DEFAULT_ENGINE_SUBSYSTEMS_RESERVE_SIZE);
    }

///Деструктор
    ~Impl ()
    {
      while (!subsystems.empty ())
        RemoveSubsystem (&*subsystems.front ());
      
      Detach ();
      OnDestroyNotify ();
    }
    
///Установка имени
    void SetName (const char* in_name)
    {
      if (!in_name)
        throw xtl::make_null_argument_exception ("client::Engine::SetName", "name");
        
      log.Printf ("Engine change name to '%s'", in_name);

      name = in_name;      

      common::Log (common::format ("client.engines.%s", in_name).c_str ()).Swap (log);
    }
    
///Получение имени
    const char* Name () { return name.c_str (); }

///Запуск систем
    void Start (Engine& engine, size_t new_start_level, IEngineStartupParams* engine_startup_params)
    {
      if (new_start_level <= start_level)
        return;

      StartupManagerSingleton::Instance ().Startup (engine, start_level + 1, new_start_level, engine_startup_params);  

      start_level = new_start_level;
    }

///Возвращение текущего уровня запуска
    size_t StartLevel () const
    {
      return start_level;
    }

///Установка точек привязки
    void Attach (const EngineAttachments& new_attachments)
    {
      Detach ();
      
      const_cast<EngineAttachments&> (new_attachments).Attach (this);

      attachments = new_attachments;
      
      for (EngineAttachments::ScreenIterator iter=attachments.CreateScreenIterator (); iter; ++iter)
        OnSetScreen (iter->Name (), iter->Value ());

      for (EngineAttachments::ListenerIterator iter=attachments.CreateListenerIterator (); iter; ++iter)
        OnSetListener (iter->Name (), iter->Value ());
        
      for (EngineAttachments::InputHandlerIterator iter=attachments.CreateInputHandlerIterator (); iter; ++iter)
        OnSetInputHandler (iter->Name (), iter->Value ());
    }

///Сброс точек привзяки
    void Detach ()
    {
      for (EngineAttachments::ScreenIterator iter=attachments.CreateScreenIterator (); iter; ++iter)
        OnRemoveScreen (iter->Name ());

      for (EngineAttachments::ListenerIterator iter=attachments.CreateListenerIterator (); iter; ++iter)
        OnRemoveListener (iter->Name ());
        
      for (EngineAttachments::InputHandlerIterator iter=attachments.CreateInputHandlerIterator (); iter; ++iter)
        OnRemoveInputHandler (iter->Name ());

      attachments.Detach (this);

      EngineAttachments ().Swap (attachments);
    }

///Получение имени ветки реестра с конфигурацией движка
    const char* ConfigurationBranch () const
    {
      return configuration_branch_name.c_str ();
    }

///Количество запущенных подсистем
    size_t SubsystemsCount () const
    {
      return subsystems.size ();
    }

///Получение подсистемы
    IEngineSubsystem& Subsystem (size_t index) const
    {
      if (index >= subsystems.size ())
        throw xtl::make_range_exception ("client::Engine::Subsystem", "index", index, 0u, subsystems.size ());

      return *subsystems[index];
    }

///Добавление подсистем
    void AddSubsystem (IEngineSubsystem* subsystem)
    {
      static const char* METHOD_NAME = "client::Engine::AddSubsystem";

      if (!subsystem)
        throw xtl::make_null_argument_exception (METHOD_NAME, "subsystem");

      Subsystems::iterator iter = stl::find (subsystems.begin (), subsystems.end (), subsystem);

      if (iter != subsystems.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Can't add subsystem '%s', already added", subsystem->Name ());

      subsystems.push_back (subsystem);
      
      log.Printf ("Add subsystem '%s'", subsystem->Name ());
    }

///Удаление подсистемы
    void RemoveSubsystem (IEngineSubsystem* subsystem)
    {
      if (!subsystem)
        return;

      Subsystems::iterator iter = stl::find (subsystems.begin (), subsystems.end (), subsystem);

      if (iter != subsystems.end ())
      {
        log.Printf ("Remove susbsystem '%s'", subsystem->Name ());
        subsystems.erase (iter);
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
      
      for (EngineAttachments::ScreenIterator iter=attachments.CreateScreenIterator (); iter; ++iter)
        listener->OnSetScreen (iter->Name (), iter->Value ());

      for (EngineAttachments::ListenerIterator iter=attachments.CreateListenerIterator (); iter; ++iter)
        listener->OnSetListener (iter->Name (), iter->Value ());
        
      for (EngineAttachments::InputHandlerIterator iter=attachments.CreateInputHandlerIterator (); iter; ++iter)
        listener->OnSetInputHandler (iter->Name (), iter->Value ());
    }

    void Detach (IEngineEventListener* listener)
    {
      if (!listener)      
        return;

      listeners.erase (listener);
    }

  private:
///События установки/удаления экрана
    void OnSetScreen (const char* attachment_name, render::Screen& screen) 
    {
      OnSetScreenNotify (attachment_name, screen);
    }

    void OnRemoveScreen (const char* attachment_name) 
    {
      OnRemoveScreenNotify (attachment_name);
    }

///События установки/удаления слушателя
    void OnSetListener (const char* attachment_name, scene_graph::Listener& listener) 
    {
      OnSetListenerNotify (attachment_name, listener);
    }

    void OnRemoveListener (const char* attachment_name) 
    {
      OnRemoveListenerNotify (attachment_name);
    }
    
///События установки/удаления обработчика событий ввода
    void OnSetInputHandler (const char* attachment_name, const InputHandler& handler) 
    {
      OnSetInputHandlerNotify (attachment_name, handler);
    }

    void OnRemoveInputHandler (const char* attachment_name) 
    {
      OnRemoveInputHandlerNotify (attachment_name);
    }    

///Событие удаления клиента
    void OnDestroy () 
    {
      OnDestroyNotify ();
    }

//Оповещение слушателей
    void OnSetScreenNotify (const char* attachment_name, render::Screen& screen)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnSetScreen (attachment_name, screen);
    }
    
    void OnRemoveScreenNotify (const char* attachment_name)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnRemoveScreen (attachment_name);
    }
    
    void OnSetListenerNotify (const char* attachment_name, scene_graph::Listener& listener)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnSetListener (attachment_name, listener);
    }
    
    void OnRemoveListenerNotify (const char* attachment_name)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnRemoveListener (attachment_name);
    }
    
    void OnSetInputHandlerNotify (const char* attachment_name, const EngineAttachments::InputHandler& handler)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnSetInputHandler (attachment_name, handler);
    }
    
    void OnRemoveInputHandlerNotify (const char* attachment_name)
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnRemoveInputHandler (attachment_name);
    }    

    void OnDestroyNotify ()
    {
      for (ListenerSet::iterator iter = listeners.begin (), end = listeners.end (); iter != end; ++iter)
        (*iter)->OnDestroy ();
    }

  private:
    typedef xtl::com_ptr<IEngineSubsystem>  SubsystemPtr; 
    typedef stl::vector<SubsystemPtr>       Subsystems;
    typedef stl::set<IEngineEventListener*> ListenerSet;

  private:
    EngineAttachments attachments;
    stl::string       name;
    stl::string       configuration_branch_name;
    Subsystems        subsystems;
    ListenerSet       listeners;
    size_t            start_level;
    common::Log       log;
};

/*
    Конструктор / деструктор
*/

Engine::Engine (const char* configuration_branch_name, size_t start_level, IEngineStartupParams* engine_startup_params)
{
  if (!configuration_branch_name)
    throw xtl::make_null_argument_exception ("client::Engine::Engine", "configuration_branch_name");
    
  static size_t default_name_id = 1;

  impl = new Impl (common::format ("Engine%u", default_name_id++).c_str (), configuration_branch_name, engine_startup_params);

  Start (start_level, engine_startup_params);
}

Engine::~Engine ()
{
}

/*
    Запуск систем
*/

void Engine::Start (size_t start_level, IEngineStartupParams* engine_startup_params)
{
  impl->Start (*this, start_level, engine_startup_params);
}

size_t Engine::StartLevel () const
{
  return impl->StartLevel ();
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
    Получение сигнала оповещения об удалении Engine
*/

xtl::trackable& Engine::GetTrackable () const
{
  return *impl;
}


namespace client
{

xtl::trackable& get_trackable (const Engine& engine)
{
  return engine.GetTrackable ();
}

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
    Добавление / удаление подсистем
*/

void Engine::AddSubsystem (IEngineSubsystem* subsystem)
{
  impl->AddSubsystem (subsystem);
}

void Engine::RemoveSubsystem (IEngineSubsystem* subsystem)
{
  impl->RemoveSubsystem (subsystem);
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
