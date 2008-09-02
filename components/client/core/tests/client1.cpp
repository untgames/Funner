#include <cstdio>

#include <xtl/connection.h>
#include <xtl/exception.h>
#include <xtl/function.h>

#include <sg/listener.h>

#include <render/screen.h>

#include <client/client.h>

const char* TRANSLATION_TABLE1_FILE_NAME = "data/translation_table1.keymap";
const char* TRANSLATION_TABLE2_FILE_NAME = "data/translation_table2.keymap";

using namespace client;

class ClientEventListener : public IClientEventListener
{
  public:
    ClientEventListener () {}
    ~ClientEventListener () {}

///События установки/удаления экрана
    void OnSetScreen (const char* attachment_name, render::Screen* screen) 
    {
      printf ("OnSetScreen: attachment_name is '%s', screen name is '%s'\n", attachment_name, screen->Name ());
    }

    void OnRemoveScreen (const char* attachment_name) 
    {
      printf ("OnRemoveScreen: attachment_name is '%s'\n", attachment_name);
    }

///События установки/удаления слушателя
    void OnSetListener (const char* attachment_name, scene_graph::Listener* listener) 
    {
      printf ("OnSetListener: attachment_name is '%s', listener name is '%s'\n", attachment_name, listener->Name ());
    }

    void OnRemoveListener (const char* attachment_name) 
    {
      printf ("OnRemoveListener: attachment_name is '%s'\n", attachment_name);
    }

///Событие удаления клиента
    void OnDestroy () 
    {
      printf ("OnDestroy\n");
    }
};

void input_event_handler (const char* attachment_name, const char* event)
{
  printf ("New input event from attachment '%s': '%s'\n", attachment_name, event);
}

int main ()
{
  ClientEventListener client_event_listener;
  Client client;

  client.AttachEventListener (&client_event_listener);

  printf ("Client screens count is %u\n", client.ScreensCount ());
  printf ("Client listeners count is %u\n", client.ListenersCount ());

  scene_graph::Listener::Pointer listener1 = scene_graph::Listener::Create (), listener2 = scene_graph::Listener::Create ();

  listener1->SetName ("Listener1");
  listener2->SetName ("Listener2");

  render::Screen screen1, screen2;

  screen1.SetName ("Screen1");
  screen2.SetName ("Screen2");

  client.SetScreen ("ScreenAttachment1", &screen1);
  client.SetScreen ("ScreenAttachment1", &screen2);

  client.SetListener ("ListenerAttachment1", listener1.get ());
  client.SetListener ("ListenerAttachment1", listener2.get ());

  printf ("Client screens count is %u\n", client.ScreensCount ());
  printf ("Client listeners count is %u\n", client.ListenersCount ());

  client.RemoveScreen ("ScreenAttachment1");
  client.RemoveListener ("ListenerAttachment1");

  printf ("Client screens count is %u\n", client.ScreensCount ());
  printf ("Client listeners count is %u\n", client.ListenersCount ());

  client.SetScreen ("ScreenAttachment2", &screen2);
  client.SetScreen ("ScreenAttachment1", &screen1);

  client.SetListener ("ListenerAttachment2", listener2.get ());
  client.SetListener ("ListenerAttachment1", listener1.get ());

  printf ("Listener 1 name is '%s'\n", client.Listener (1)->Name ());
  printf ("Screen 0 name is '%s'\n", client.Screen (0)->Name ());

  printf ("Listener 'ListenerAttachment2' name is '%s'\n", client.FindListener ("ListenerAttachment2")->Name ());
  printf ("Screen 'ScreenAttachment1' name is '%s'\n", client.FindScreen ("ScreenAttachment1")->Name ());

  printf ("Listener 'asd' is %p\n", client.FindListener ("asd"));
  printf ("Screen 'asd' is %p\n", client.FindScreen ("asd"));

  printf ("Client screens count is %u\n", client.ScreensCount ());
  printf ("Client listeners count is %u\n", client.ListenersCount ());

  client.DetachEventListener (&client_event_listener);
  
  client.RemoveAllListeners ();
  
  client.AttachEventListener (&client_event_listener);
  
  client.RemoveAllScreens ();

  printf ("Client screens count is %u\n", client.ScreensCount ());
  printf ("Client listeners count is %u\n", client.ListenersCount ());

  try
  {
    client.SetInputTranslator ("InputTranslatorAttachment1", TRANSLATION_TABLE1_FILE_NAME);
    client.SetInputTranslator ("InputTranslatorAttachment2", TRANSLATION_TABLE2_FILE_NAME);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception: %s\n", e.what ());
    return 1;
  }

  client.RegisterInputHandler (&input_event_handler);

  client.ProcessInputEvent ("InputTranslatorAttachment1", "event1");
  client.ProcessInputEvent ("InputTranslatorAttachment2", "event1");
  client.ProcessInputEvent ("InputTranslatorAttachment2", "event2 param1");

  client.RemoveInputTranslator ("InputTranslatorAttachment2");

  client.ProcessInputEvent ("InputTranslatorAttachment1", "event1");
  client.ProcessInputEvent ("InputTranslatorAttachment2", "event2");

  client.RemoveAllInputTranslators ();

  client.ProcessInputEvent ("InputTranslatorAttachment1", "event1");
  client.ProcessInputEvent ("InputTranslatorAttachment2", "event2");

  return 0;
}
