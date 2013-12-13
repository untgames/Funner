#include "shared.h"

#import <AdSupport/ASIdentifierManager.h>
#import <UIKit/UIPasteboard.h>

using namespace social;
using namespace social::facebook;

static NSString *kStringBoundary = @"3i2ndDfv2rTHiSisAbouNdArYfORhtTPEefj3q2f";

@interface FacebookRequestBody : NSObject

@property (nonatomic, retain, readonly) NSData *data;
@property (nonatomic, retain, readonly) NSMutableData *mutableData;

- (void)appendUTF8:(NSString *)utf8;

@end

@implementation FacebookRequestBody

@synthesize mutableData = _mutableData;

- (id)init
{
  self = [super init];

  if (!self)
    return nil;

  _mutableData = [[NSMutableData alloc] init];

  return self;
}

- (void)dealloc
{
  [_mutableData release];

  [super dealloc];
}

+ (NSString *)mimeContentType
{
  return [NSString stringWithFormat:@"multipart/form-data; boundary=%@", kStringBoundary];
}

- (void)appendUTF8:(NSString *)utf8
{
  if (![self.mutableData length])
  {
    NSString *headerUTF8 = [NSString stringWithFormat:@"--%@\r\n", kStringBoundary];
    NSData *headerData = [headerUTF8 dataUsingEncoding:NSUTF8StringEncoding];
    [self.mutableData appendData:headerData];
  }

  NSData *data = [utf8 dataUsingEncoding:NSUTF8StringEncoding];
  [self.mutableData appendData:data];
}

- (void)appendRecordBoundary
{
  NSString *boundary = [NSString stringWithFormat:@"\r\n--%@\r\n", kStringBoundary];
  [self appendUTF8:boundary];
}

- (void)appendWithKey:(NSString *)key
            formValue:(NSString *)value
{
  NSString *disposition = [NSString stringWithFormat:@"Content-Disposition: form-data; name=\"%@\"\r\n\r\n", key];
  [self appendUTF8:disposition];
  [self appendUTF8:value];
  [self appendRecordBoundary];
}

- (NSData *)data
{
  // No need to enforce immutability since this is internal-only and sdk will
  // never cast/modify.
  return self.mutableData;
}

@end

namespace
{

//Constants
const char* LOG_NAME                     = "social.facebook.FacebookSession.IOsPlatform";
const char* OPEN_URL_NOTIFICATION_PREFIX = "ApplicationOpenURL ";

const float WAIT_FOR_URL_AFTER_ACTIVATE_DELAY = 0.1f; //Длительность ожидания URL после активации до сообщения о ошибке

common::ParseNode load_json_from_file (common::File& input_file)
{
  xtl::uninitialized_storage<char> buffer (input_file.Size () + 1);

  size_t bytes_copied = 0;

  while (!input_file.Eof ())
  {
    size_t read_size = input_file.Read (buffer.data () + bytes_copied, buffer.size () - bytes_copied);

    if (!read_size)
      break;

    bytes_copied += read_size;
  }

  if (!input_file.Eof ())
    throw xtl::format_operation_exception ("social::facebook::load_json_from_file", "Internal error: can't read input file");

  buffer.data () [bytes_copied] = 0;

  common::Log (LOG_NAME).Printf ("JSON loaded: '%s'", buffer.data ());

  common::Parser   parser ("json", bytes_copied, buffer.data (), "json");
  common::ParseLog parse_log = parser.Log ();

  for (size_t i = 0; i < parse_log.MessagesCount (); i++)
    switch (parse_log.MessageType (i))
    {
      case common::ParseLogMessageType_Error:
      case common::ParseLogMessageType_FatalError:
        throw xtl::format_operation_exception ("social::facebook::load_json_from_file", "Parser error: '%s'", parse_log.Message (i));
        break;
      default:
        break;
    }

  common::ParseNode iter = parser.Root ().First ();

  if (!iter)
    throw xtl::format_operation_exception ("social::facebook::load_json_from_file", "There is no root node in response");

  return iter;
}

//Синглтон платформы
class IOsPlatformImpl
{
  public:
    //Конструктор
    IOsPlatformImpl ()
      : log (LOG_NAME)
      { }

    //Деструктор
    ~IOsPlatformImpl ()
    {
      RemoveLoginHandler ();
    }

    //Добавление/удаление обработчиков открытия ссылок
    void SetLoginHandler (const char* app_base_uri, const social::facebook::DefaultPlatform::PlatformLoginCallback& handler)
    {
      try
      {
        if (!app_base_uri)
          throw xtl::make_null_argument_exception ("", "app_base_uri");

        open_url_connection    = syslib::Application::RegisterNotificationHandler (common::format ("%s%s*", OPEN_URL_NOTIFICATION_PREFIX, app_base_uri).c_str (), xtl::bind (&IOsPlatformImpl::OnOpenUrl, this, _1));
        on_activate_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnResume, xtl::bind (&IOsPlatformImpl::OnApplicationActivated, this));

        login_handler = handler;
      }
      catch (xtl::exception& e)
      {
        e.touch ("social::facebook::IOsPlatformImpl::AddOpenUrlHandler");
        throw;
      }
    }

