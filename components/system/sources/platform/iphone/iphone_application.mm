#include "shared.h"

#include <netdb.h>

#include <sys/sysctl.h>

#import <Foundation/NSAutoreleasePool.h>

#import <UIKit/UIApplication.h>

#import <QuartzCore/CADisplayLink.h>

#import <SystemConfiguration/SCNetworkReachability.h>

#import <AudioToolbox/AudioServices.h>

using namespace syslib;
using namespace syslib::iphone;

namespace
{

class ApplicationDelegateImpl;

bool                     application_launched = false;
ApplicationDelegateImpl* application_delegate = 0;

NSString*            USER_DEFAULTS_UUID                     = @"UUID";
const char*          LOG_NAME                               = "syslib.IPhoneApplication";
const char*          REGISTER_FOR_PUSH_NOTIFICATIONS_PREFIX = "RegisterForPushNotification Register ";
const NSTimeInterval BACKGROUND_IDLE_TIMER_PERIOD           = 1.f / 60.f; //idle timer calling interval when app is in background state

//Functions for printing objective-c objects to json
void ns_object_to_json (id obj, stl::string& output);

void ns_array_to_json (NSArray* arr, stl::string& output)
{
  output += "[";

  for (size_t i = 0, count = [arr count]; i < count; i++)
  {
    if (i)
      output += ",";

    ns_object_to_json ([arr objectAtIndex:i], output);
  }

  output += "]";
}

void ns_dictionary_to_json (NSDictionary* dic, stl::string& output)
{
  output += "{";

  __block bool first_item_processed = false;

  [dic enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop)
    {
      if (![key isKindOfClass:[NSString class]])
        return;

      if (first_item_processed)
        output += ",";

      output += common::format ("\"%s\":", [(NSString*)key UTF8String]);

      ns_object_to_json (obj, output);

      first_item_processed = true;
    }
  ];

  output += "}";
}

void ns_object_to_json (id obj, stl::string& output)
{
  if ([obj isKindOfClass:[NSString class]])
    output += common::format ("\"%s\"", [(NSString*)obj UTF8String]);
  else if ([obj isKindOfClass:[NSNumber class]])
    output += common::format ("%s", [[(NSNumber*)obj stringValue] UTF8String]);
  else if ([obj isKindOfClass:[NSDictionary class]])
    ns_dictionary_to_json (obj, output);
  else if ([obj isKindOfClass:[NSArray class]])
    ns_array_to_json (obj, output);
  else
    output += "\"\"";
}

//получение имени ошибки Audio Services
const char* get_audio_session_error_name (OSStatus error)
{
  switch (error)
  {
    case kAudioSessionNoError:                  return "No error has occurred.";
    case kAudioSessionNotInitialized:           return "An Audio Session Services function was called without first initializing the session.";
    case kAudioSessionAlreadyInitialized:       return "The AudioSessionInitialize function was called more than once during the lifetime of your application.";
    case kAudioSessionInitializationError:      return "There was an error during audio session initialization.";
    case kAudioSessionUnsupportedPropertyError: return "The audio session property is not supported.";
    case kAudioSessionBadPropertySizeError:     return "The size of the audio session property data was not correct.";
    case kAudioSessionNotActiveError:           return "The operation failed because your application's audio session was not active.";
    case kAudioServicesNoHardwareError:         return "The audio operation failed because the device has no audio input available.";
    case kAudioSessionNoCategorySet:            return "The audio operation failed because it requires the audio session to have an explicitly-set category, but none was set. To use a hardware codec you must explicitly initialize the audio session and explicitly set an audio session category.";
    case kAudioSessionIncompatibleCategory:     return "The specified audio session category cannot be used for the attempted audio operation. For example, you attempted to play or record audio with the audio session category set to kAudioSessionCategory_AudioProcessing.";
    case kAudioSessionUnspecifiedError:         return "An unspecified audio session error has occurred. This typically results from the audio system being in an inconsistent state.";
    default:                                    return "Unknown error";
  }
}

//проверка ошибок Audio Services
void check_audio_session_error (OSStatus error_code, const char* source)
{
  if (error_code == kAudioSessionNoError)
    return;

  throw xtl::format_operation_exception (source, "Audio session error. %s. Code %d.", get_audio_session_error_name (error_code), error_code);
}

class ApplicationDelegateImpl: public IApplicationDelegate, public xtl::reference_counter
{
  public:
///Конструктор
    ApplicationDelegateImpl ()
      : log (LOG_NAME)
      , idle_enabled (false)
      , listener (0)
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

