#include "shared.h"

#import <UIKit/UIPasteboard.h>

#import <FacebookSDK/FBAppCall.h>
#import <FacebookSDK/FBAppEvents.h>
#import <FacebookSDK/FBSession.h>
#import <FacebookSDK/FBSettings.h>
#import <FacebookSDK/FBErrorUtility.h>

using namespace social;
using namespace social::facebook;

//static NSString* RESOURCES_VERSION_KEY_NAME = @"FacebookSdkResourcesVersion"; //key for facebook sdk resources version in localization file

namespace
{

//Constants
const char* LOG_NAME = "social.facebook.FacebookSession.IOsPlatform";

const float WAIT_FOR_URL_AFTER_ACTIVATE_DELAY = 0.1f; //Duration for waiting for opened from with URL notification after activation

//Interface of login results events
class ILoginResultListener
{
  public:
    virtual void OnLoginResult (bool platform_login_result, OperationStatus status, const char* error, const char* token) = 0;

  protected:
    virtual ~ILoginResultListener () {}
};

//code copied from facebook sdk 3.18.1 src/Core/FBUtility.m
bool is_publish_permission (NSString *permission)
{
  return [permission hasPrefix:@"publish"] ||
         [permission hasPrefix:@"manage"] ||
         [permission isEqualToString:@"ads_management"] ||
         [permission isEqualToString:@"create_event"] ||
         [permission isEqualToString:@"rsvp_event"];
}

}

@interface FacebookLoginer : NSObject
{
  @private
    ILoginResultListener* listener;
}

@property (nonatomic, assign) ILoginResultListener* listener;

@end

@implementation FacebookLoginer

@synthesize listener;

-(void)dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(id)initWithListener:(ILoginResultListener*)in_listener
{
  self = [super init];

  if (!self)
    return nil;

  listener = in_listener;

  return self;
}

-(void)loginWithAppId:(const char*)app_id properties:(const common::PropertyMap&)properties
{
  printf ("%s %d\n", __FUNCTION__, __LINE__);
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector (onOpenURL:)
                                               name:@"ApplicationOpenURL"
                                             object:[UIApplication sharedApplication].delegate];

  [FBSettings setDefaultAppID:[NSString stringWithUTF8String:app_id]];

  if (properties.IsPresent ("UrlSchemeSuffix"))
    [FBSettings setDefaultUrlSchemeSuffix:[NSString stringWithUTF8String:properties.GetString ("UrlSchemeSuffix")]];

  common::StringArray permissions;

  if (properties.IsPresent ("Permissions"))
    permissions = common::split (properties.GetString ("Permissions"), ",");

  NSMutableArray *read_permissions    = [NSMutableArray arrayWithCapacity:permissions.Size ()],
                 *publish_permissions = [NSMutableArray arrayWithCapacity:permissions.Size ()];

  [read_permissions addObject:@"public_profile"];

  for (size_t i = 0, count = permissions.Size (); i < count; i++)
  {
    NSString* permission = [NSString stringWithUTF8String:permissions [i]];

    if (is_publish_permission (permission))
      [publish_permissions addObject:permission];
    else
      [read_permissions addObject:permission];
  }

  if ([publish_permissions count])
    throw xtl::format_operation_exception ("social::facebook::ios_platform::loginWithAppId:properties:", "Publish permissions requesting is not implemented");

  printf ("%s %d\n", __FUNCTION__, __LINE__);
  // Open session with public_profile (required) and user_birthday read permissions
  [FBSession openActiveSessionWithReadPermissions:read_permissions
                                     allowLoginUI:YES
                                completionHandler:
       ^(FBSession *session, FBSessionState state, NSError *error)
       {
         printf ("%s %d\n", __FUNCTION__, __LINE__);
         if (error)
         {
           printf ("%s %d\n", __FUNCTION__, __LINE__);
           // If the user cancelled login
           if ([FBErrorUtility errorCategoryForError:error] == FBErrorCategoryUserCancelled)
           {
             printf ("%s %d\n", __FUNCTION__, __LINE__);
             [self onLoginCanceled];
             return;
           }

           printf ("%s %d\n", __FUNCTION__, __LINE__);
           [self onLoginFailedWithError:[error description]];
           return;
         }

         printf ("%s %d\n", __FUNCTION__, __LINE__);
         if (state != FBSessionStateOpen)
           return;
         printf ("%s %d ******   %d\n", __FUNCTION__, __LINE__, FBSession.activeSession.isOpen);

         if (![publish_permissions count])
         {
           printf ("%s %d\n", __FUNCTION__, __LINE__);
           [self onLoginSucceeded];
           return;
         }
         printf ("%s %d\n", __FUNCTION__, __LINE__);

         //TODO request publish permissions
       }];
}

-(void)onLoginSucceeded
{
  printf ("%s %d\n", __FUNCTION__, __LINE__);
  if (!listener)
    return;

  listener->OnLoginResult (true, OperationStatus_Success, "", [[FBSession activeSession].accessTokenData.accessToken UTF8String]);
}

-(void)onLoginCanceled
{
  printf ("%s %d\n", __FUNCTION__, __LINE__);
  if (!listener)
    return;

  listener->OnLoginResult (true, OperationStatus_Canceled, "", "");
}

-(void)onLoginFailedWithError:(NSString*)error
{
  printf ("%s %d\n", __FUNCTION__, __LINE__);
  if (!listener)
    return;

  listener->OnLoginResult (true, OperationStatus_Failure, [error UTF8String], "");
}

//Handle url opening
-(void)onOpenURL:(NSNotification*)notification
{
  [FBAppCall handleOpenURL:notification.userInfo [@"URL"] sourceApplication:notification.userInfo [@"sourceApplication"]];
}


@end

