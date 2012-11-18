#include "shared.h"

using namespace social;
using namespace social::facebook;

/*
   Получение залогиненного пользователя
*/

User& FacebookSessionImpl::CurrentUser ()
{
  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception ("social::facebook::FacebookSessionImpl::CurrentUser", "User is not logged in yet");

  return current_user;
}

/*
   Загрузка пользователя по идентификатору
*/

void FacebookSessionImpl::LoadUser (const char* user_id, const LoadUserCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::LoadUser";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  if (!user_id)
    throw xtl::make_null_argument_exception (METHOD_NAME, "id");

  CheckUnknownProperties (METHOD_NAME, properties, 0, 0);

  //TODO
}

/*
   Аватар
*/

void FacebookSessionImpl::LoadUserPicture (const User& user, const LoadUserPictureCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::LoadUserPicture";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  if (!user.Handle ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load picture for user without setted handle");

  //TODO
}

/*
   Друзья
*/

void FacebookSessionImpl::LoadFriendsIds (const User& user, const LoadFriendsIdsCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::LoadFriendsIds";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  if (!user.Handle ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load friends ids for user without setted handle");

  //TODO
}

void FacebookSessionImpl::LoadFriends (const User& user, const LoadFriendsCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::LoadFriends";

  if (!IsUserLoggedIn ())
    throw xtl::format_operation_exception (METHOD_NAME, "User is not logged in yet");

  if (!user.Handle ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load friends ids for user without setted handle");

  //TODO
}
