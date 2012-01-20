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

namespace
{

void on_authentificated (const char* source, NSError* error, common::Log log, User current_user)
{
  try
  {
    GKLocalPlayer *local_player = [GKLocalPlayer localPlayer];

    if (error)
      log.Printf ("Game center authentification error '%s'", [[error description] UTF8String]);

    if (!local_player.isAuthenticated)
      return;

    Utility::Instance ()->FillUser (local_player, current_user);

    current_user.Properties ().SetProperty ("Underage", local_player.underage ? 1 : 0);
  }
  catch (xtl::exception& e)
  {
    log.Printf ("Exception in %s callback: '%s'", source, e.what ());
  }
  catch (...)
  {
    log.Printf ("Unknown exception in %s callback", source);
  }
}

}

/*
   Конструктор / Деструктор
*/

GameKitSessionImpl::GameKitSessionImpl ()
  : log (LOG_NAME)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::GameKitSessionImpl";

  GKLocalPlayer *local_player = [GKLocalPlayer localPlayer];

  [local_player authenticateWithCompletionHandler:^(NSError *error)
  {
    on_authentificated (METHOD_NAME, error, log, current_user);
  }];

  NSString* system_version = [[UIDevice currentDevice] systemVersion];

  system_version_5_0_available = [system_version compare:@"5.0" options:NSNumericSearch] != NSOrderedAscending;
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

  try
  {
    if (!window_name)
      throw xtl::make_null_argument_exception ("", "window_name");

    UIViewController* view_controller = nil;

    if (!xtl::xstrcmp ("Leaderboards", window_name))
    {
      CheckUnknownProperties (METHOD_NAME, properties, 0, 0);

      GKLeaderboardViewController* leaderboard_view_controller = [[GKLeaderboardViewController alloc] init];

      leaderboard_view_controller.leaderboardDelegate = [[DismissGameKitViewDelegate alloc] init];
      leaderboard_view_controller.category            = nil;

      view_controller = leaderboard_view_controller;
    }
    else if (!xtl::xstrcmp ("Leaderboard", window_name))
    {
      static const char* KNOWN_PROPERTIES [] = { "Id", "TimeScope" };

      CheckUnknownProperties (METHOD_NAME, properties, sizeof (KNOWN_PROPERTIES) / sizeof (*KNOWN_PROPERTIES), KNOWN_PROPERTIES);

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
            throw xtl::format_operation_exception ("", "Unknown time scope '%s'", time_scope);
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
      CheckUnknownProperties (METHOD_NAME, properties, 0, 0);

      GKAchievementViewController* achievement_view_controller = [[GKAchievementViewController alloc] init];

      achievement_view_controller.achievementDelegate = [[DismissGameKitViewDelegate alloc] init];

      view_controller = achievement_view_controller;
    }
    else
      throw xtl::make_argument_exception (METHOD_NAME, "window_name", window_name);

    [[UIApplication sharedApplication].keyWindow.rootViewController presentModalViewController:view_controller animated:YES];

    [view_controller release];
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
    throw;
  }
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
  if (!NSClassFromString (@"GKLocalPlayer"))
    return false;

  // The device must be running iOS 4.1 or later.
  if ([[[UIDevice currentDevice] systemVersion] compare:@"4.1" options:NSNumericSearch] == NSOrderedAscending)
    return false;

  return true;
}

/*
   Проверка наличия в PropertyMap неизвестных полей
*/

void GameKitSessionImpl::CheckUnknownProperties (const char* source, const common::PropertyMap& properties,
                                                 size_t known_properties_count, const char** known_properties_names)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::CheckUnknownProperties";

  if (!source)
    throw xtl::make_null_argument_exception (METHOD_NAME, "source");

  if (known_properties_count && !known_properties_names)
    throw xtl::make_null_argument_exception (METHOD_NAME, "known_properties_names");

  for (size_t i = 0, count = properties.Size (); i < count; i++)
  {
    const char* property_name = properties.PropertyName (i);

    bool property_known = false;

    for (size_t j = 0; j < known_properties_count; j++)
    {
      if (!xtl::xstrcmp (property_name, known_properties_names [j]))
      {
        property_known = true;
        break;
      }
    }

    if (!property_known)
      common::Log (LOG_NAME).Printf ("Unknown property '%s' at '%s'", property_name, source);
  }
}
