#include "shared.h"

using namespace social;
using namespace social::game_kit;

namespace
{

void on_categories_loaded (const char* source, NSArray *categories, NSArray *titles, NSError *error, const common::Log& log, const LoadLeaderboardsCallback& callback)
{
  try
  {
    if (error)
    {
      const char* error_string = [[error description] UTF8String];

      log.Printf ("%s error '%s'", source, error_string);
      callback (LeaderboardList (), OperationStatus_Failure, error_string);
      return;
    }

    if ([categories count] != [titles count])
    {
      static const char* ERROR = "categories count not equals to titles count";

      log.Printf ("%s error '%s'", source, ERROR);
      callback (LeaderboardList (), OperationStatus_Failure, ERROR);
      return;
    }

    LeaderboardList leaderboards;

    leaderboards.Reserve ([categories count]);

    for (size_t i = 0, count = [categories count]; i < count; i++)
    {
      NSString *category_id = [categories objectAtIndex:i],
               *title       = [titles objectAtIndex:i];

      Leaderboard leaderboard;

      leaderboard.SetId    ([category_id UTF8String]);
      leaderboard.SetTitle ([title UTF8String]);

      leaderboards.Add (leaderboard);
    }

    callback (leaderboards, OperationStatus_Success, OK_STATUS);
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

void on_scores_loaded (const char* source, GKLeaderboard *ns_leaderboard, NSError *error, const common::Log& log, const LoadLeaderboardCallback& callback)
{
  try
  {
    if (error)
    {
      const char* error_string = [[error description] UTF8String];

      log.Printf ("%s error '%s'", source, error_string);
      callback (Leaderboard (), OperationStatus_Failure, error_string);
      return;
    }

    Leaderboard leaderboard;

    Utility::Instance ()->FillLeaderboard (ns_leaderboard, leaderboard);

    callback (leaderboard, OperationStatus_Success, OK_STATUS);
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
   Таблицы рекордов
*/

void GameKitSessionImpl::LoadLeaderboards (const LoadLeaderboardsCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::LoadLeaderboardsIds";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  CheckUnknownProperties (METHOD_NAME, properties, 0, 0);

  [GKLeaderboard loadCategoriesWithCompletionHandler:^(NSArray *categories, NSArray *titles, NSError *error)
  {
    on_categories_loaded (METHOD_NAME, categories, titles, error, log, callback);
  }];
}

void GameKitSessionImpl::LoadLeaderboard (const char* leaderboard_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::LoadLeaderboard";

  try
  {
    if (!IsUserLoggedIn ())
      throw xtl::format_operation_exception ("", "User is not logged in yet");

    static const char* KNOWN_PROPERTIES [] = { "PlayerScope", "RangeBegin", "RangeLength", "TimeScope" };

    CheckUnknownProperties (METHOD_NAME, properties, sizeof (KNOWN_PROPERTIES) / sizeof (*KNOWN_PROPERTIES), KNOWN_PROPERTIES);

    GKLeaderboard* leaderboard = [[GKLeaderboard alloc] init];

    NSString* ns_leaderboard_id = [[NSString alloc] initWithUTF8String:leaderboard_id];

    leaderboard.category = ns_leaderboard_id;

    [ns_leaderboard_id release];

    try
    {
      if (properties.IsPresent ("PlayerScope"))
      {
        const char* player_scope = properties.GetString ("PlayerScope");

        if (!xtl::xstrcmp (player_scope, "Global"))
          leaderboard.playerScope = GKLeaderboardPlayerScopeGlobal;
        else if (!xtl::xstrcmp (player_scope, "FriendsOnly"))
          leaderboard.playerScope = GKLeaderboardPlayerScopeFriendsOnly;
        else
          throw xtl::format_operation_exception ("", "Unknown player scope '%s'", player_scope);
      }

      if (properties.IsPresent ("RangeBegin") ^ properties.IsPresent ("RangeLength"))
        throw xtl::format_operation_exception ("", "Both of properties 'RangeBegin' and 'RangeEnd' must be present or none of them");

      if (properties.IsPresent ("RangeBegin") && properties.IsPresent ("RangeLength"))
      {
        NSRange range = { properties.GetInteger ("RangeBegin"), properties.GetInteger ("RangeLength") };

        if (range.location < 1)
          throw xtl::format_operation_exception ("", "RangeBegin must be greater than 0");
        if (range.length < 1 || range.length > 100)
          throw xtl::make_range_exception ("", "RangeLength", range.length, 1, 100);

        leaderboard.range = range;
      }

      if (properties.IsPresent ("TimeScope"))
      {
        const char* time_scope = properties.GetString ("TimeScope");

        if (!xtl::xstrcmp (time_scope, "Today"))
          leaderboard.timeScope = GKLeaderboardTimeScopeToday;
        if (!xtl::xstrcmp (time_scope, "Week"))
          leaderboard.timeScope = GKLeaderboardTimeScopeWeek;
        if (!xtl::xstrcmp (time_scope, "AllTime"))
          leaderboard.timeScope = GKLeaderboardTimeScopeAllTime;
        else
          throw xtl::format_operation_exception ("", "Unknown time scope '%s'", time_scope);
      }
    }
    catch (...)
    {
      [leaderboard release];
      throw;
    }

    [leaderboard loadScoresWithCompletionHandler:^(NSArray *scores, NSError *error)
    {
      on_scores_loaded (METHOD_NAME, leaderboard, error, log, callback);
    }];

    [leaderboard release];
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
    throw;
  }
}

void GameKitSessionImpl::LoadLeaderboard (const char* leaderboard_id, const char* user_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties)
{

}

/*
   Публикация
*/

void GameKitSessionImpl::SendScore (const Score& score, const SendScoreCallback& callback, const common::PropertyMap& properties)
{

}

