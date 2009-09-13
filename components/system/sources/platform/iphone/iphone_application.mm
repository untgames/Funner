#include "shared.h"

#import <NSObject.h>
#import <NSAutoreleasePool.h>

#import <UIApplication.h>

using namespace syslib;

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

@interface ApplicationDelegate : NSObject
{
}

@end

@implementation ApplicationDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
  application_launched = true;

  run_loop_context->OnEnterRunLoop ();
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
