#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

#import <UIKit/UIApplication.h>
#import <UIKit/UIImage.h>
#import <UIKit/UIImageView.h>
#import <UIKit/UIViewController.h>
#import <UIKit/UIScreen.h>
#import <UIKit/UIWindow.h>

#import "Tracking.h"

#import <MobileAppTracker/MobileAppTracker.h>

#include <cstdio>

#include <utility/utils.h>

#include <launcher/application.h>

using namespace engine;
using namespace plarium::launcher;

//AD-X constants
static NSString* PHONE_ADX_URL_SCHEME = @"ADX1144";
static NSString* PAD_ADX_URL_SCHEME   = @"ADX1145";
static NSString* PHONE_APPLE_ID       = @"543831789";
static NSString* PAD_APPLE_ID         = @"586574454";

//MobileAppTracking constants
static NSString* MAT_AD_ID  = @"5740";
static NSString* MAT_AD_KEY = @"3fa11cf8822ff1d86461c44ac1ee09b2";

@interface Startup : NSObject
{
  UIImageView* imageView;
  NSTimer*     fadeOutTimer;
}

-(void)onStartup;

@end

@implementation Startup

-(void)dealloc
{
  [imageView release];

  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(void)onStartup
{
  UIWindow* keyWindow     = [UIApplication sharedApplication].keyWindow;
  UIView*   keyWindowView = keyWindow.rootViewController.view;
  UIView*   parentView;

  NSString* imageName;

  if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
  {
    imageName  = [UIScreen mainScreen].scale > 1 ? @"Default_ipad-Landscape@2x.png" : @"Default_ipad-Landscape.png";
    parentView = keyWindowView;
  }
  else
  {
    imageName  = [UIScreen mainScreen].scale > 1 ? @"Default-568h@2x.png" : @"Default.png";
    parentView = keyWindow;
  }

  CGRect keyWindowFrame = parentView.bounds;

  if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad && keyWindowFrame.size.width < keyWindowFrame.size.height)
  {
    float t = keyWindowFrame.size.width;

    keyWindowFrame.size.width  = keyWindowFrame.size.height;
    keyWindowFrame.size.height = t;
  }

  [imageView removeFromSuperview];
  [imageView release];
  imageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:imageName]];

  imageView.contentMode = UIViewContentModeScaleAspectFill;

  imageView.frame = keyWindowFrame;

  [parentView addSubview:imageView];

  [fadeOutTimer invalidate];
  [fadeOutTimer release];
  fadeOutTimer = [[NSTimer scheduledTimerWithTimeInterval:3 target:self selector:@selector (startFadeout) userInfo:nil repeats:NO] retain];
}

-(void)startFadeout
{
  [UIView beginAnimations:nil context:nil];
  [UIView setAnimationDuration:0.5];

  [UIView setAnimationDelegate:self];
  [UIView setAnimationDidStopSelector:@selector(onFadedOut)];

  imageView.alpha = 0;

  [UIView commitAnimations];
}

-(void)onFadedOut
{
  [imageView removeFromSuperview];
  [self release];
}

@end

@interface TrackingWrapper : NSObject
{
  Tracking* tracker;
}

@end

@implementation TrackingWrapper

