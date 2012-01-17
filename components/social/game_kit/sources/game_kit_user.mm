#include "shared.h"

using namespace social;
using namespace social::game_kit;

namespace
{

void on_user_loaded (const char* source, NSString* user_id, NSArray *players, NSError *error, const common::Log& log, const LoadUserCallback& callback)
{
  try
  {
    if (error)
    {
      const char* error_string = [[error description] UTF8String];

      log.Printf ("%s error '%s'", source, error_string);
      callback (User (), OperationStatus_Failure, error_string);
      return;
    }

    GKPlayer* player = nil;

    for (GKPlayer* current_player in players)
    {
      if ([current_player.playerID isEqualToString:user_id])
      {
        player = current_player;
        break;
      }
    }

    if (!player)
    {
      static const char* ERROR = "requested player not loaded";

      log.Printf ("%s error '%s'", source, ERROR);
      callback (User (), OperationStatus_Failure, ERROR);
      return;
    }

    User user;

    fill_user (player, user);

    callback (user, OperationStatus_Success, OK_STATUS);
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

void on_user_picture_loaded (const char* source, UIImage* picture, NSError *error, const common::Log& log, const LoadUserPictureCallback& callback)
{
  try
  {
    if (error)
    {
      const char* error_string = [[error description] UTF8String];

      log.Printf ("%s error '%s'", source, error_string);
      callback (media::Image (), OperationStatus_Failure, error_string);
      return;
    }

    if (!picture)
    {
      static const char* ERROR = "requested picture not loaded";

      log.Printf ("%s error '%s'", source, ERROR);
      callback (media::Image (), OperationStatus_Failure, ERROR);
      return;
    }

    callback (convert_image (picture), OperationStatus_Success, OK_STATUS);
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

void on_user_friends_ids_loaded (const char* source, NSArray* friends, NSError* error, const common::Log& log, const LoadFriendsIdsCallback& callback)
{
  try
  {
    if (error)
    {
      const char* error_string = [[error description] UTF8String];

      log.Printf ("%s error '%s'", source, error_string);
      callback (common::StringArray (), OperationStatus_Failure, error_string);
      return;
    }

    common::StringArray friends_ids;

    for (NSString* friend_id in friends)
      friends_ids.Add ([friend_id UTF8String]);

    callback (friends_ids, OperationStatus_Success, OK_STATUS);
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

void load_users (const char* source, NSArray* friends, NSError* error, const common::Log& log, const LoadFriendsCallback& callback)
{
  try
  {
    if (error)
    {
      const char* error_string = [[error description] UTF8String];

      log.Printf ("%s error '%s'", source, error_string);
      callback (UserList (), OperationStatus_Failure, error_string);
      return;
    }

    [GKPlayer loadPlayersForIdentifiers:friends withCompletionHandler:^(NSArray *players, NSError *error)
    {
      if (error)
      {
        const char* error_string = [[error description] UTF8String];

        log.Printf ("%s error '%s'", source, error_string);
        callback (UserList (), OperationStatus_Failure, error_string);
        return;
      }

      UserList friends_list;

      friends_list.Reserve ([players count]);

      for (GKPlayer* player in players)
      {
        User user;

        fill_user (player, user);

        friends_list.Add (user);
      }

      callback (friends_list, OperationStatus_Success, OK_STATUS);
    }];
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
   Получение залогиненного пользователя
*/

User& GameKitSessionImpl::CurrentUser ()
{
  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception ("social::game_kit::GameKitSessionImpl::CurrentUser", "User is not logged in yet");

  return current_user;
}

/*
   Загрузка пользователя по идентификатору
*/

void GameKitSessionImpl::LoadUser (const char* user_id, const LoadUserCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::LoadUser";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  if (!user_id)
    throw xtl::make_null_argument_exception (METHOD_NAME, "id");

  CheckUnknownProperties (METHOD_NAME, properties, 0, 0);

  NSString* ns_id = [[NSString alloc] initWithUTF8String:user_id];

  [GKPlayer loadPlayersForIdentifiers:[NSArray arrayWithObject:ns_id] withCompletionHandler:^(NSArray *players, NSError *error)
  {
    on_user_loaded (METHOD_NAME, ns_id, players, error, log, callback);
  }];

  [ns_id release];
}

/*
   Аватар
*/

void GameKitSessionImpl::LoadUserPicture (const User& user, const LoadUserPictureCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::LoadUserPicture";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  if ([system_version compare:@"5.0" options:NSNumericSearch] == NSOrderedAscending)
    throw xtl::format_not_supported_exception (METHOD_NAME, "iOS version '%@' does not support user picture loading", [system_version UTF8String]);

  if (!user.Handle ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load picture for user without setted handle");

  static const char* KNOWN_PROPERTIES [] = { "SIZE" };

  CheckUnknownProperties (METHOD_NAME, properties, sizeof (KNOWN_PROPERTIES) / sizeof (*KNOWN_PROPERTIES), KNOWN_PROPERTIES);

  GKPhotoSize photo_size = GKPhotoSizeNormal;

  [(GKPlayer*)user.Handle () loadPhotoForSize:photo_size withCompletionHandler:^(UIImage *photo, NSError *error)
  {
    on_user_picture_loaded (METHOD_NAME, photo, error, log, callback);
  }];
}

/*
   Друзья
*/

void GameKitSessionImpl::LoadFriendsIds (const User& user, const LoadFriendsIdsCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::LoadFriendsIds";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  if (!user.Handle ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load friends ids for user without setted handle");

  if (![(id)user.Handle () isKindOfClass:[GKLocalPlayer class]])
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load friends for user other than logged user");

  CheckUnknownProperties (METHOD_NAME, properties, 0, 0);

  [(GKLocalPlayer*)user.Handle () loadFriendsWithCompletionHandler:^(NSArray *friends, NSError *error)
  {
    on_user_friends_ids_loaded (METHOD_NAME, friends, error, log, callback);
  }];
}

void GameKitSessionImpl::LoadFriends (const User& user, const LoadFriendsCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::game_kit::GameKitSessionImpl::LoadFriends";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  if (!user.Handle ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load friends ids for user without setted handle");

  if (![(id)user.Handle () isKindOfClass:[GKLocalPlayer class]])
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load friends for user other than logged user");

  [(GKLocalPlayer*)user.Handle () loadFriendsWithCompletionHandler:^(NSArray *friends, NSError *error)
  {
    load_users (METHOD_NAME, friends, error, log, callback);
  }];
}
