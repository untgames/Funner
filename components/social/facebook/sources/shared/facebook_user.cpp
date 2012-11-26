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

  if (xtl::xstrcmp (user.Id (), current_user.Id ()))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't load friends for not current user");

  static const char* SUPPORTED_PROPERTIES [] = { "fields" };

  CheckUnknownProperties (METHOD_NAME, properties, sizeof (SUPPORTED_PROPERTIES) / sizeof (*SUPPORTED_PROPERTIES), SUPPORTED_PROPERTIES);

  stl::string fields = "id,username";

  if (properties.IsPresent ("fields"))
  {
    const char* additional_fields = properties.GetString ("fields");

    if (xtl::xstrlen (additional_fields))
    {
      fields.append (",");
      fields.append (additional_fields);
    }
  }

  PerformRequest ("friends", common::format ("limit=9999&fields=%s", fields.c_str ()).c_str (), xtl::bind (&FacebookSessionImpl::OnFriendsInfoLoaded, this, _1, _2, _3, callback));
}

void FacebookSessionImpl::OnFriendsInfoLoaded (bool succeeded, const stl::string& status, common::ParseNode response, const LoadFriendsCallback& callback)
{
  try
  {
    log.Printf ("Friends load status '%s'", status.c_str ());

    UserList return_value;

    if (!succeeded)
    {
      callback (return_value, OperationStatus_Failure, status.c_str ());
      return;
    }

    common::ParseNode data = response.First ("data").First ();

    for (common::ParseNode iter = data.First (); iter; iter = iter.Next ())
    {
      User user;

      user.SetFriend (true);

      user.SetId       (iter.First ("id").Attribute (0));
      user.SetNickname (get_named_attribute (iter, "username", ""));

      const char *first_name = get_named_attribute (iter, "first_name", 0),
                 *last_name  = get_named_attribute (iter, "last_name", 0);

      common::PropertyMap& properties = user.Properties ();

      if (first_name)
        properties.SetProperty ("first_name", first_name);
      if (last_name)
        properties.SetProperty ("last_name", last_name);

      return_value.Add (user);
    }

    callback (return_value, OperationStatus_Success, status.c_str ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("social::facebook::FacebookSessionImpl::OnFriendsLoaded");
    throw;
  }
}