-(void)dealloc
{
  [tracker release];

  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(void)reportAppOpen
{
  if (!tracker)
  {
    tracker = [[Tracking alloc] init];
    [tracker setURLScheme:UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? PAD_ADX_URL_SCHEME : PHONE_ADX_URL_SCHEME];
    [tracker setClientId:@"PLR7hjus768DP"];
    [tracker setAppleId:UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? PAD_APPLE_ID : PHONE_APPLE_ID];
  }

  [NSTimer scheduledTimerWithTimeInterval:0.5 target:tracker selector:@selector (reportAppOpen) userInfo:nil repeats:NO];
}

-(void)handleOpenURL:(const char*)url
{
  [tracker handleOpenURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

@end

@interface MobileAppTrackerWrapper : NSObject<MobileAppTrackerDelegate>
{
}

@end

@implementation MobileAppTrackerWrapper

-(void)dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

-(void)reportAppInstall
{
  NSError *error = nil;

  [[MobileAppTracker sharedManager] startTrackerWithAdvertiserId:MAT_AD_ID advertiserKey:MAT_AD_KEY withError:&error];

  if (error)
  {
    NSLog (@"Error while starting MobileAppTracker - '%@'", error);
    return;
  }

  [[MobileAppTracker sharedManager] setDelegate:self];

  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

  NSString* uuid = [defaults objectForKey:@"UUID"];

  if (!uuid)
  {
    if ([[[UIDevice currentDevice] systemVersion] compare:@"6.0" options:NSNumericSearch] != NSOrderedAscending)
    {
      uuid = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
    }
    else
    {
      CFUUIDRef cf_uuid = CFUUIDCreate (0);

      uuid = [(NSString*)CFUUIDCreateString (0, cf_uuid) autorelease];

      CFRelease (cf_uuid);

      [defaults setObject:uuid forKey:@"UUID"];
      [defaults synchronize];
    }
  }

  [[MobileAppTracker sharedManager] setDeviceId:uuid];

  NSArray*       paths        = NSSearchPathForDirectoriesInDomains (NSLibraryDirectory, NSUserDomainMask, YES);
  NSFileManager* file_manager = [NSFileManager defaultManager];
  NSString*      path         = [paths count] ? [paths objectAtIndex:0] : nil;

  bool mat_flag_exists = [file_manager fileExistsAtPath:[path stringByAppendingPathComponent:@"Plarium/mat_report_sent.flag"]];

  if ([file_manager fileExistsAtPath:[path stringByAppendingPathComponent:@"Plarium/Slots/Downloads_1"]] && !mat_flag_exists)
  {
    NSLog (@"MAT track update");
    [[MobileAppTracker sharedManager] trackUpdate];
  }
  else
  {
    NSLog (@"MAT track install");
    [[MobileAppTracker sharedManager] trackInstall];
  }

  if (!mat_flag_exists)
    [file_manager createFileAtPath:[path stringByAppendingPathComponent:@"Plarium/mat_report_sent.flag"] contents:nil attributes:nil];
}

/*!
 Delegate method called when a track action succeeds.
 @param tracker The MobileAppTracker instance.
 @param data The success data returned by the MobileAppTracker.
 */
- (void)mobileAppTracker:(MobileAppTracker *)tracker didSucceedWithData:(NSData *)data
{
  NSLog (@"MAT tracking succeeded '%@'", [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease]);
}

/*!
 Delegate method called when a track action fails.
 @param tracker The MobileAppTracker instance.
 @param error Error object returned by the MobileAppTracker.
 */
- (void)mobileAppTracker:(MobileAppTracker *)tracker didFailWithError:(NSError *)error
{
  NSLog (@"MAT tracking failed with error '%@'", error);
}

@end

@interface SystemAlertDelegate : NSObject
{
  @private
    sgi_stl::string  tag;
    engine::IEngine* engine;
}

@end

@implementation SystemAlertDelegate

-(id)initWithTag:(sgi_stl::string&)inTag engine:(engine::IEngine*)inEngine
{
  self = [super init];

  if (!self)
    return nil;

  tag    = inTag;
  engine = inEngine;

  return self;
}

-(void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
  engine->PostNotification (plarium::utility::format ("SystemAlertButtonClicked %s %d", tag.c_str (), buttonIndex).c_str ());

  [self release];
}

@end

namespace
{

class OpenUrlHandler : public IOpenUrlHandler
{
  public:
    OpenUrlHandler (TrackingWrapper* in_tracker)
      : tracker (in_tracker)
      {}

    void HandleUrlOpen (const char* url)
    {
      [tracker handleOpenURL:url];
    }

  private:
    OpenUrlHandler (const OpenUrlHandler&);             //no impl
    OpenUrlHandler& operator = (const OpenUrlHandler&); //no impl

  private:
    TrackingWrapper* tracker;
};

class SystemAlertHandler : public INotificationListener
{
  private:
    static const char* SYSTEM_ALERT_NOTIFICATION_PREFIX;

  public:
    ///Constructor/destructor
    SystemAlertHandler (engine::IEngine* in_engine)
      : engine (in_engine)
    {
      engine->AttachNotificationListener (plarium::utility::format ("%s*", SYSTEM_ALERT_NOTIFICATION_PREFIX).c_str (), this);
    }

    ~SystemAlertHandler ()
    {
      engine->DetachNotificationListener (this);
    }

    ///Notification handler
    void OnNotification (const char* notification)
    {
      notification += strlen (SYSTEM_ALERT_NOTIFICATION_PREFIX);

      sgi_stl::vector<sgi_stl::string> components = plarium::utility::split (notification, "|");

      if (components.size () < 4)
        throw sgi_stl::invalid_argument (plarium::utility::format ("SystemAlertHandler::OnNotification: invalid 'SystemAlert' arguments '%s'", notification));

      for (size_t i = 0, count = components.size (); i < count; i++)
        if (components [i] == " ")
          components [i] = "";

      SystemAlertDelegate* alert_delegate = [[SystemAlertDelegate alloc] initWithTag:components [0] engine:engine];

      UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:[NSString stringWithUTF8String:components [1].c_str ()]
                                                          message:[NSString stringWithUTF8String:components [2].c_str ()]
                                                         delegate:alert_delegate
                                                cancelButtonTitle:[NSString stringWithUTF8String:components [3].c_str ()]
                                                otherButtonTitles:nil];

      for (size_t i = 4, count = components.size (); i < count; i++)
        [alertView addButtonWithTitle:[NSString stringWithUTF8String:components [i].c_str ()]];

      [alertView show];
      [alertView release];
    }

  private:
    SystemAlertHandler (const SystemAlertHandler&);             //no impl
    SystemAlertHandler& operator = (const SystemAlertHandler&); //no impl

  private:
    engine::IEngine *engine;
};

const char* SystemAlertHandler::SYSTEM_ALERT_NOTIFICATION_PREFIX = "ShowSystemAlert ";

}

//точка входа
int main (int argc, const char* argv [], const char* env [])
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSBundle* main_bundle = [NSBundle mainBundle];

  if (!main_bundle)
  {
      printf ("Can't locate main bundle\n");
      return 1;
  }

  NSString* resources_path = [main_bundle resourcePath];

  if (![[NSFileManager defaultManager] changeCurrentDirectoryPath:resources_path])
  {
    NSLog (@"Can't set current dir '%@'\n", resources_path);
    [pool release];
    return 1;
  }

  [pool release];

  IEngine* funner = FunnerInit ();

  if (!funner)
  {
      printf ("Funner startup failed!");
      return 1;
  }

  if (!funner->ParseCommandLine (argc, argv, env))
      return 1;

  {
    pool = [[NSAutoreleasePool alloc] init];

    Startup* startup = [[Startup alloc] init];

    TrackingWrapper* tracking = [[TrackingWrapper alloc] init];

    MobileAppTrackerWrapper* mat_tracking = [[MobileAppTrackerWrapper alloc] init];

    [[NSNotificationCenter defaultCenter] addObserver:startup selector:@selector (onStartup) name:UIApplicationDidFinishLaunchingNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:tracking selector:@selector (reportAppOpen) name:UIApplicationDidFinishLaunchingNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:tracking selector:@selector (reportAppOpen) name:UIApplicationWillEnterForegroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:mat_tracking selector:@selector (reportAppInstall) name:UIApplicationDidFinishLaunchingNotification object:nil];

    SystemAlertHandler system_alert_handler (funner);

    OpenUrlHandler open_url_handler (tracking);

    Application application;

    application.SetOpenUrlHandler (&open_url_handler);

    application.Run (funner);

    [pool release];
  }

  delete funner;
}
