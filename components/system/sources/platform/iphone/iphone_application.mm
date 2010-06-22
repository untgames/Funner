#include "shared.h"

#import <NSObject.h>
#import <NSAutoreleasePool.h>

#import <UIApplication.h>

using namespace syslib;
using namespace syslib::iphone;

namespace
{

const size_t IDLE_TIMER_PERIOD = 1000 / 70; //ограничение в 70 fps

class ApplicationDelegateImpl;

bool                     application_launched = false;
ApplicationDelegateImpl* application_delegate = 0;

class ApplicationDelegateImpl: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    ApplicationDelegateImpl ()
      : idle_enabled (false), listener (0)
      {}

    ~ApplicationDelegateImpl ()
    {
      if (application_delegate == this)
        application_delegate = 0;
    }

///Запуск цикла обработки сообщений
    void Run ()
    {
      if (application_delegate)
        throw xtl::format_operation_exception ("syslib::iPhonePlatform::ApplicationDelegateImpl::ApplicationDelegateImpl::Run", "Loop already runned");

      NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

      application_delegate = this;

      UIApplicationMain (0, 0, nil, @"ApplicationDelegate");

      [pool release];
    }

///Выход из приложения
    void Exit (int code)
    {
      throw xtl::format_not_supported_exception ("Application exit not supported on iphone platform");
    }

///Установка необходимости вызова событий idle
    void SetIdleState (bool state)
    {
      idle_enabled = state;
    }

///Установка слушателя событий приложения
    void SetListener (syslib::IApplicationListener* in_listener)
    {
      listener = in_listener;
    }

///События приложения
    void OnIdle ()
    {
      if (idle_enabled && listener)
        listener->OnIdle ();
    }

    void OnInitialized ()
    {
      if (listener)
        listener->OnInitialized ();
    }

    void OnExit ()
    {
      if (listener)
        listener->OnExit (0);
    }

///Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:
    bool                          idle_enabled;
    syslib::IApplicationListener* listener;
};

void idle_handler (Timer& timer)
{
  if (application_delegate)
    application_delegate->OnIdle ();
}

}

typedef stl::vector<syslib::iphone::IApplicationListener*> ListenerArray;

@interface ApplicationDelegate : NSObject
{
  @private
    ListenerArray *listeners;  //слушатели событий
    Timer         *idle_timer; //таймер вызова OnIdle
}

-(id) init;
-(void) dealloc;

-(void) attachListener:(syslib::iphone::IApplicationListener*)listener;
-(void) detachListener:(syslib::iphone::IApplicationListener*)listener;

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
  application_launched = false;

  delete idle_timer;
  delete listeners;

  [super dealloc];
}

-(void) applicationWillFinishLaunching:(UIApplication*)application
{
}

-(void) applicationDidFinishLaunching:(UIApplication*)application
{
  application_launched = true;
  
  application_delegate->OnInitialized ();

  idle_timer->Run ();  
}

-(void) applicationWillTerminate:(UIApplication*)application
{
  idle_timer->Pause ();

  application_delegate->OnExit ();
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

-(void) attachListener:(syslib::iphone::IApplicationListener*)listener
{
  listeners->push_back (listener);
}

-(void) detachListener:(syslib::iphone::IApplicationListener*)listener
{
  listeners->erase (stl::remove (listeners->begin (), listeners->end (), listener), listeners->end ());
}

@end

namespace syslib
{

bool is_in_run_loop () //запущен ли главный цикл
{
  return application_launched;
}

namespace iphone
{

/*
   Добавление/удаление подписчиков на события приложения
*/

void attach_application_listener (syslib::iphone::IApplicationListener* listener)
{
  if  (!listener)
    return;

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception ("syslib::iphone::attach_application_listener", "Can't attach application listener before entering run loop");

  [(ApplicationDelegate*)([UIApplication sharedApplication].delegate) attachListener:listener];
}

void detach_application_listener (syslib::iphone::IApplicationListener* listener)
{
  if (!is_in_run_loop ())
    return;

  [(ApplicationDelegate*)([UIApplication sharedApplication].delegate) detachListener:listener];
}

}

}

/*
    Создание делегата приложения
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return new ApplicationDelegateImpl;
}
