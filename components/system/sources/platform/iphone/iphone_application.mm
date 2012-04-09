#include "shared.h"

#import <Foundation/NSAutoreleasePool.h>

#import <UIKit/UIApplication.h>

#import <QuartzCore/CADisplayLink.h>

using namespace syslib;
using namespace syslib::iphone;

namespace
{

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
      static const char* METHOD_NAME = "syslib::iPhonePlatform::ApplicationDelegateImpl::ApplicationDelegateImpl::Run";

      if (application_delegate)
        throw xtl::format_operation_exception (METHOD_NAME, "Loop already runned");

      @try
      {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

        application_delegate = this;

        UIApplicationMain (0, 0, nil, @"ApplicationDelegate");

        [pool release];
      }
      @catch (NSException* e)
      {
        throw xtl::format_operation_exception (METHOD_NAME, "%s", [[e reason] cStringUsingEncoding:NSASCIIStringEncoding]);
      }
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

    void OnInitialize ()
    {
      if (listener)
        listener->OnInitialize ();
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

}

typedef stl::vector<syslib::iphone::IApplicationListener*> ListenerArray;

@interface ApplicationDelegateInternal : NSObject
{
  @private
    ListenerArray *listeners;        //слушатели событий
    CADisplayLink *idle_timer;       //таймер вызова OnIdle
    bool          main_view_visible; //виден ли главный view приложения
}

@property (nonatomic, readonly) ListenerArray* listeners;
@property (nonatomic, readonly) CADisplayLink* idle_timer;
@property (nonatomic)           bool           main_view_visible;

@end

@implementation ApplicationDelegateInternal

@synthesize listeners, idle_timer, main_view_visible;

-(void)initIdleTimer
{
  [idle_timer invalidate];
  [idle_timer release];

  idle_timer = [[CADisplayLink displayLinkWithTarget:self selector:@selector (onIdle:)] retain];

  [idle_timer addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
}

-(id) init
{
  self = [super init];

  if (!self)
    return nil;

  try
  {
    listeners = new ListenerArray ();

    [self initIdleTimer];

    idle_timer.paused = YES;
  }
  catch (...)
  {
    [idle_timer invalidate];
    [idle_timer release];
    delete listeners;

    [self release];
    return nil;
  }

  return self;
}

-(void) dealloc
{
  application_launched = false;

  [idle_timer invalidate];
  [idle_timer release];

  delete listeners;

  [super dealloc];
}

-(void) onIdle:(CADisplayLink*)sender
{
  if (application_delegate && main_view_visible)
    application_delegate->OnIdle ();
}

@end

@implementation ApplicationDelegate

-(id) init
{
  self = [super init];

  if (!self)
    return nil;

  impl = [[ApplicationDelegateInternal alloc] init];

  if (!impl)
  {
    [self release];
    return nil;
  }

  return self;
}

-(void) dealloc
{
  [impl release];

  [super dealloc];
}

-(BOOL) application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
  application_launched = true;
  
  application_delegate->OnInitialize ();

  impl.idle_timer.paused = NO;

  return YES;
}

-(void) applicationWillTerminate:(UIApplication*)application
{
  [impl.idle_timer invalidate];
  impl.idle_timer.paused = YES;

  application_delegate->OnExit ();
}

-(BOOL) application:(UIApplication*)application openURL:(NSURL*)url sourceApplication:(NSString*)sourceApplication annotation:(id)annotation
{
  stl::string notification = common::format ("ApplicationOpenURL %s", [[url absoluteString] UTF8String]);

  syslib::Application::PostNotification (notification.c_str ());

  return YES;
}

-(void) applicationDidReceiveMemoryWarning:(UIApplication*)application
{
  for (ListenerArray::iterator iter = impl.listeners->begin (), end = impl.listeners->end (); iter != end; ++iter)
    (*iter)->OnMemoryWarning ();
}

-(void) applicationWillResignActive:(UIApplication*)application
{
  for (ListenerArray::iterator iter = impl.listeners->begin (), end = impl.listeners->end (); iter != end; ++iter)
    (*iter)->OnInactive ();

  impl.idle_timer.paused = YES;
  [impl.idle_timer invalidate];
}

-(void) applicationDidBecomeActive:(UIApplication*)application
{
  for (ListenerArray::iterator iter = impl.listeners->begin (), end = impl.listeners->end (); iter != end; ++iter)
    (*iter)->OnActive ();

  [impl initIdleTimer];
}

/*
   Добавление/удаление подписчиков
*/

-(void) attachListener:(syslib::iphone::IApplicationListener*)listener
{
  impl.listeners->push_back (listener);
}

-(void) detachListener:(syslib::iphone::IApplicationListener*)listener
{
  impl.listeners->erase (stl::remove (impl.listeners->begin (), impl.listeners->end (), listener), impl.listeners->end ());
}

-(void)setMainViewVisible:(bool)state
{
  impl.main_view_visible = state;
}

-(bool)isMainViewVisible
{
  return impl.main_view_visible;
}

@end

namespace syslib
{

bool is_in_run_loop () //запущен ли главный цикл
{
  return application_launched;
}

}

/*
   Менеджер приложения
*/

/*
   Добавление/удаление подписчиков на события приложения
*/

void ApplicationManager::AttachApplicationListener (syslib::iphone::IApplicationListener* listener)
{
  if  (!listener)
    return;

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception ("syslib::iphone::attach_application_listener", "Can't attach application listener before entering run loop");

  [(ApplicationDelegate*)([UIApplication sharedApplication].delegate) attachListener:listener];
}

void ApplicationManager::DetachApplicationListener (syslib::iphone::IApplicationListener* listener)
{
  if (!is_in_run_loop ())
    return;

  [(ApplicationDelegate*)([UIApplication sharedApplication].delegate) detachListener:listener];
}

/*
    Создание делегата приложения
*/

IApplicationDelegate* IPhoneApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new ApplicationDelegateImpl;
}

/*
   Открытие URL во внешнем браузере
*/

void IPhoneApplicationManager::OpenUrl (const char* url)
{
  NSAutoreleasePool *pool          = [[NSAutoreleasePool alloc] init];
  NSString          *ns_url_string = [NSString stringWithUTF8String:url];
  NSURL             *ns_url        = [NSURL URLWithString:[ns_url_string stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];

  BOOL result = [[UIApplication sharedApplication] openURL:ns_url];

  [pool release];

  if (!result)
    throw xtl::format_operation_exception ("::UIApplication::openURL", "Can't open URL '%s'", url);
}
