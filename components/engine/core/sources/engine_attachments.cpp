#include "shared.h"

using namespace client;

/*
    Реализация клиента
*/

struct EngineAttachments::Impl: public xtl::reference_counter
{
  public:
///Деструктор
    ~Impl ()
    {
      RemoveAllScreens       ();
      RemoveAllListeners     ();
      RemoveAllInputHandlers ();
      OnDestroyNotify        ();
    }

///Установка экрана
    void SetScreen (const char* attachment_name, render::Screen& screen)
    {
      static const char* METHOD_NAME = "EngineAttachments::EngineAttachments::SetScreen";

      if (!attachment_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "attachment_name");

      ScreenAttachments::iterator iter = screen_attachments.find (attachment_name);

      if (iter != screen_attachments.end ())
      {
        iter->second->screen = screen;
      }
      else
      {
        screen_attachments.insert_pair (attachment_name, ScreenAttachmentPtr (new ScreenAttachment (attachment_name, screen), false));
      }

      OnSetScreenNotify (attachment_name, screen);
    }

///Удаление экрана
    void RemoveScreen (const char* attachment_name)
    {
      if (!attachment_name)
        return;

      ScreenAttachments::iterator iter = screen_attachments.find (attachment_name);

      if (iter == screen_attachments.end ())
        return;

      OnRemoveScreenNotify     (attachment_name);
      screen_attachments.erase (iter);
    }

///Удаление всех экранов
    void RemoveAllScreens ()
    {
      for (ScreenAttachments::iterator iter=screen_attachments.begin (), end=screen_attachments.end (); iter!=end; ++iter)
        OnRemoveScreenNotify (iter->second->name.c_str ());

      screen_attachments.clear ();
    }

///Поиск экрана
    render::Screen* FindScreen (const char* attachment_name)
    {
      if (!attachment_name)
        return 0;

      ScreenAttachments::iterator iter = screen_attachments.find (attachment_name);

      if (iter != screen_attachments.end ())
        return &iter->second->screen;

      return 0;
    }
    
///Перебор экранов
    ScreenIterator CreateScreenIterator ()
    {
      return ScreenIterator (screen_attachments.begin (), screen_attachments.begin (), screen_attachments.end (), ScreenSelector ());
    }

///Установка слушателя
    void SetListener (const char* attachment_name, scene_graph::Listener& listener)
    {
      static const char* METHOD_NAME = "client::EngineAttachments::SetListener";

      if (!attachment_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "attachment_name");

      ListenerAttachments::iterator iter = listener_attachments.find (attachment_name);

      if (iter != listener_attachments.end ())
      {
        iter->second->listener = &listener;
      }
      else
      {        
        listener_attachments.insert_pair (attachment_name, ListenerAttachmentPtr (new ListenerAttachment (attachment_name, listener), false));
      }

      OnSetListenerNotify (attachment_name, listener);
    }

///Удаление слушателя
    void RemoveListener (const char* attachment_name)
    {
      if (!attachment_name)
        return;

      ListenerAttachments::iterator iter = listener_attachments.find (attachment_name);

      if (iter == listener_attachments.end ())
        return;

      OnRemoveListenerNotify     (attachment_name);
      listener_attachments.erase (iter);
    }

///Удаление всех слушателей
    void RemoveAllListeners ()
    {
      for (ListenerAttachments::iterator iter=listener_attachments.begin (), end=listener_attachments.end (); iter!=end; ++iter)
        OnRemoveListenerNotify (iter->second->name.c_str ());

      listener_attachments.clear ();
    }

///Поиск слушателя по имени
    scene_graph::Listener* FindListener (const char* attachment_name)
    {
      if (!attachment_name)
        return 0;

      ListenerAttachments::iterator iter = listener_attachments.find (attachment_name);

      if (iter != listener_attachments.end ())
        return iter->second->listener.get ();

      return 0;
    }
    
///Перебор слушателей
    ListenerIterator CreateListenerIterator ()
    {
      return ListenerIterator (listener_attachments.begin (), listener_attachments.begin (), listener_attachments.end (), ListenerSelector ());
    }
    
///Регистрация обработчика событий ввода
    void SetInputHandler (const char* attachment_name, const InputHandler& handler)
    {
      static const char* METHOD_NAME = "EngineAttachments::EngineAttachments::SetScreen";

      if (!attachment_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "attachment_name");

      InputHandlerAttachments::iterator iter = input_attachments.find (attachment_name);

      if (iter != input_attachments.end ())
      {
        iter->second->handler = handler;
      }
      else
      {
        input_attachments.insert_pair (attachment_name, InputHandlerAttachmentPtr (new InputHandlerAttachment (attachment_name, handler), false));
      }

      OnSetInputHandlerNotify (attachment_name, handler);
    }
    
///Удаление обработчика событий ввода
    void RemoveInputHandler (const char* attachment_name)
    {
      if (!attachment_name)
        return;
        
      InputHandlerAttachments::iterator iter = input_attachments.find (attachment_name);
      
      if (iter == input_attachments.end ())
        return;
        
      OnRemoveInputHandlerNotify (attachment_name);
      input_attachments.erase    (iter);
    }
    
///Удаление всех обработчиков событий ввода
    void RemoveAllInputHandlers ()
    {
      for (InputHandlerAttachments::iterator iter=input_attachments.begin (), end=input_attachments.end (); iter!=end; ++iter)
        OnRemoveInputHandlerNotify (iter->second->name.c_str ());

      input_attachments.clear ();
    }
    
///Поиск обработчика событий ввода
    const InputHandler* FindInputHandler (const char* attachment_name)
    {
      if (!attachment_name)
        return 0;
      
      InputHandlerAttachments::iterator iter = input_attachments.find (attachment_name);
      
      if (iter != input_attachments.end ())
        return &iter->second->handler;
        
      return 0;
    }
    
///Перебор обработчиков событий ввода
    InputHandlerIterator CreateInputHandlerIterator ()
    {
      return InputHandlerIterator (input_attachments.begin (), input_attachments.begin (), input_attachments.end (), InputHandlerSelector ());
    }

///Работа со слушателями событий
    void Attach (IEngineEventListener* listener)
    {
      static const char* METHOD_NAME = "client::EngineAttachments::Attach";

      if (!listener)
        throw xtl::make_null_argument_exception (METHOD_NAME, "listener");

      EventListenerSet::iterator iter = event_listeners.find (listener);

      if (iter != event_listeners.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Listener already attached");

      event_listeners.insert (listener);
    }

    void Detach (IEngineEventListener* listener)
    {
      if (!listener)
        throw xtl::make_null_argument_exception ("client::EngineAttachments::Detach", "listener");

      event_listeners.erase (listener);
    }

  private:
//Оповещение слушателей
    void OnSetScreenNotify (const char* attachment_name, render::Screen& screen)
    {
      for (EventListenerSet::iterator iter = event_listeners.begin (), end = event_listeners.end (); iter != end; ++iter)
        (*iter)->OnSetScreen (attachment_name, screen);
    }
    
    void OnRemoveScreenNotify (const char* attachment_name)
    {
      for (EventListenerSet::iterator iter = event_listeners.begin (), end = event_listeners.end (); iter != end; ++iter)
        (*iter)->OnRemoveScreen (attachment_name);
    }
    
    void OnSetListenerNotify (const char* attachment_name, scene_graph::Listener& listener)
    {
      for (EventListenerSet::iterator iter = event_listeners.begin (), end = event_listeners.end (); iter != end; ++iter)
        (*iter)->OnSetListener (attachment_name, listener);
    }
    
    void OnRemoveListenerNotify (const char* attachment_name)
    {
      for (EventListenerSet::iterator iter = event_listeners.begin (), end = event_listeners.end (); iter != end; ++iter)
        (*iter)->OnRemoveListener (attachment_name);
    }
    
    void OnSetInputHandlerNotify (const char* attachment_name, const InputHandler& handler)
    {
      for (EventListenerSet::iterator iter = event_listeners.begin (), end = event_listeners.end (); iter != end; ++iter)
        (*iter)->OnSetInputHandler (attachment_name, handler);
    }
    
    void OnRemoveInputHandlerNotify (const char* attachment_name)
    {
      for (EventListenerSet::iterator iter = event_listeners.begin (), end = event_listeners.end (); iter != end; ++iter)
        (*iter)->OnRemoveInputHandler (attachment_name);
    }    

    void OnDestroyNotify ()
    {
      for (EventListenerSet::iterator iter = event_listeners.begin (), end = event_listeners.end (); iter != end; ++iter)
        (*iter)->OnDestroy ();
    }
   
  private:
    struct ScreenAttachment: public IAttachment<render::Screen>, public xtl::reference_counter
    {
      stl::string    name;
      render::Screen screen;
      
      ScreenAttachment (const char* in_name, render::Screen& in_screen) : name (in_name), screen (in_screen) {}                  

      const char*     Name  () { return name.c_str (); }
      render::Screen& Value () { return screen; }
    };
    
    struct ListenerAttachment: public IAttachment<scene_graph::Listener>, public xtl::reference_counter
    {
      stl::string                    name;
      scene_graph::Listener::Pointer listener;
      
      ListenerAttachment (const char* in_name, scene_graph::Listener& in_listener) : name (in_name), listener (&in_listener) {}

      const char*            Name  () { return name.c_str (); }
      scene_graph::Listener& Value () { return *listener; }
    };
    
    struct InputHandlerAttachment: public IAttachment<const InputHandler>, public xtl::reference_counter
    {
      stl::string  name;
      InputHandler handler;
      
      InputHandlerAttachment (const char* in_name, const InputHandler& in_handler) : name (in_name), handler (in_handler) {} 

      const char*         Name  () { return name.c_str (); }
      const InputHandler& Value () { return handler; }
    };    

    typedef xtl::intrusive_ptr<ScreenAttachment>                                 ScreenAttachmentPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, ScreenAttachmentPtr>       ScreenAttachments;
    typedef xtl::intrusive_ptr<ListenerAttachment>                               ListenerAttachmentPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, ListenerAttachmentPtr>     ListenerAttachments;
    typedef xtl::intrusive_ptr<InputHandlerAttachment>                           InputHandlerAttachmentPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, InputHandlerAttachmentPtr> InputHandlerAttachments;
    typedef stl::hash_set<IEngineEventListener*>                                 EventListenerSet;    
    
    struct ScreenSelector
    {
      IAttachment<render::Screen>& operator () (ScreenAttachments::value_type& value) const { return *value.second; }
    };    
    
    struct ListenerSelector
    {
      IAttachment<scene_graph::Listener>& operator () (ListenerAttachments::value_type& value) const { return *value.second; }
    };    
    
    struct InputHandlerSelector
    {
      IAttachment<const InputHandler>& operator () (InputHandlerAttachments::value_type& value) const { return *value.second; }
    };    

  private:
    ScreenAttachments       screen_attachments;
    ListenerAttachments     listener_attachments;
    InputHandlerAttachments input_attachments;
    EventListenerSet        event_listeners;
};

/*
===================================================================================================
    EngineAttachments
===================================================================================================
*/

/*
    Конструктор / деструктор / присваивание
*/

EngineAttachments::EngineAttachments ()
  : impl (new Impl)
{
}

EngineAttachments::EngineAttachments (const EngineAttachments& source)
  : impl (source.impl)
{
  addref (impl);
}

EngineAttachments::~EngineAttachments ()
{
  release (impl);
}

EngineAttachments& EngineAttachments::operator = (const EngineAttachments& source)
{
  EngineAttachments (source).Swap (*this);

  return *this;
}

/*
    Работа с экранами
*/

void EngineAttachments::SetScreen (const char* attachment_name, render::Screen& screen)
{
  impl->SetScreen (attachment_name, screen);
}

void EngineAttachments::RemoveScreen (const char* attachment_name)
{
  impl->RemoveScreen (attachment_name);
}

void EngineAttachments::RemoveAllScreens ()
{
  impl->RemoveAllScreens ();
}

render::Screen* EngineAttachments::FindScreen (const char* attachment_name) const
{
  return impl->FindScreen (attachment_name);
}

EngineAttachments::ScreenIterator EngineAttachments::CreateScreenIterator () const
{
  return impl->CreateScreenIterator ();
}

/*
    Работа со слушателями
*/

void EngineAttachments::SetListener (const char* attachment_name, scene_graph::Listener& listener)
{
  impl->SetListener (attachment_name, listener);
}

void EngineAttachments::RemoveListener (const char* attachment_name)
{
  impl->RemoveListener (attachment_name);
}

void EngineAttachments::RemoveAllListeners ()
{
  impl->RemoveAllListeners ();
}

scene_graph::Listener* EngineAttachments::FindListener (const char* attachment_name) const
{
  return impl->FindListener (attachment_name);
}

EngineAttachments::ListenerIterator EngineAttachments::CreateListenerIterator () const
{
  return impl->CreateListenerIterator ();
}

/*
    Работа с устройствами ввода
*/

void EngineAttachments::SetInputHandler (const char* attachment_name, const InputHandler& input_handler)
{
  impl->SetInputHandler (attachment_name, input_handler);
}

void EngineAttachments::RemoveInputHandler (const char* attachment_name)
{
  impl->RemoveInputHandler (attachment_name);
}

void EngineAttachments::RemoveAllInputHandlers ()
{
  impl->RemoveAllInputHandlers ();
}

const EngineAttachments::InputHandler* EngineAttachments::FindInputHandler (const char* attachment_name) const
{
  return impl->FindInputHandler (attachment_name);
}

EngineAttachments::InputHandlerIterator EngineAttachments::CreateInputHandlerIterator () const
{
  return impl->CreateInputHandlerIterator ();
}

/*
    Работа со слушателями событий
*/

void EngineAttachments::Attach (IEngineEventListener* listener)
{
  impl->Attach (listener);
}

void EngineAttachments::Detach (IEngineEventListener* listener)
{
  impl->Detach (listener);
}

/*
    Обмен
*/

void EngineAttachments::Swap (EngineAttachments& attachments)
{
  stl::swap (impl, attachments.impl);
}

namespace client
{

/*
    Обмен
*/

void swap (EngineAttachments& attachments1, EngineAttachments& attachments2)
{
  attachments1.Swap (attachments2);
}

}
