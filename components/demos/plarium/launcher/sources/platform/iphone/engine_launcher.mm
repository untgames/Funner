#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

#import <UIKit/UIApplication.h>
#import <UIKit/UIImage.h>
#import <UIKit/UIImageView.h>
#import <UIKit/UIViewController.h>
#import <UIKit/UIScreen.h>
#import <UIKit/UIWindow.h>

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

-(id)init
{
  self = [super init];

  if (!self)
    return nil;

  return self;
}

-(void)onStartup
{
  UIWindow* keyWindow     = [UIApplication sharedApplication].keyWindow;
  UIView*   keyWindowView = keyWindow.rootViewController.view;
  UIView*   parentView;

  NSString* imageName;

  if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
  {
    imageName  = @"Default_ipad-Landscape@2x.png";
    parentView = keyWindowView;
  }
  else
  {
    imageName  = [UIScreen mainScreen].scale > 1 ? @"Default-568h@2x.png" : @"Default.png";
    parentView = keyWindow;
  }

  CGRect keyWindowFrame = parentView.bounds;

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

    [[NSNotificationCenter defaultCenter] addObserver:startup selector:@selector (onStartup) name:UIApplicationDidFinishLaunchingNotification object:nil];

    Application application;

    application.Run (funner);

    [pool release];
  }

  delete funner;
}