namespace
{

//Platform singleton
class IOsPlatformImpl : public ILoginResultListener
{
  public:
    //Constructor
    IOsPlatformImpl ()
      : log (LOG_NAME)
      , on_activate_connection (syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnResume, xtl::bind (&IOsPlatformImpl::OnApplicationActivated, this)))
      {}

    //Destructor
    ~IOsPlatformImpl ()
    {
      RemoveLoginHandler ();
    }

    //Login/cancel login
    void Login (const char* app_id, const social::facebook::DefaultPlatform::PlatformLoginCallback& callback, const common::PropertyMap& properties)
    {
      printf ("%s %d\n", __FUNCTION__, __LINE__);

      try
      {
        if (!CanLogin ())
        {
          callback (false, OperationStatus_Failure, "", "");
          return;
        }

        if (facebook_loginer)
          throw xtl::format_operation_exception ("", "Login is already in progress");

        if ([FBSession activeSession].isOpen)  //we called to refresh token, needs to reauthorize
          LogOut ();

        on_activate_login_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnResume, xtl::bind (&IOsPlatformImpl::OnApplicationLoginActivated, this));

        login_handler = callback;

        facebook_loginer = [[FacebookLoginer alloc] initWithListener:this];

        [facebook_loginer loginWithAppId:app_id properties:properties];
      }
      catch (xtl::exception& e)
      {
        e.touch ("social::facebook::IOsPlatformImpl::Login");
        throw;
      }
    }

    void CancelLogin ()
    {
      RemoveLoginHandler ();
    }

    void LogOut ()
    {
      RemoveLoginHandler ();

      [[FBSession activeSession] closeAndClearTokenInformation];
    }

    //Remove login handlers
    void RemoveLoginHandler ()
    {
      facebook_loginer.listener = nil;

      [facebook_loginer release];

      facebook_loginer = nil;

      on_activate_login_connection.disconnect ();

      login_handler = social::facebook::DefaultPlatform::PlatformLoginCallback ();

      login_error_action.Cancel ();
    }

    //Handle application activation
    void TrackActivateApp ()
    {
      try
      {
        log.Printf ("Track app activated event");

        [FBAppEvents activateApp];
      }
      catch (xtl::exception& e)
      {
        e.touch ("social::facebook::IOsPlatformImpl::TrackActivateApp");
        throw;
      }
    }

    void OnLoginResult (bool platform_login_result, OperationStatus status, const char* error, const char* token)
    {
      login_handler (platform_login_result, status, error, token);

      RemoveLoginHandler ();
    }

  private:
    bool CanLogin ()
    {
      if ([[[UIDevice currentDevice] systemVersion] compare:@"5.0" options:NSNumericSearch] == NSOrderedAscending) //facebook SDK doesn't support iOS versions prior to 5.0
        return false;

/*      NSString* resources_version = NSLocalizedString (RESOURCES_VERSION_KEY_NAME, @"");

      if ([resources_version isEqualToString:RESOURCES_VERSION_KEY_NAME] || [resources_version isEqualToString:@""])
      {
        log.Print ("Application has no Facebook SDK resources, sdk login unavailable");
        return false;
      }

      log.Printf ("Check can login. SDK version = %s, resourcesVersion = %s", [[FBSettings sdkVersion] UTF8String], [resources_version UTF8String]);

      if (![[FBSettings sdkVersion] isEqualToString:resources_version])
        throw xtl::format_operation_exception ("social::facebook::IOsPlatformImpl::CanLogin", "Facebook SDK and Facebook SDK resources version mismatch");*/

      return true;
    }

    //Handle application activation during login
    void OnApplicationLoginActivated ()
    {
      login_error_action = common::ActionQueue::PushAction (xtl::bind (&IOsPlatformImpl::OnLoginUrlNotReceived, this), common::ActionThread_Current, WAIT_FOR_URL_AFTER_ACTIVATE_DELAY);
    }

    //Notification about login error, in case of application was activated not from facebook redirect while waiting for login result
    void OnLoginUrlNotReceived ()
    {
      login_handler (true, OperationStatus_Failure, "Login URL not received", "");

      RemoveLoginHandler ();
    }

    //Handle app activation
    void OnApplicationActivated ()
    {
      [FBAppCall handleDidBecomeActive];
      TrackActivateApp ();
    }

  private:
    common::Log                                              log;                          //log
    xtl::auto_connection                                     on_activate_connection;       //connection for application activation event
    xtl::auto_connection                                     on_activate_login_connection; //connection for application activation during login event
    common::Action                                           login_error_action;           //action for delayed notification about login error, in case of application was activated not from facebook redirect while waiting for login result
    social::facebook::DefaultPlatform::PlatformLoginCallback login_handler;                //login results handler
    FacebookLoginer*                                         facebook_loginer;             //object performing objective-C login operations
};

typedef common::Singleton<IOsPlatformImpl> IOsPlatformSingleton;

}

/*
   iOs platform
*/

/*
   Login/logout
*/

void IOsPlatform::Login (const char* app_id, const PlatformLoginCallback& callback, const common::PropertyMap& properties)
{
  IOsPlatformSingleton::Instance ()->Login (app_id, callback, properties);
}

void IOsPlatform::CancelLogin ()
{
  IOsPlatformSingleton::Instance ()->CancelLogin ();
}

void IOsPlatform::LogOut ()
{
  IOsPlatformSingleton::Instance ()->LogOut ();
}

/*
   Check if facebook application is installed
*/

bool IOsPlatform::IsFacebookAppInstalled ()
{
  return [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:@"fbauth://"]];
}

/*
   Publish app install event
*/

void IOsPlatform::PublishAppInstallEvent (const char*)
{
  IOsPlatformSingleton::Instance ()->TrackActivateApp ();
}