      register_for_push_notifications_connection = Application::RegisterNotificationHandler (common::format ("%s*", REGISTER_FOR_PUSH_NOTIFICATIONS_PREFIX).c_str (),
                                                                                             xtl::bind (&ApplicationDelegateImpl::RegisterForPushNotifications, this, _1));

      @try
      {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

        application_delegate = this;

        UIApplicationMain (0, 0, nil, @"ApplicationDelegate");

        [pool release];
      }
      @catch (NSException* e)
      {
        throw xtl::format_operation_exception (METHOD_NAME, "%s, at %s", [[e reason] cStringUsingEncoding:NSASCIIStringEncoding], [[[e callStackSymbols] description] cStringUsingEncoding:NSASCIIStringEncoding]);
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

    void OnResume ()
    {
      if (listener)
        listener->OnResume ();
    }

    void OnPause ()
    {
      if (listener)
        listener->OnPause ();
    }

///Подписка на обновления системных свойств
    void RegisterForSystemPropertiesUpdates (common::PropertyMap& system_properties)
    {
      system_properties_update_connection = system_properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, xtl::bind (&ApplicationDelegateImpl::OnSystemPropertiesUpdated, this, system_properties));
    }

    void UnregisterFromSystemPropertiesUpdates ()
    {
      system_properties_update_connection.disconnect ();
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
///Обработка обновления системных свойств
    void OnSystemPropertiesUpdated (const common::PropertyMap& property_map)
    {
      NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

      if (property_map.IsPresent ("ApplicationIconBadgeNumber"))
        [UIApplication sharedApplication].applicationIconBadgeNumber = property_map.GetInteger ("ApplicationIconBadgeNumber");

      [pool release];
    }

///Регистрация обработки push-сообщений
    void RegisterForPushNotifications (const char* notification)
    {
      common::PropertyMap params = common::parse_init_string (notification + strlen (REGISTER_FOR_PUSH_NOTIFICATIONS_PREFIX));

      UIRemoteNotificationType types = UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert;

      if (params.IsPresent ("Types"))
      {
        common::StringArray params_types = common::split (params.GetString ("Types"));

        types = 0;

        for (size_t i = 0, count = params_types.Size (); i < count; i++)
        {
          const char* type = params_types [i];

          if (!xtl::xstrcmp (type, "Badge"))
            types |= UIRemoteNotificationTypeBadge;
          else if (!xtl::xstrcmp (type, "Sound"))
            types |= UIRemoteNotificationTypeSound;
          else if (!xtl::xstrcmp (type, "Alert"))
            types |= UIRemoteNotificationTypeAlert;
          else
            log.Printf ("Ignored unknown push notification type '%s'", type);
        }
      }

      NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

      [[UIApplication sharedApplication] registerForRemoteNotificationTypes:types];

      [pool release];
    }

  private:
    common::Log                   log;                                        //протокол
    bool                          idle_enabled;                               //необходимо ли вызывать события idle
    syslib::IApplicationListener* listener;                                   //слушатель событий
    xtl::auto_connection          system_properties_update_connection;        //соединение обновления системных свойств
    xtl::auto_connection          register_for_push_notifications_connection; //соединение события инициации подписки на push-сообщения
};

}

typedef stl::vector<syslib::iphone::IApplicationListener*> ListenerArray;

@interface ApplicationDelegateInternal : NSObject
{
  @private
    ListenerArray             *listeners;                     //слушатели событий
    CADisplayLink             *idle_timer;                    //таймер вызова OnIdle
    NSTimer                   *background_idle_timer;         //таймер вызова OnIdle в неактивном состоянии
    NSDictionary              *launch_options;                //параметры запуска приложения
    bool                      main_view_visible;              //виден ли главный view приложения
    ApplicationBackgroundMode background_mode;                //режим фоновой работы приложения
    int                       suspend_audio_session_category; //аудио-категория, бывшая активной до установки режима ApplicationBackgroundMode_Active
}

@property (nonatomic, readonly) ListenerArray*            listeners;
@property (nonatomic, readonly) CADisplayLink*            idle_timer;
@property (nonatomic, retain)   NSDictionary*             launch_options;
@property (nonatomic)           bool                      main_view_visible;
@property (nonatomic)           ApplicationBackgroundMode background_mode;
@property (nonatomic)           int                       suspend_audio_session_category;

@end

@implementation ApplicationDelegateInternal

@synthesize listeners, idle_timer, launch_options, main_view_visible, background_mode, suspend_audio_session_category;