    void RemoveLoginHandler ()
    {
      open_url_connection.disconnect ();
      on_activate_connection.disconnect ();

      login_handler = social::facebook::DefaultPlatform::PlatformLoginCallback ();

      login_error_action.Cancel ();
    }

    //Отправка события установки приложения
    void PublishInstall (const char* app_id)
    {
      try
      {
        if (!app_id)
          throw xtl::make_null_argument_exception ("", "app_id");

        if (install_app_published)
          return;

        log.Printf ("Publishing install app event");

        install_app_published = true;

        // look for a previous ping & grab the facebook app's current attribution id.
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSString *pingKey = [NSString stringWithFormat:@"com.facebook.sdk:lastAttributionPing%s", app_id, nil];
        NSDate *lastPing = [defaults objectForKey:pingKey];

        if (lastPing)
        {
          log.Printf ("Install app event already published");
          return;
        }

        NSString *attributionID = [[UIPasteboard pasteboardWithName:@"fb_app_attribution" create:NO] string];
        NSString *advertiserID  = nil;

        if ([ASIdentifierManager class])
        {
          ASIdentifierManager *manager = [ASIdentifierManager sharedManager];
          advertiserID = [[manager advertisingIdentifier] UUIDString];
        }

        if (attributionID || advertiserID)
        {
          stl::string attribution_id = attributionID ? [attributionID UTF8String] : "";
          stl::string advertiser_id  = advertiserID ? [advertiserID UTF8String] : "";

          common::ActionQueue::PushAction (xtl::bind (&IOsPlatformImpl::PerformPublishInstall, this, attribution_id, advertiser_id, app_id), common::ActionThread_Background);
        }
        else
        {
          log.Printf ("Can't publish install app event, no attribution and no advertiser id found");
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("social::facebook::IOsPlatformImpl::PublishInstall");
        throw;
      }
    }

    void PerformPublishInstall (stl::string attribution_id, stl::string advertiser_id, stl::string app_id)
    {
      NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

      try
      {
        stl::string ping_url = common::format ("https://graph.facebook.com/%s?fields=supports_attribution&format=json&migration_bundle=fbsdk%%3A20121003&sdk=ios", app_id.c_str ());

        log.Printf ("Performing request '%s'", ping_url.c_str ());

        common::StdFile ping_file (ping_url.c_str (), common::FileMode_Read);

        common::ParseNode ping_response = load_json_from_file (ping_file);

        ping_file.Close ();

        common::ParseNode supports_attribution_node = ping_response.First ("supports_attribution");

        if (!supports_attribution_node || !supports_attribution_node.AttributesCount ())
        {
          [pool release];
          return;
        }

        bool supports_attribution = !xtl::xstrcmp (supports_attribution_node.Attribute (0), "true");

        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSString *pingKey = [NSString stringWithFormat:@"com.facebook.sdk:lastAttributionPing%s", app_id.c_str (), nil];

        if (supports_attribution)
        {
          // set up the HTTP POST to publish the attribution ID.
          FacebookRequestBody* body = [[FacebookRequestBody alloc] init];

          [body appendWithKey:@"format" formValue:@"json"];
          [body appendWithKey:@"migration_bundle" formValue:@"fbsdk:20121003"];
          [body appendWithKey:@"sdk" formValue:@"ios"];

          if (!advertiser_id.empty ())
            [body appendWithKey:@"advertiser_id" formValue:[NSString stringWithUTF8String:advertiser_id.c_str ()]];

          [body appendWithKey:@"event" formValue:@"MOBILE_APP_INSTALL"];

          if (!attribution_id.empty ())
            [body appendWithKey:@"attribution" formValue:[NSString stringWithUTF8String:attribution_id.c_str ()]];

          stl::string publish_url = common::format ("https://graph.facebook.com/%s/activities?format=json&migration_bundle=fbsdk%%3A20121003&sdk=ios", app_id.c_str ());

          log.Printf ("Performing request '%s'", publish_url.c_str ());

          NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:publish_url.c_str ()]]
                                            cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
                                        timeoutInterval:60];

          [request setHTTPMethod:@"POST"];
          [request setValue:@"FBiOSSDK.3.1.1" forHTTPHeaderField:@"User-Agent"];
          [request setValue:[FacebookRequestBody mimeContentType] forHTTPHeaderField:@"Content-Type"];

          [request setHTTPBody:[body data]];

          [body release];

          NSURLResponse* response = nil;
          NSError*       error    = nil;

          NSData* response_data = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];

