#include "shared.h"

#import <NSObject.h>
#import <NSAutoreleasePool.h>

#import <UIAccelerometer.h>
#import <UIApplication.h>

using namespace syslib;
using namespace syslib::iphone;

namespace
{

IRunLoopContext* run_loop_context     = 0;
bool             application_launched = false;

}

namespace syslib
{

bool is_in_run_loop () //запущен ли главный цикл
{
  return application_launched;
}

}

typedef stl::vector<IApplicationListener*> ListenerArray;

@interface ApplicationDelegate : NSObject <UIAccelerometerDelegate>
{
  @private
    ListenerArray *listeners;  //слушатели событий
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
    try
    {
      listeners = new ListenerArray ();
    }
    catch (...)
    {
      [self release];
      return nil;
    }
  }

  return self;
}

-(void) dealloc
{
  delete listeners;

  [super dealloc];
}

-(void) applicationDidFinishLaunching:(UIApplication*)application
{
  application_launched = true;

  [UIAccelerometer sharedAccelerometer].delegate = self;

  run_loop_context->OnEnterRunLoop ();
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

-(void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnAcceleration (acceleration.x, acceleration.y, acceleration.z);
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

/*
   Установка параметров устройства
*/

void set_accelerometer_update_interval (double seconds)
{
  static const char* METHOD_NAME = "syslib::iphone::set_accelerometer_update_interval";

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set accelerometer update interval before entering run loop");

  if (seconds < 0)
    throw xtl::make_argument_exception (METHOD_NAME, "seconds", seconds, "Interval must be larger than 0");

  [UIAccelerometer sharedAccelerometer].updateInterval = seconds;
}

}

}
