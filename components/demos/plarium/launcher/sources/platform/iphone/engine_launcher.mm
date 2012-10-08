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

#include <cstdio>

#include <launcher/application.h>

using namespace engine;
using namespace plarium::launcher;

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
  fadeOutTimer = [[NSTimer scheduledTimerWithTimeInterval:2 target:self selector:@selector (startFadeout) userInfo:nil repeats:NO] retain];
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
    [tracker setURLScheme:@"ADX1144"];
    [tracker setClientId:@"PLR7hjus768DP"];
    [tracker setAppleId:@"543831789"];
  }

  [tracker reportAppOpen];
}

-(void)handleOpenURL:(const char*)url
{
  [tracker handleOpenURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
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

    [[NSNotificationCenter defaultCenter] addObserver:startup selector:@selector (onStartup) name:UIApplicationDidFinishLaunchingNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:tracking selector:@selector (reportAppOpen) name:UIApplicationDidFinishLaunchingNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:tracking selector:@selector (reportAppOpen) name:UIApplicationWillEnterForegroundNotification object:nil];

    OpenUrlHandler open_url_handler (tracking);

    Application application;

    application.SetOpenUrlHandler (&open_url_handler);

    application.Run (funner);

    [pool release];
  }

  delete funner;
}