          if (response_data)
          {
            NSString* response_string = [[NSString alloc] initWithData:response_data encoding:NSUTF8StringEncoding];
            log.Printf ("Publish response: '%s'", [response_string UTF8String]);
            [response_string release];

            [defaults setObject:[NSDate date] forKey:pingKey];
            [defaults synchronize];
          }
          else
            throw xtl::format_operation_exception ("", "Publish failed, error '%s'", [[error description] UTF8String]);
        }
        else
        {
          log.Printf ("Attribution not supported");

          // the app has turned off install insights.  prevent future attempts.
          [defaults setObject:[NSDate date] forKey:pingKey];
          [defaults synchronize];
        }
      }
      catch (xtl::exception& e)
      {
        log.Printf ("Request failed, error '%s'", e.what ());

        // there was a problem.  allow a repeat execution.
        install_app_published = false;
      }
      catch (...)
      {
        log.Printf ("Request failed, unknown error");

        // there was a problem.  allow a repeat execution.
        install_app_published = false;
      }

      [pool release];
    }

  private:
    //Обработка открытия приложения по ссылке
    void OnOpenUrl (const char* notification)
    {
      const char* url = notification + strlen (OPEN_URL_NOTIFICATION_PREFIX);

      stl::string token = get_url_parameter (url, "access_token=");

      if (token.empty ())  //login failed
      {
        stl::string error = get_url_parameter (url, "error_reason=");

        if (error == "user_denied")
        {
          log.Printf ("Login canceled");

          login_handler (true, OperationStatus_Canceled, "", "");
        }
        else
        {
          log.Printf ("Login failed, error '%s'", error.c_str ());

          login_handler (true, OperationStatus_Failure, error.c_str (), "");
        }
      }
      else
      {
        log.Printf ("Logged in");

        login_handler (true, OperationStatus_Success, "", token.c_str ());
      }

      RemoveLoginHandler ();
    }

    //Обработка активации приложения во время ожидания данных логина от внешнего приложения
    void OnApplicationActivated ()
    {
      login_error_action = common::ActionQueue::PushAction (xtl::bind (&IOsPlatformImpl::OnLoginUrlNotReceived, this), common::ActionThread_Current, WAIT_FOR_URL_AFTER_ACTIVATE_DELAY);
    }

    //Оповещение о ошибке логина, если приложение было активировано не по внешней ссылке во время ожидания результатов логина
    void OnLoginUrlNotReceived ()
    {
      login_handler (true, OperationStatus_Failure, "Login URL not received", "");

      RemoveLoginHandler ();
    }

  private:
    common::Log                                              log;                      //протокол
    xtl::auto_connection                                     open_url_connection;      //соединение открытия приложения по вшеншней ссылке
    xtl::auto_connection                                     on_activate_connection;   //соединение активации приложения
    common::Action                                           login_error_action;       //экшн отложенного оповещения о ошибке логина, если приложение было активировано не по внешней ссылке во время ожидания результатов логина
    social::facebook::DefaultPlatform::PlatformLoginCallback login_handler;            //обработчик результатов логина
    volatile bool                                            install_app_published;    //отправлено ли событие установки приложения
};

typedef common::Singleton<IOsPlatformImpl> IOsPlatformSingleton;

}

/*
   iOs платформа
*/

/*
   Логин (возвращает true, если началось выполнение специфичного для данной платформы логина)
*/

void IOsPlatform::Login (const char* app_id, const PlatformLoginCallback& callback, const common::PropertyMap& properties)
{
  //Facebook application auth
  stl::string scheme = "fbauth";
  stl::string url_scheme_suffix;

  if (properties.IsPresent ("UrlSchemeSuffix"))
  {
    url_scheme_suffix = properties.GetString ("UrlSchemeSuffix");

    scheme.append ("2");
  }

  stl::string params;

  if (properties.IsPresent ("Permissions"))
    params = common::format ("&scope=%s", properties.GetString ("Permissions"));
  if (!url_scheme_suffix.empty ())
    params.append (common::format ("&local_client_id=%s", url_scheme_suffix.c_str ()));

  NSString *url = [NSString stringWithFormat:@"%s://authorize?client_id=%s%s", scheme.c_str (), app_id, params.c_str ()];

  stl::string app_base_uri = common::format ("fb%s%s://authorize", app_id, url_scheme_suffix.c_str ());

  IOsPlatformSingleton::Instance ()->SetLoginHandler (app_base_uri.c_str (), callback);

  if ([[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]])
    return;

  url = [NSString stringWithFormat:@"https://m.facebook.com/dialog/oauth?type=user_agent&sdk=ios&client_id=%s&redirect_uri=%s&display=touch&response_type=token%s", app_id, app_base_uri.c_str (), params.c_str ()];

  if ([[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]])
    return;

  IOsPlatformSingleton::Instance ()->RemoveLoginHandler ();

  callback (false, OperationStatus_Failure, "", "");
}

void IOsPlatform::CancelLogin ()
{
  IOsPlatformSingleton::Instance ()->RemoveLoginHandler ();
}

/*
   Публикация события установки приложения
*/

void IOsPlatform::PublishAppInstallEvent (const char* app_id)
{
  IOsPlatformSingleton::Instance ()->PublishInstall (app_id);
}
