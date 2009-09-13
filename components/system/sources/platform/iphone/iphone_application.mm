#include "shared.h"

#import <NSObject.h>
#import <NSAutoreleasePool.h>

#import <UIApplication.h>

using namespace syslib;

namespace
{

bool application_launched = false;

//генерация исключения с кодом ошибки
void pthread_raise_error (const char* source, int status)
{
  throw xtl::format_operation_exception (source, "Operation failed. Reason: %s (function exit with code %d)", common::strerror (status), status);
}

}

@interface ApplicationDelegate : NSObject
{
}

- (void)applicationDidFinishLaunching:(UIApplication *)application;

@end

@implementation ApplicationDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
  application_launched = true;
}

@end

@interface MainApplicationStarter : NSObject
{
}

-(void) StartApplication: (id) arg;

@end

@implementation MainApplicationStarter

-(void) StartApplication: (id) arg
{
  NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

  UIApplicationMain (0, 0, nil, @"ApplicationDelegate");

  [pool release];
}

@end

namespace
{

/*
   Класс обеспечивающий работу приложения для iPhone
*/

class IPhoneApplicationImpl
{
  public:
//Конструктор / деструктор
    IPhoneApplicationImpl ()
      : window (0)
    {
      main_application_starter = [[MainApplicationStarter alloc] init];

      [NSThread detachNewThreadSelector:@selector (StartApplication:) toTarget:main_application_starter withObject:nil];

      while (!application_launched)
        Platform::Sleep (10);
    }

    ~IPhoneApplicationImpl ()
    {
      [main_application_starter release];
    }

///Получение/установка окна приложения
    WindowImpl* GetWindow ()
    {
      return window;
    }

    void SetWindow (WindowImpl* new_window)
    {
      if (window == new_window)
        return;

      window = new_window;

      while (!message_queue.empty ())
        message_queue.pop ();
    }

///Работа с очередью сообщений
    bool IsMessageQueueEmpty ()
    {
      return message_queue.empty ();
    }

    void PushMessage (const IPhoneMessage& message)
    {
      message_queue.push (message);
    }

    void GetOldestMessage (IPhoneMessage& target)
    {
      target = message_queue.front ();
    }

    void PopOldestMessage ()
    {
      message_queue.pop ();
    }

///Получение события нити
    Condition& GetEventThreadCondition ()
    {
      return event_thread_condition;
    }

  private:
    typedef stl::queue <IPhoneMessage> MessageQueue;

  private:
    WindowImpl*             window;
    MessageQueue            message_queue;
    MainApplicationStarter* main_application_starter;
    Condition               event_thread_condition;
};

typedef common::Singleton <IPhoneApplicationImpl> IPhoneApplicationSingleton;

}

/*
   Получение/установка окна приложения
*/

WindowImpl* IPhoneApplication::GetWindow ()
{
  return IPhoneApplicationSingleton::Instance ()->GetWindow ();
}

void IPhoneApplication::SetWindow (WindowImpl* new_window)
{
  IPhoneApplicationSingleton::Instance ()->SetWindow (new_window);
}

/*
   Работа с очередью сообщений
*/

bool IPhoneApplication::IsMessageQueueEmpty ()
{
  return IPhoneApplicationSingleton::Instance ()->IsMessageQueueEmpty ();
}

void IPhoneApplication::PushMessage (const IPhoneMessage& message)
{
  IPhoneApplicationSingleton::Instance ()->PushMessage (message);
}

void IPhoneApplication::GetOldestMessage (IPhoneMessage& target)
{
  IPhoneApplicationSingleton::Instance ()->GetOldestMessage (target);
}

void IPhoneApplication::PopOldestMessage ()
{
  IPhoneApplicationSingleton::Instance ()->PopOldestMessage ();
}

/*
   Получение события нити
*/

Condition& IPhoneApplication::GetEventThreadCondition ()
{
  return IPhoneApplicationSingleton::Instance ()->GetEventThreadCondition ();
}

/*
    Работа с очередью сообщений
*/

bool Platform::IsMessageQueueEmpty ()
{
  return IPhoneApplication::IsMessageQueueEmpty ();
}

void Platform::DoNextEvent ()
{
  IPhoneApplicationSingleton::Instance instance;

  WindowImpl* window = instance->GetWindow ();

  if (instance->IsMessageQueueEmpty () || !window)
    return;

  IPhoneMessage next_message;

  instance->GetOldestMessage (next_message);
  instance->PopOldestMessage ();

  window->Notify (next_message.event, next_message.event_context);
}

void Platform::WaitMessage ()
{
  if (!IPhoneApplication::IsMessageQueueEmpty ())
    return;

  //!!!!!!!!!!!установка ожидания сообщения!!!!!!!!!!
}

void Platform::UpdateMessageQueue ()
{
}
