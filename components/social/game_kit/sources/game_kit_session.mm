#import <UIKit/UIKit.h>

#import <GameKit/GKAchievementViewController.h>
#import <GameKit/GKLeaderboardViewController.h>
#import <GameKit/GKLocalPlayer.h>

#include "shared.h"

using namespace social;
using namespace social::game_kit;

namespace
{

const char* LOG_NAME            = "social.game_kit.GameKitSession";
const char* SESSION_DESCRIPTION = "GameKit";

}

@interface DismissGameKitViewDelegate : NSObject <GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate>
{
}

@end

@implementation DismissGameKitViewDelegate

-(void)dismiss
{
  [[UIApplication sharedApplication].keyWindow.rootViewController dismissModalViewControllerAnimated:NO];

  [self release];
}

-(void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController*)viewController
{
  viewController.leaderboardDelegate = nil;

  [self dismiss];
}

-(void)achievementViewControllerDidFinish:(GKAchievementViewController*)viewController
{
  viewController.achievementDelegate = nil;

  [self dismiss];
}

@end

/*
   Конструктор / Деструктор
*/

GameKitSessionImpl::GameKitSessionImpl ()
  : log (LOG_NAME)
{
  GKLocalPlayer* local_player = [GKLocalPlayer localPlayer];

  [local_player authenticateWithCompletionHandler:^(NSError *error) {
    if (error)
      log.Printf ("Game center authentification error '%s'", [[error description] UTF8String]);

    if (!local_player.isAuthenticated)
      return;
  }];
}

GameKitSessionImpl::~GameKitSessionImpl ()
{
}

/*
   Описание сессии
*/

const char* GameKitSessionImpl::GetDescription ()
{
  return SESSION_DESCRIPTION;
}

/*
   Показ стандартных окон
*/

void GameKitSessionImpl::ShowWindow (const char* window_name, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::ShowWindow";

  if (!window_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "window_name");

  UIViewController* view_controller = nil;

  if (!xtl::xstrcmp ("Leaderboards", window_name))
  {
    GKLeaderboardViewController* leaderboard_view_controller = [[GKLeaderboardViewController alloc] init];

    leaderboard_view_controller.leaderboardDelegate = [[DismissGameKitViewDelegate alloc] init];
    leaderboard_view_controller.category            = nil;

    view_controller = leaderboard_view_controller;
  }
  else if (!xtl::xstrcmp ("Leaderboard", window_name))
  {
    GKLeaderboardViewController* leaderboard_view_controller = [[GKLeaderboardViewController alloc] init];
    DismissGameKitViewDelegate*  dismiss_delegate            = [[DismissGameKitViewDelegate alloc] init];

    leaderboard_view_controller.leaderboardDelegate = dismiss_delegate;

    try
    {
      if (properties.IsPresent ("Id"))
      {
        const char* category_id = properties.GetString ("Id");

        NSString* ns_category_id = [[NSString alloc] initWithUTF8String:category_id];

        leaderboard_view_controller.category = ns_category_id;

        [ns_category_id release];
      }

      if (properties.IsPresent ("TimeScope"))
      {
        const char* time_scope = properties.GetString ("TimeScope");

        if (!xtl::xstrcmp ("Today", time_scope))
          leaderboard_view_controller.timeScope = GKLeaderboardTimeScopeToday;
        else if (!xtl::xstrcmp ("Week", time_scope))
          leaderboard_view_controller.timeScope = GKLeaderboardTimeScopeWeek;
        else if (!xtl::xstrcmp ("AllTime", time_scope))
          leaderboard_view_controller.timeScope = GKLeaderboardTimeScopeAllTime;
        else
          throw xtl::format_operation_exception (METHOD_NAME, "Unknown time scope '%s'", time_scope);
      }
    }
    catch (...)
    {
      [dismiss_delegate            release];
      [leaderboard_view_controller release];
      throw;
    }

    view_controller = leaderboard_view_controller;
  }
  else if (!xtl::xstrcmp ("Achievements", window_name))
  {
    GKAchievementViewController* achievement_view_controller = [[GKAchievementViewController alloc] init];

    achievement_view_controller.achievementDelegate = [[DismissGameKitViewDelegate alloc] init];

    view_controller = achievement_view_controller;
  }
  else
    throw xtl::make_argument_exception (METHOD_NAME, "window_name", window_name);

  [[UIApplication sharedApplication].keyWindow.rootViewController presentModalViewController:view_controller animated:YES];

  [view_controller release];
}

/*
   Завершился ли процесс логина
*/

bool GameKitSessionImpl::IsUserLoggedIn ()
{
  return [GKLocalPlayer localPlayer].authenticated;
}

/*
   Проверка наличия поддержки Game Kit API
*/

bool GameKitSessionImpl::IsApiAvailable ()
{
  // Check for presence of GKLocalPlayer class.
  if (!NSClassFromString (@"GKLocalPlayer"));
    return false;

  // The device must be running iOS 4.1 or later.
  if ([[[UIDevice currentDevice] systemVersion] compare:@"4.1" options:NSNumericSearch] == NSOrderedAscending)
    return false;

  return true;
}
