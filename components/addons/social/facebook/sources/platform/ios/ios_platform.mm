#include "shared.h"

#import <FBSDKCoreKit/FBSDKAccessToken.h>
#import <FBSDKCoreKit/FBSDKAppEvents.h>
#import <FBSDKCoreKit/FBSDKApplicationDelegate.h>
#import <FBSDKCoreKit/FBSDKSettings.h>

#import <FBSDKLoginKit/FBSDKLoginManager.h>
#import <FBSDKLoginKit/FBSDKLoginManagerLoginResult.h>

using namespace social;
using namespace social::facebook;

namespace
{

//Constants
const char* LOG_NAME = "social.facebook.FacebookSession.IOsPlatform";

const common::ActionQueue::time_t WAIT_FOR_URL_AFTER_ACTIVATE_DELAY (100, 1000); //Duration for waiting for opened from with URL notification after activation

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
  [FBSDKSettings setAppID:[NSString stringWithUTF8String:app_id]];

  if (properties.IsPresent ("UrlSchemeSuffix"))
    [FBSDKSettings setAppURLSchemeSuffix:[NSString stringWithUTF8String:properties.GetString ("UrlSchemeSuffix")]];

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

  // Open session with public_profile (required) and user_birthday read permissions
  FBSDKLoginManager* loginManager = [[FBSDKLoginManager alloc] init];

  [loginManager logInWithReadPermissions:read_permissions
                      fromViewController:nil
                                 handler:
       ^(FBSDKLoginManagerLoginResult *result, NSError *error)
       {
         //TODO handle result.declinedPermissions
         [loginManager autorelease];

         if (result.isCancelled)
         {
           [self onLoginCanceled];
           return;
         }

         if (error)
         {
           [self onLoginFailedWithError:[error description]];
           return;
         }

         if (![publish_permissions count])
         {
           [self onLoginSucceeded];
           return;
         }

         //TODO request publish permissions
       }];
}

-(void)onLoginSucceeded
{
  if (!listener)
    return;

  //TODO implement FBSDKAccessTokenDidChangeNotification

  listener->OnLoginResult (true, OperationStatus_Success, "", [[FBSDKAccessToken currentAccessToken].tokenString UTF8String]);
}

-(void)onLoginCanceled
{
  if (!listener)
    return;

  listener->OnLoginResult (true, OperationStatus_Canceled, "", "");
}

-(void)onLoginFailedWithError:(NSString*)error
{
  if (!listener)
    return;

  listener->OnLoginResult (true, OperationStatus_Failure, [error UTF8String], "");
}

@end

@interface FacebookOpenURLHandler: NSObject
{
}

@end

@implementation FacebookOpenURLHandler

-(void)dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(id)init
{
  self = [super init];

  if (!self)
    return nil;

  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector (onOpenURL:)
                                               name:@"ApplicationOpenURL"
                                             object:[UIApplication sharedApplication].delegate];

  return self;
}

//Handle url opening
-(void)onOpenURL:(NSNotification*)notification
{
  [[FBSDKApplicationDelegate sharedInstance] application:notification.userInfo [@"application"]
      openURL:notification.userInfo [@"URL"]
      sourceApplication:notification.userInfo [@"sourceApplication"]
      annotation:notification.userInfo [@"annotation"]
    ];
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
      , facebook_loginer (0)
    {
      NSDictionary* launch_options = syslib::iphone::ApplicationManager::GetLaunchOptions ();

      [[FBSDKApplicationDelegate sharedInstance] application:[UIApplication sharedApplication] didFinishLaunchingWithOptions:launch_options];

      facebook_open_url_handler = [[FacebookOpenURLHandler alloc] init];
    }

    //Destructor
    ~IOsPlatformImpl ()
    {
      [facebook_open_url_handler release];

      RemoveLoginHandler ();
    }

    //Login/cancel login
    void Login (const char* app_id, const social::facebook::DefaultPlatform::PlatformLoginCallback& callback, const common::PropertyMap& properties)
    {
      try
      {
        if (facebook_loginer)
          throw xtl::format_operation_exception ("", "Login is already in progress");

        FBSDKAccessToken* current_access_token = [FBSDKAccessToken currentAccessToken];

        if (current_access_token)  //we are already logged in
        {
          if (properties.IsPresent ("Token") && !xtl::xstrcmp (properties.GetString("Token"), [current_access_token.tokenString UTF8String]))
          {
            bool needs_new_permissions = false;

            common::StringArray permissions;

            if (properties.IsPresent ("Permissions"))
              permissions = common::split (properties.GetString ("Permissions"), ",");

            permissions.Add ("public_profile");

            for (size_t i = 0, count = permissions.Size (); i < count; i++)
            {
              if (![current_access_token hasGranted:[NSString stringWithUTF8String:permissions [i]]])
              {
                needs_new_permissions = true;
                break;
              }
            }

            if (!needs_new_permissions)
            {
              //we already have valid token with needed permissions set
              login_handler = callback;

              OnLoginResult (true, OperationStatus_Success, "", [current_access_token.tokenString UTF8String]);

              return;
            }
          }

          LogOut ();
        }

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

      FBSDKLoginManager* loginManager = [[FBSDKLoginManager alloc] init];

      [loginManager logOut];

      [loginManager release];
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

        [FBSDKAppEvents activateApp];
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
      TrackActivateApp ();
    }

  private:
    common::Log                                              log;                          //log
    xtl::auto_connection                                     on_activate_connection;       //connection for application activation event
    xtl::auto_connection                                     on_activate_login_connection; //connection for application activation during login event
    common::Action                                           login_error_action;           //action for delayed notification about login error, in case of application was activated not from facebook redirect while waiting for login result
    social::facebook::DefaultPlatform::PlatformLoginCallback login_handler;                //login results handler
    FacebookLoginer*                                         facebook_loginer;             //object performing objective-C login operations
    FacebookOpenURLHandler*                                  facebook_open_url_handler;    //object performing translation of open url event to Facebook SDK
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
  return [[UIApplication sharedApplication] canOpenURL:[NSURL URLWithString:@"fbauth2://"]];
}

/*
   Publish app install event
*/

void IOsPlatform::PublishAppInstallEvent (const char*)
{
  IOsPlatformSingleton::Instance ()->TrackActivateApp ();
}

/*
   Handle component start, used for initializing facebook SDK before user logins
*/

void IOsPlatform::OnComponentStart ()
{
  IOsPlatformSingleton::Instance ();
}