-(void)initIdleTimer
{
  [idle_timer invalidate];
  [idle_timer release];

  idle_timer = [[CADisplayLink displayLinkWithTarget:self selector:@selector (onIdle:)] retain];

  [idle_timer addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void)deleteBackgroundIdleTimer
{
  [background_idle_timer invalidate];
  [background_idle_timer release];

  background_idle_timer = nil;
}

-(void)startBackgroundIdleTimer
{
  [self deleteBackgroundIdleTimer];

  background_idle_timer = [[NSTimer scheduledTimerWithTimeInterval:BACKGROUND_IDLE_TIMER_PERIOD target:self selector:@selector (onIdle:) userInfo:nil repeats:YES] retain];
}

-(id) init
{
  self = [super init];

  if (!self)
    return nil;

  background_mode = ApplicationBackgroundMode_Suspend;

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

  [self deleteBackgroundIdleTimer];

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

-(void) setBackgroundMode:(ApplicationBackgroundMode)new_background_mode
{
  if (impl.background_mode == new_background_mode)
    return;

  try
  {
    switch (new_background_mode)
    {
      case ApplicationBackgroundMode_Active:
      {
        int suspend_audio_session_category = impl.suspend_audio_session_category;

        UInt32 buffer_size = sizeof (suspend_audio_session_category);

        check_audio_session_error (AudioSessionGetProperty (kAudioSessionProperty_AudioCategory, &buffer_size, &suspend_audio_session_category), "::AudioSessionGetProperty");

        impl.suspend_audio_session_category = suspend_audio_session_category;

        int background_execution_category = kAudioSessionCategory_MediaPlayback;

        check_audio_session_error (AudioSessionSetProperty (kAudioSessionProperty_AudioCategory,
                                   sizeof (background_execution_category), &background_execution_category), "::AudioSessionSetProperty");

        break;
      }
      case ApplicationBackgroundMode_Suspend:
      {
        int suspend_audio_session_category = impl.suspend_audio_session_category;

        check_audio_session_error (AudioSessionSetProperty (kAudioSessionProperty_AudioCategory,
                                   sizeof (suspend_audio_session_category), &suspend_audio_session_category), "::AudioSessionSetProperty");
        
        break;
      }
      default:
        throw xtl::make_argument_exception ("", "mode", new_background_mode);
    }

    impl.background_mode = new_background_mode;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::iphone::ApplicationDelegate::setBackgroundMode");
    throw;
  }
}

-(NSDictionary*)launchOptions
{
  return impl.launch_options;
}

-(ApplicationBackgroundMode) backgroundMode
{
  return impl.background_mode;
}

-(void) onRemoteNotificationReceived:(NSDictionary*)notification_data whileActive:(bool)while_active
{
  NSMutableDictionary* data = [NSMutableDictionary dictionaryWithDictionary:notification_data];

  [data setObject:[NSNumber numberWithInt: while_active ? 1 : 0] forKey:@"received_while_active"];

  stl::string json_string;

  json_string.reserve (512);

  ns_dictionary_to_json (data, json_string);

  stl::string notification = common::format ("PushNotificationReceived %s", json_string.c_str ());

  syslib::Application::PostNotification (notification.c_str ());
}

-(BOOL) application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
  application_launched = true;

  impl.launch_options = launchOptions;

  application_delegate->OnInitialize ();

  impl.idle_timer.paused = NO;

  NSDictionary *remote_notification = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];

  if (remote_notification)
    [self onRemoteNotificationReceived:remote_notification whileActive:false];

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
  impl.idle_timer.paused = YES;
  [impl.idle_timer invalidate];

  if (impl.background_mode == ApplicationBackgroundMode_Active)
    [impl startBackgroundIdleTimer];

  if (application_delegate)
    application_delegate->OnPause ();
}

-(void) applicationDidBecomeActive:(UIApplication*)application
{
  [impl deleteBackgroundIdleTimer];
  [impl initIdleTimer];

  if (application_delegate)
    application_delegate->OnResume ();
}

-(NSUInteger)application:(UIApplication*)application supportedInterfaceOrientationsForWindow:(UIWindow*)window
{
  return UIInterfaceOrientationMaskAll;
}

//push notifications
-(void)application:(UIApplication*)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken
{
  const unsigned char              *token_bytes = (unsigned char*)[deviceToken bytes];
  size_t                           token_size   = [deviceToken length];
  xtl::uninitialized_storage<char> token (token_size * 2 + 1);

  for (size_t i = 0; i < token_size; i++)
    xtl::xsnprintf (token.data () + i * 2, 3, "%02x", token_bytes [i]);

  token.data () [token.size () - 1] = 0;

  common::Log (LOG_NAME).Printf ("Registration for push notifications succeeded, token '%s'", token.data ());

  stl::string notification = common::format ("RegisterForPushNotification Succeeded %s", token.data ());

  syslib::Application::PostNotification (notification.c_str ());
}

