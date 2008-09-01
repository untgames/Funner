#include <stl/set>
#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/shared_ptr.h>
#include <xtl/signal.h>

#include <client/client.h>

using namespace client;

namespace
{

struct ScreenAttachment
{
  stl::string    attachment_name;
  render::Screen screen;

  ScreenAttachment (const char* in_attachment_name, render::Screen* in_screen)
    : attachment_name (in_attachment_name), screen (*in_screen)
    {}
};

struct ListenerAttachment
{
  stl::string                    attachment_name;
  scene_graph::Listener::Pointer listener;

  ListenerAttachment (const char* in_attachment_name, scene_graph::Listener* in_listener)
    : attachment_name (in_attachment_name), listener (in_listener)
    {}
};

typedef xtl::shared_ptr<ScreenAttachment>   ScreenAttachmentPtr;
typedef xtl::shared_ptr<ListenerAttachment> ListenerAttachmentPtr;

typedef stl::vector<ScreenAttachmentPtr>   ScreenAttachments;
typedef stl::vector<ListenerAttachmentPtr> ListenerAttachments;

}

/*
   Реализация клиента
*/

struct Client::Impl
{
  public:
    ~Impl ()
    {
      OnDestroyNotify ();
    }

///Работа с экранами
    void SetScreen (const char* attachment_name, render::Screen* screen)
    {
      static const char* METHOD_NAME = "client::Client::SetScreen";

      if (!attachment_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "attachment_name");

      if (!screen)
        throw xtl::make_null_argument_exception (METHOD_NAME, "screen");

      ScreenAttachments::iterator insert_position = FindScreenAttachment (attachment_name);

      if (insert_position != screen_attachments.end ())
        (*insert_position)->screen = *screen;
      else
        screen_attachments.push_back (ScreenAttachmentPtr (new ScreenAttachment (attachment_name, screen)));

      OnSetScreenNotify (attachment_name, screen);
    }

    void RemoveScreen (const char* attachment_name)
    {
      if (!attachment_name)
        throw xtl::make_null_argument_exception ("client::Client::RemoveScreen", "attachment_name");

      ScreenAttachments::iterator erase_position = FindScreenAttachment (attachment_name);

      if (erase_position != screen_attachments.end ())
      {
        screen_attachments.erase (erase_position);      

        OnRemoveScreenNotify (attachment_name);
      }
    }

    void RemoveAllScreens ()
    {
      for (ScreenAttachments::iterator iter = screen_attachments.begin (), end = screen_attachments.end (); iter != end; ++iter)
        OnRemoveScreenNotify ((*iter)->attachment_name.c_str ());

      screen_attachments.clear ();
    }

    render::Screen* FindScreen (const char* attachment_name)
    {
      if (!attachment_name)
        throw xtl::make_null_argument_exception ("client::Client::FindScreen", "attachment_name");

      ScreenAttachments::iterator find_position = FindScreenAttachment (attachment_name);

      if (find_position != screen_attachments.end ())
        return &(*find_position)->screen;

      return 0;
    }

    size_t ScreensCount () const
    {
      return screen_attachments.size ();
    }

    render::Screen* Screen (size_t index) const
    {
      if (index >= screen_attachments.size ())
        throw xtl::make_range_exception ("client::Client::Screen", "index", index, 0u, screen_attachments.size ());

      return &screen_attachments[index]->screen;
    }

///Работа со слушателями
    void SetListener (const char* attachment_name, scene_graph::Listener* listener)
    {
      static const char* METHOD_NAME = "client::Client::SetListener";

      if (!attachment_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "attachment_name");

      if (!listener)
        throw xtl::make_null_argument_exception (METHOD_NAME, "listener");

      ListenerAttachments::iterator insert_position = FindListenerAttachment (attachment_name);

      if (insert_position != listener_attachments.end ())
        (*insert_position)->listener = listener;
      else
        listener_attachments.push_back (ListenerAttachmentPtr (new ListenerAttachment (attachment_name, listener)));

      OnSetListenerNotify (attachment_name, listener);
    }

    void RemoveListener (const char* attachment_name)
    {
      if (!attachment_name)
        throw xtl::make_null_argument_exception ("client::Client::RemoveListener", "attachment_name");

      ListenerAttachments::iterator erase_position = FindListenerAttachment (attachment_name);

      if (erase_position != listener_attachments.end ())
      {
        listener_attachments.erase (erase_position);

        OnRemoveListenerNotify (attachment_name);
      }
    }

    void RemoveAllListeners ()
    {
      for (ListenerAttachments::iterator iter = listener_attachments.begin (), end = listener_attachments.end (); iter != end; ++iter)
        OnRemoveListenerNotify ((*iter)->attachment_name.c_str ());

      listener_attachments.clear ();
    }

    scene_graph::Listener* FindListener (const char* attachment_name)
    {
      if (!attachment_name)
        throw xtl::make_null_argument_exception ("client::Client::FindListener", "attachment_name");

      ListenerAttachments::iterator find_position = FindListenerAttachment (attachment_name);

      if (find_position != listener_attachments.end ())
        return (*find_position)->listener.get ();

      return 0;
    }

    size_t ListenersCount () const
    {
      return listener_attachments.size ();
    }

    scene_graph::Listener* Listener (size_t index) const
    {
      if (index >= listener_attachments.size ())
        throw xtl::make_range_exception ("client::Client::Listener", "index", index, 0u, listener_attachments.size ());

      return listener_attachments[index]->listener.get ();
    }

///Работа с устройствами ввода
    xtl::connection RegisterInputHandler (const InputEventHandler& input_handler)
    {
      return input_signal.connect (input_handler);
    }

