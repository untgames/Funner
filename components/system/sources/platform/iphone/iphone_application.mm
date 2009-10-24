#include "shared.h"

#import <NSObject.h>
#import <NSAutoreleasePool.h>

#import <UIApplication.h>

using namespace syslib;
using namespace syslib::iphone;

namespace
{

const size_t IDLE_TIMER_PERIOD = 1000 / 70; //ограничение в 70 fps

IRunLoopContext* run_loop_context     = 0;
bool             application_launched = false;

void idle_handler (Timer& timer)
{
  if (run_loop_context)
    run_loop_context->OnIdle ();
}

}

namespace syslib
{

bool is_in_run_loop () //запущен ли главный цикл
{
  return application_launched;
}

}

typedef stl::vector<IApplicationListener*> ListenerArray;

@interface ApplicationDelegate : NSObject
{
  @private
    ListenerArray *listeners;  //слушатели событий
    Timer         *idle_timer; //таймер вызова OnIdle
}

-(id) init;
-(void) dealloc;

-(void) attachListener:(IApplicationListener*)listener;
-(void) detachListener:(IApplicationListener*)listener;

@end

@implementation ApplicationDelegate

-(id) init
{
  self = [super init];

  if (self)
  {
    listeners  = 0;
    idle_timer = 0;

    try
    {
      listeners  = new ListenerArray ();
      idle_timer = new Timer (&idle_handler, IDLE_TIMER_PERIOD, TimerState_Paused);
    }
    catch (...)
    {
      delete idle_timer;
      delete listeners;

      [self release];
      return nil;
    }
  }

  return self;
}

-(void) dealloc
{
  run_loop_context     = 0;
  application_launched = false;

  delete idle_timer;
  delete listeners;

  [super dealloc];
}

-(void) applicationWillFinishLaunching:(UIApplication*)application
{
//  run_loop_context->OnEnterRunLoop ();  
}

-(void) applicationDidFinishLaunching:(UIApplication*)application
{
  application_launched = true;
  
  run_loop_context->OnEnterRunLoop ();  

  idle_timer->Run ();  
}

-(void) applicationWillTerminate:(UIApplication*)application
{
  idle_timer->Pause ();

  run_loop_context->OnExit (0);
}

-(void) applicationDidReceiveMemoryWarning:(UIApplication*)application
{
  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnMemoryWarning ();
}

-(void) applicationWillResignActive:(UIApplication*)application
{
  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnInactive ();
}

-(void) applicationDidBecomeActive:(UIApplication*)application
{
  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnActive ();
}

/*
   Добавление/удаление подписчиков
*/

-(void) attachListener:(IApplicationListener*)listener
{
  listeners->push_back (listener);
}

-(void) detachListener:(IApplicationListener*)listener
{
  listeners->erase (stl::remove (listeners->begin (), listeners->end (), listener), listeners->end ());
}

@end

/*
    Работа с очередью сообщений
*/

bool Platform::IsMessageQueueEmpty ()
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::IsMessageQueueEmpty", "Message queue not supported");
}

void Platform::DoNextEvent ()
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::DoNextEvent", "Message queue not supported");
}

void Platform::WaitMessage ()
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::WaitMessage", "Message queue not supported");
}

void Platform::UpdateMessageQueue ()
{
}

/*
    Запуск приложения
*/

void Platform::RunLoop (IRunLoopContext* context)
{
  if (!context)
    throw xtl::make_null_argument_exception ("syslib::iPhonePlatform::RunLoop", "context");

  if (run_loop_context)
    throw xtl::format_operation_exception ("syslib::iPhonePlatform::RunLoop", "Loop already runned");

  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  run_loop_context = context;

  UIApplicationMain (0, 0, nil, @"ApplicationDelegate");

  [pool release];
}

namespace syslib
{

namespace iphone
{

/*
   Добавление/удаление подписчиков на события приложения
*/

void attach_application_listener (IApplicationListener* listener)
{
  if  (!listener)
    return;

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception ("syslib::iphone::attach_application_listener", "Can't attach application listener before entering run loop");

  [(ApplicationDelegate*)([UIApplication sharedApplication].delegate) attachListener:listener];
}

void detach_application_listener (IApplicationListener* listener)
{
  if (!is_in_run_loop ())
    return;

  [(ApplicationDelegate*)([UIApplication sharedApplication].delegate) detachListener:listener];
}

}

}
