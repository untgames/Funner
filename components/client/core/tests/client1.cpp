#include <cstdio>

#include <sg/listener.h>

#include <render/screen.h>

#include <client/client.h>

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

  return 0;
}