    void SetInputTranslator (const char* attachment_name, const char* translation_table)
    {
      static const char* METHOD_NAME = "client::Client::SetInputTranslator";

      if (!attachment_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "attachment_name");

      if (!translation_table)
        throw xtl::make_null_argument_exception (METHOD_NAME, "translation_table");

      //???????????
    }

    void RemoveInputTranslator (const char* attachment_name)
    {
      if (!attachment_name)
        throw xtl::make_null_argument_exception ("client::Client::RemoveInputTranslator", "attachment_name");

      //???????????
    }

    void RemoveAllInputTranslators ()
    {
      //???????????
    }

    void ProcessInputEvent (const char* attachment_name, const char* event) const
    {
      static const char* METHOD_NAME = "client::Client::ProcessInputEvent";

      if (!attachment_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "attachment_name");

      if (!event)
        throw xtl::make_null_argument_exception (METHOD_NAME, "event");

      //???????????
    }

///Работа со слушателями событий
    void AttachEventListener (IClientEventListener* listener)
    {
      static const char* METHOD_NAME = "client::Client::AttachEventListener";

      if (!listener)
        throw xtl::make_null_argument_exception (METHOD_NAME, "listener");

      ListenerSet::iterator iter = listeners.find (listener);

      if (iter != listeners.end ())
        throw xtl::format_operation_exception (METHOD_NAME, "Listener already attached");

      listeners.insert (listener);
    }

    void DetachEventListener (IClientEventListener* listener)
    {
      if (!listener)
        throw xtl::make_null_argument_exception ("client::Client::DetachEventListener", "listener");

      listeners.erase (listener);
    }

  private:
///Поиск экрана
    ScreenAttachments::iterator FindScreenAttachment (const char* attachment_name)
    {     
      for (ScreenAttachments::iterator iter = screen_attachments.begin (), end = screen_attachments.end (); iter != end; ++iter)
        if (!xtl::xstrcmp ((*iter)->attachment_name.c_str (), attachment_name))
          return iter;

      return screen_attachments.end ();
    }

///Поиск слушателя
    ListenerAttachments::iterator FindListenerAttachment (const char* attachment_name)
    {     
      for (ListenerAttachments::iterator iter = listener_attachments.begin (), end = listener_attachments.end (); iter != end; ++iter)
        if (!xtl::xstrcmp ((*iter)->attachment_name.c_str (), attachment_name))
          return iter;

      return listener_attachments.end ();
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
    typedef xtl::signal<void (const char*, const char*)> InputSignal;
    typedef stl::set<IClientEventListener*>              ListenerSet;

  private:
    ScreenAttachments   screen_attachments;
    ListenerAttachments listener_attachments;
    InputSignal         input_signal;
    ListenerSet         listeners;
};

/*
   Клиент
*/

/*
   Конструктор/деструктор
*/

Client::Client ()
  : impl (new Impl)
{
}

Client::~Client ()
{
}

/*
   Работа с экранами
*/

void Client::SetScreen (const char* attachment_name, render::Screen* screen)
{
  impl->SetScreen (attachment_name, screen);
}

void Client::RemoveScreen (const char* attachment_name)
{
  impl->RemoveScreen (attachment_name);
}

void Client::RemoveAllScreens ()
{
  impl->RemoveAllScreens ();
}

render::Screen* Client::FindScreen (const char* attachment_name) const
{
  return impl->FindScreen (attachment_name);
}

size_t Client::ScreensCount () const
{
  return impl->ScreensCount ();
}

render::Screen* Client::Screen (size_t index) const
{
  return impl->Screen (index);
}

/*
   Работа со слушателями
*/

void Client::SetListener (const char* attachment_name, scene_graph::Listener* listener)
{
  impl->SetListener (attachment_name, listener);
}

void Client::RemoveListener (const char* attachment_name)
{
  impl->RemoveListener (attachment_name);
}

void Client::RemoveAllListeners ()
{
  impl->RemoveAllListeners ();
}

scene_graph::Listener* Client::FindListener (const char* attachment_name) const
{
  return impl->FindListener (attachment_name);
}

size_t Client::ListenersCount () const
{
  return impl->ListenersCount ();
}

scene_graph::Listener* Client::Listener (size_t index) const
{
  return impl->Listener (index);
}

/*
   Работа с устройствами ввода
*/

xtl::connection Client::RegisterInputHandler (const InputEventHandler& input_handler)
{
  return impl->RegisterInputHandler (input_handler);
}

void Client::SetInputTranslator (const char* attachment_name, const char* translation_table)
{
  impl->SetInputTranslator (attachment_name, translation_table);
}

void Client::RemoveInputTranslator (const char* attachment_name)
{
  impl->RemoveInputTranslator (attachment_name);
}

void Client::RemoveAllInputTranslators ()
{
  impl->RemoveAllInputTranslators ();
}

void Client::ProcessInputEvent (const char* attachment_name, const char* event) const
{
  impl->ProcessInputEvent (attachment_name, event);
}

/*
   Работа со слушателями событий
*/

void Client::AttachEventListener (IClientEventListener* listener)
{
  impl->AttachEventListener (listener);
}

void Client::DetachEventListener (IClientEventListener* listener)
{
  impl->DetachEventListener (listener);
}
