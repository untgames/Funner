#import <UIKit/UIKit.h>

#import <GameKit/GKLocalPlayer.h>

#include "shared.h"

using namespace social;
using namespace social::game_kit;

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

void GameKitSessionImpl::LoadUser (const char* id, const LoadUserCallback& callback, const common::PropertyMap& properties)
{

}

/*
   Аватар
*/

void GameKitSessionImpl::LoadUserPicture (const User& user, const LoadUserPictureCallback& callback, const common::PropertyMap& properties)
{

}

/*
   Друзья
*/

void GameKitSessionImpl::LoadFriendsIds (const User& user, const LoadFriendsIdsCallback& callback, const common::PropertyMap& properties)
{

}

void GameKitSessionImpl::LoadFriends (const User& user, const LoadFriendsCallback& callback, const common::PropertyMap& properties)
{

}