-(void)application:(UIApplication*)application didFailToRegisterForRemoteNotificationsWithError:(NSError*)error
{
  common::Log (LOG_NAME).Printf ("Registration for push notifications failed, error '%s'", [[error description] UTF8String]);

  stl::string notification = common::format ("RegisterForPushNotification Failed %s", [[error description] UTF8String]);

  syslib::Application::PostNotification (notification.c_str ());
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
  [self onRemoteNotificationReceived:userInfo whileActive:[UIApplication sharedApplication].applicationState == UIApplicationStateActive];
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

namespace iphone
{

bool is_in_run_loop () //запущен ли главный цикл
{
  return application_launched;
}

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
   Получение параметров запуска приложения
*/

NSDictionary* ApplicationManager::GetLaunchOptions ()
{
  if (!application_launched)
    throw xtl::format_operation_exception ("syslib::iphone::ApplicationManager::GetLaunchOptions", "Application was not launched yet");

  return [(ApplicationDelegate*)([UIApplication sharedApplication].delegate) launchOptions];
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

/*
   Управление режимом работы в фоне
*/

void IPhoneApplicationManager::SetApplicationBackgroundMode (syslib::ApplicationBackgroundMode mode)
{
  [(ApplicationDelegate*)([UIApplication sharedApplication].delegate) setBackgroundMode:mode];
}

syslib::ApplicationBackgroundMode IPhoneApplicationManager::GetApplicationBackgroundMode ()
{
  return [(ApplicationDelegate*)([UIApplication sharedApplication].delegate) backgroundMode];
}

/*
   Управление энергосбережением
*/

void IPhoneApplicationManager::SetScreenSaverState (bool state)
{
  [UIApplication sharedApplication].idleTimerDisabled = !state;
}

bool IPhoneApplicationManager::GetScreenSaverState ()
{
  return ![UIApplication sharedApplication].idleTimerDisabled;
}

/*
   Получение системных свойств
*/

void IPhoneApplicationManager::GetSystemProperties (common::PropertyMap& properties)
{
  if (application_delegate)
    application_delegate->UnregisterFromSystemPropertiesUpdates ();

  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSString* system_version = [[UIDevice currentDevice] systemVersion];

  properties.SetProperty ("Operating System", "iOS");
  properties.SetProperty ("Platform", UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? "pad" : "phone");
  properties.SetProperty ("Language", [((NSString*)[[NSLocale preferredLanguages] objectAtIndex:0]) UTF8String]);
  properties.SetProperty ("OSVersion", [system_version UTF8String]);

  NSString* application_version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];

  if ([application_version isKindOfClass:[NSString class]])
    properties.SetProperty ("ApplicationVersion", [application_version UTF8String]);

  size_t model_name_size = 0;

  sysctlbyname ("hw.machine", 0, &model_name_size, 0, 0);

  xtl::uninitialized_storage<char> model_name (model_name_size + 1);

  if (!sysctlbyname ("hw.machine", model_name.data (), &model_name_size, 0, 0))
  {
    model_name.data () [model_name_size] = 0;
    properties.SetProperty ("DeviceModel", model_name.data ());
  }

  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

  NSString* uuid = [defaults objectForKey:USER_DEFAULTS_UUID];

  if (!uuid)
  {
    if ([system_version compare:@"6.0" options:NSNumericSearch] != NSOrderedAscending)
    {
      uuid = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
    }
    else
    {
      CFUUIDRef cf_uuid = CFUUIDCreate (0);

      uuid = [(NSString*)CFUUIDCreateString (0, cf_uuid) autorelease];

      CFRelease (cf_uuid);

      [defaults setObject:uuid forKey:USER_DEFAULTS_UUID];
      [defaults synchronize];
    }
  }

  properties.SetProperty ("UUID", [uuid UTF8String]);

  struct sockaddr_in zeroAddress;

  bzero(&zeroAddress, sizeof(zeroAddress));
  zeroAddress.sin_len = sizeof(zeroAddress);
  zeroAddress.sin_family = AF_INET;

  SCNetworkReachabilityRef   defaultRouteReachability = SCNetworkReachabilityCreateWithAddress (0, (struct sockaddr *)&zeroAddress);
  SCNetworkReachabilityFlags flags;

  if (SCNetworkReachabilityGetFlags(defaultRouteReachability, &flags) && (flags & kSCNetworkReachabilityFlagsReachable) && (flags & kSCNetworkReachabilityFlagsTransientConnection))
    properties.SetProperty ("CellularOnlyInternet", 1);

  properties.SetProperty ("ApplicationIconBadgeNumber", [UIApplication sharedApplication].applicationIconBadgeNumber);

  [pool release];

  if (application_delegate)
    application_delegate->RegisterForSystemPropertiesUpdates (properties);
}
