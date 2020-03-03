#import <AppKit/NSApplication.h>
#import <AppKit/NSMenu.h>
#import <Foundation/NSAutoreleasePool.h>

#include "shared.h"

using namespace syslib;
using namespace syslib::macosx;

namespace
{

const NSTimeInterval IDLE_TIMER_PERIOD = 1.f / 1000.f; //idle timer calling interval

class MacOSXApplicationDelegate;

}

@interface ApplicationDelegate : NSObject<NSApplicationDelegate>
{
  @private
    MacOSXApplicationDelegate* delegate;           //delegate
    NSTimer*                         idle_timer;         //timer for calling OnIdle

}

- (id)initWithDelegate:(MacOSXApplicationDelegate *)inDelegate;

@end

namespace
{

const char* LOG_NAME = "syslib.MacOSXApplication";

const size_t UPDATE_SYSTEM_ACTIVITY_PERIOD = 30 * 1000;   //период обновления активности системы для предотвращения запуска хранителя экрана

class MacOSXApplicationDelegate: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    MacOSXApplicationDelegate ()
      : idle_enabled (false)
      , is_exited (false)
      , listener (0)
      , application_delegate (0)
      {}

    ~MacOSXApplicationDelegate ()
    {
      NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

      NSApp.delegate = nil;

      [application_delegate release];

      [pool release];
    }

///Запуск цикла обработки сообщений
    void Run ()
    {
      static const char* METHOD_NAME = "syslib::MacOSXApplicationDelegate::Run";

      if (application_delegate)
        throw xtl::format_operation_exception (METHOD_NAME, "Application already runned");

      @try
      {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

        application_delegate = [[ApplicationDelegate alloc] initWithDelegate:this];

        //initialize application
        NSApplication* application = [NSApplication sharedApplication];

        application.delegate = application_delegate;

        [pool release];

        //run application loop
        [application run];
      }
      @catch (NSException* e)
      {
        throw xtl::format_operation_exception (METHOD_NAME, "%s, at %s", [[e reason] cStringUsingEncoding:NSASCIIStringEncoding], [[[e callStackSymbols] description] cStringUsingEncoding:NSASCIIStringEncoding]);
      }
      @catch (...)
      {
        throw xtl::format_operation_exception (METHOD_NAME, "Unknown exception caught");
      }
    }

    //Idle event handler
    void OnIdle ()
    {
      if (idle_enabled && listener)
        listener->OnIdle ();
    }

    //Initialize event handler
    void OnInitialize ()
    {
      if (listener)
        listener->OnInitialize ();
    }

    //Событие выхода из приложения
    void OnExit ()
    {
      if (listener)
        listener->OnExit (0);

      is_exited = true;
    }

    //Выход из приложения
    void Exit (int code)
    {
      [NSApp terminate:nil];
    }

///Установка необходимости вызова событий idle
    void SetIdleState (bool state)
    {
      if (idle_enabled == state)
        return;

      idle_enabled = state;
    }

///Установка слушателя событий приложения
    void SetListener (IApplicationListener* in_listener)
    {
      listener = in_listener;
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
    bool                  idle_enabled;
    bool                  is_exited;
    IApplicationListener* listener;
    ApplicationDelegate*  application_delegate;
};

///Данные приложения
class ApplicationImpl
{
  public:
///Конструктор
    ApplicationImpl ()
      : screen_saver_state (true)
      , activity_update_timer (xtl::bind (&ApplicationImpl::UpdateActivity, this), UPDATE_SYSTEM_ACTIVITY_PERIOD, TimerState_Paused)
      {}

    ///Деструктор
    ~ApplicationImpl ()
    {
      try
      {
        if (!screen_saver_state)
          SetScreenSaverState (true);
      }
      catch (...)
      {
      }
    }

///Установка состояния скрин-сейвера
    void SetScreenSaverState (bool state)
    {
      if (state == screen_saver_state)
        return;

      if (state)
      {
        activity_update_timer.Pause ();
      }
      else
      {
        activity_update_timer.Run ();
        UpdateActivity ();
      }

      screen_saver_state = state;
    }

    bool GetScreenSaverState () { return screen_saver_state; }

  private:
    void UpdateActivity ()
    {
      UpdateSystemActivity (OverallAct);
    }

  private:
    bool  screen_saver_state;     //состояние скрин-сейвера
    Timer activity_update_timer;  //таймер обновления активности приложения для предотвращения запуска скрин-сейвера
};

typedef common::Singleton<ApplicationImpl> ApplicationSingleton;

}

/*
    Создание делегата приложения
*/

IApplicationDelegate* MacOSXApplicationManager::CreateDefaultApplicationDelegate ()
{
  return new MacOSXApplicationDelegate;
}

/*
   Открытие URL во внешнем браузере
*/

void MacOSXApplicationManager::OpenUrl (const char* url)
{
  NSAutoreleasePool *pool   = [[NSAutoreleasePool alloc] init];
  NSURL             *ns_url = [NSURL URLWithString:[NSString stringWithUTF8String:url]];

  BOOL result = [[NSWorkspace sharedWorkspace] openURL:ns_url];

  [pool release];

  if (!result)
    throw xtl::format_operation_exception ("::NSWorkspace::openURL", "Can't open URL '%s'", url);
}

/*
   Управление энергосбережением
*/

void MacOSXApplicationManager::SetScreenSaverState (bool state)
{
  try
  {
    ApplicationSingleton::Instance ()->SetScreenSaverState (state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::MacOSXApplicationManager::SetScreenSaverState");
    throw;
  }
}

bool MacOSXApplicationManager::GetScreenSaverState ()
{
  return ApplicationSingleton::Instance ()->GetScreenSaverState ();
}

@implementation ApplicationDelegate

- (id)initWithDelegate:(MacOSXApplicationDelegate *)inDelegate
{
  self = [super init];

  if (!self)
    return nil;

  delegate = inDelegate;

  return self;
}

- (void)dealloc
{
  delegate = 0;

  [idle_timer invalidate];
  [idle_timer release];

  [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
  if (![NSApp setActivationPolicy:NSApplicationActivationPolicyRegular])
    common::Log (LOG_NAME).Printf ("Can't set application activation policy");

  [NSApp activateIgnoringOtherApps:YES];

  idle_timer = [[NSTimer scheduledTimerWithTimeInterval:IDLE_TIMER_PERIOD target:self selector:@selector (onIdle) userInfo:nil repeats:YES] retain];

  //create default menu items if menu is empty
  NSMenu *main_menu = NSApp.mainMenu;

  if (!main_menu)
  {
    main_menu = [[NSMenu alloc] initWithTitle:@""];

    NSApp.mainMenu = main_menu;

    [main_menu release];
  }

  if (!main_menu.numberOfItems)
  {
    NSMenuItem* app_item = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];

    [main_menu addItem:app_item];

    NSMenu* app_menu = [[NSMenu alloc] initWithTitle:@""];

    NSMenuItem* quit_item = [[NSMenuItem alloc] initWithTitle:[@"Quit " stringByAppendingString:[[NSProcessInfo processInfo] processName]] action:@selector (terminate:) keyEquivalent:@"q"];

    quit_item.target = NSApp;

    [app_menu addItem:quit_item];

    [quit_item release];

    app_item.submenu = app_menu;

    [app_item release];
    [app_menu release];
  }

  delegate->OnInitialize ();
}

- (void)onIdle
{
  delegate->OnIdle ();
}

@end
