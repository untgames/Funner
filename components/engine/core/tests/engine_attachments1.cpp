#include <cstdio>

#include <xtl/connection.h>
#include <xtl/exception.h>
#include <xtl/function.h>
#include <xtl/iterator.h>

#include <sg/listener.h>

#include <render/screen.h>

#include <client/engine_attachments.h>

using namespace client;

class ClientEventListener : public IEngineEventListener
{
  public:
///События установки/удаления экрана
    void OnSetScreen (const char* attachment_name, render::Screen& screen) 
    {
      printf ("OnSetScreen: attachment_name is '%s', screen name is '%s'\n", attachment_name, screen.Name ());
    }

    void OnRemoveScreen (const char* attachment_name) 
    {
      printf ("OnRemoveScreen: attachment_name is '%s'\n", attachment_name);
    }

///События установки/удаления слушателя
    void OnSetListener (const char* attachment_name, scene_graph::Listener& listener) 
    {
      printf ("OnSetListener: attachment_name is '%s', listener name is '%s'\n", attachment_name, listener.Name ());
    }

    void OnRemoveListener (const char* attachment_name) 
    {
      printf ("OnRemoveListener: attachment_name is '%s'\n", attachment_name);
    }

///События установки/удаления обработчика событий ввода
    void OnSetInputHandler (const char* attachment_name, const InputHandler&)
    {
      printf ("OnSetInputHandler: attachment_name is '%s'\n", attachment_name);
    }
    
    void OnRemoveInputHandler (const char* attachment_name)
    {
      printf ("OnRemoveInputHandler: attachment_name is '%s'\n", attachment_name);
    }    

///Событие удаления клиента
    void OnDestroy () 
    {
      printf ("OnDestroy\n");
    }
};

void input_event_handler (const char*)
{
}

void dump (const EngineAttachments& client)
{
  printf ("screens={");
  
  bool need_separator = false;
  
  for (EngineAttachments::ScreenIterator iter=client.CreateScreenIterator (); iter; ++iter, need_separator=true)
    printf ("%s%s::%s", need_separator ? ", " : "", iter->Name (), iter->Value ().Name ());
    
  printf ("} listeners={");
  
  need_separator = false;
  
  for (EngineAttachments::ListenerIterator iter=client.CreateListenerIterator (); iter; ++iter, need_separator=true)
    printf ("%s%s::%s", need_separator ? ", " : "", iter->Name (), iter->Value ().Name ());
    
  printf ("} input_handlers={");
  
  need_separator = false;
  
  for (EngineAttachments::InputHandlerIterator iter=client.CreateInputHandlerIterator (); iter; ++iter, need_separator=true)
    printf ("%s%s", need_separator ? ", " : "", iter->Name ());
    
  printf ("}\n");
}

int main ()
{
  printf ("Results of engine_attachments1_test:\n");

  ClientEventListener client_event_listener;
  EngineAttachments client;

  client.Attach (&client_event_listener);

  dump (client);

  scene_graph::Listener::Pointer listener1 = scene_graph::Listener::Create (), listener2 = scene_graph::Listener::Create ();

  listener1->SetName ("Listener1");
  listener2->SetName ("Listener2");

  render::Screen screen1, screen2;

  screen1.SetName ("Screen1");
  screen2.SetName ("Screen2");

  client.SetScreen ("ScreenAttachment1", screen1);
  client.SetScreen ("ScreenAttachment1", screen2);

  client.SetListener ("ListenerAttachment1", *listener1);
  client.SetListener ("ListenerAttachment1", *listener2);

  dump (client);

  client.RemoveScreen ("ScreenAttachment1");
  client.RemoveListener ("ListenerAttachment1");

  dump (client);

  client.SetScreen ("ScreenAttachment2", screen2);
  client.SetScreen ("ScreenAttachment1", screen1);

  client.SetListener ("ListenerAttachment2", *listener2);
  client.SetListener ("ListenerAttachment1", *listener1);
  
  dump (client);  

  printf ("Listener 'ListenerAttachment2' name is '%s'\n", client.FindListener ("ListenerAttachment2")->Name ());
  printf ("Screen 'ScreenAttachment1' name is '%s'\n", client.FindScreen ("ScreenAttachment1")->Name ());

  printf ("Listener 'asd' is %p\n", client.FindListener ("asd"));
  printf ("Screen 'asd' is %p\n", client.FindScreen ("asd"));

  dump (client);  

  client.Detach (&client_event_listener);
  
  client.RemoveAllListeners ();
  
  client.Attach (&client_event_listener);
  
  client.RemoveAllScreens ();

  dump (client);

  client.SetInputHandler ("InputTranslatorAttachment1", &input_event_handler);
  client.SetInputHandler ("InputTranslatorAttachment2", &input_event_handler);
  client.SetInputHandler ("InputTranslatorAttachment3", &input_event_handler);

  dump (client);

  client.RemoveInputHandler ("InputTranslatorAttachment2");

  dump (client);

  client.RemoveAllInputHandlers ();

  dump (client);

  return 0;
}
