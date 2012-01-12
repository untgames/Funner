#include "shared.h"

using namespace social;

namespace
{

const char* NOT_LOGGED_IN_SESSION_DESCRIPTION = "NotLoggedInSession";

}

/*
   Реализация сессии
*/

struct Session::Impl : public xtl::reference_counter
{
  stl::string      name;    //имя сессии
  ISessionManager* manager; //менеджер сессии

  Impl (const char* in_name)
    : manager (0)
  {
    if (!in_name)
      throw xtl::make_null_argument_exception ("social::Session::Session", "session_name");

    name = in_name;
  }

  ~Impl ()
  {
    LogOut ();
  }

  ///Логин
  void LogIn (const common::PropertyMap& config)
  {
    LogOut ();

    try
    {
      manager = SessionManagerSingleton::Instance ()->CreateSession (name.c_str (), config);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LogIn");
      throw;
    }
  }

  void LogOut ()
  {
    delete manager;

    manager = 0;
  }

  bool IsUserLoggedIn ()
  {
    return manager && manager->IsUserLoggedIn ();
  }

  ///Показ стандартных окон
  void ShowWindow (const char* window_name, const common::PropertyMap& properties)
  {
    try
    {
      if (!window_name)
        throw xtl::make_null_argument_exception ("", "window_name");

      if (!manager)
        throw xtl::format_operation_exception ("", "Not logged in");

      manager->ShowWindow (window_name, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::ShowWindow");
      throw;
    }
  }

  ///Получение залогиненного пользователя
  User& CurrentUser ()
  {
    try
    {
      if (!IsUserLoggedIn ())
        throw xtl::format_operation_exception ("", "User not logged in");

      return manager->CurrentUser ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::CurrentUser");
      throw;
    }
  }

  IUserManager* GetUserManager ()
  {
    if (!manager)
      throw xtl::format_operation_exception ("", "Not logged in");

    IUserManager* user_manager = dynamic_cast<IUserManager*> (manager);

    if (!user_manager)
      throw xtl::format_not_supported_exception ("", "operation not supported for session '%s'", manager->GetDescription ());

    return user_manager;
  }

  ///Загрузка пользователя по идентификатору
  void LoadUser (const char* id, const LoadUserCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetUserManager ()->LoadUser (id, callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LoadUser");
      throw;
    }
  }

  ///Аватар
  void LoadUserPicture (const User& user, const LoadUserPictureCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetUserManager ()->LoadUserPicture (user, callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LoadUserPicture");
      throw;
    }
  }

  ///Друзья
  void LoadFriendsIds (const User& user, const LoadFriendsIdsCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetUserManager ()->LoadFriendsIds (user, callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LoadFriendsIds");
      throw;
    }
  }

  void LoadFriends (const User& user, const LoadFriendsCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetUserManager ()->LoadFriends (user, callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LoadFriends");
      throw;
    }
  }

  ///Достижения
  IAchievementManager* GetAchievementManager ()
  {
    if (!manager)
      throw xtl::format_operation_exception ("", "Not logged in");

    IAchievementManager* achievement_manager = dynamic_cast<IAchievementManager*> (manager);

    if (!achievement_manager)
      throw xtl::format_not_supported_exception ("", "operation not supported for session '%s'", manager->GetDescription ());

    return achievement_manager;
  }

  void LoadAchievements (const LoadAchievementsCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetAchievementManager ()->LoadAchievements (callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LoadAchievements");
      throw;
    }
  }

  void LoadAchievementPicture (const Achievement& achievement, const LoadAchievementPictureCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetAchievementManager ()->LoadAchievementPicture (achievement, callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LoadAchievementPicture");
      throw;
    }
  }

  void SendAchievement (const Achievement& achievement, const SendAchievementCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetAchievementManager ()->SendAchievement (achievement, callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::SendAchievement");
      throw;
    }
  }

  ///Таблицы рекордов
  ILeaderboardManager* GetLeaderboardManager ()
  {
    if (!manager)
      throw xtl::format_operation_exception ("", "Not logged in");

    ILeaderboardManager* leaderboard_manager = dynamic_cast<ILeaderboardManager*> (manager);

    if (!leaderboard_manager)
      throw xtl::format_not_supported_exception ("", "operation not supported for session '%s'", manager->GetDescription ());

    return leaderboard_manager;
  }

  void LoadLeaderboardsIds (const LoadLeaderboardsIdsCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetLeaderboardManager ()->LoadLeaderboardsIds (callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LoadLeaderboardIds");
      throw;
    }
  }

  void LoadLeaderboard (const char* leaderboard_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetLeaderboardManager ()->LoadLeaderboard (leaderboard_id, callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LoadLeaderboard");
      throw;
    }
  }

  void LoadLeaderboard (const char* leaderboard_id, const char* user_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetLeaderboardManager ()->LoadLeaderboard (leaderboard_id, user_id, callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::LoadLeaderboard");
      throw;
    }
  }

  void SendScore (const Score& score, const SendScoreCallback& callback, const common::PropertyMap& properties)
  {
    try
    {
      GetLeaderboardManager ()->SendScore (score, callback, properties);
    }
    catch (xtl::exception& e)
    {
      e.touch ("social::Session::SendScore");
      throw;
    }
  }
};

/*
   Конструктор / деструктор / копирование
*/

Session::Session (const char* session_name)
  : impl (new Impl (session_name))
  {}

Session::Session (const Session& source)
  : impl (source.impl)
{
  addref (impl);
}

Session::~Session ()
{
  release (impl);
}

Session& Session::operator = (const Session& source)
{
  Session (source).Swap (*this);

  return *this;
}

/*
   Описание сессии
*/

const char* Session::Description () const
{
  if (!impl->manager)
    return NOT_LOGGED_IN_SESSION_DESCRIPTION;

  return impl->manager->GetDescription ();
}

/*
   Логин
*/

void Session::LogIn (const common::PropertyMap& config)
{
  impl->LogIn (config);
}

void Session::LogOut ()
{
  impl->LogOut ();
}

bool Session::IsUserLoggedIn () const
{
  return impl->IsUserLoggedIn ();
}

/*
   Показ стандартных окон
*/

void Session::ShowWindow (const char* window_name, const common::PropertyMap& properties) const
{
  impl->ShowWindow (window_name, properties);
}

/*
   Получение залогиненного пользователя
*/

User& Session::CurrentUser ()
{
  return impl->CurrentUser ();
}

const User& Session::CurrentUser () const
{
  return const_cast<Session&> (*this).CurrentUser ();
}

/*
   Загрузка пользователя по идентификатору
*/

void Session::LoadUser (const char* id, const LoadUserCallback& callback, const common::PropertyMap& properties) const
{
  impl->LoadUser (id, callback, properties);
}

/*
   Аватар
*/

void Session::LoadUserPicture (const User& user, const LoadUserPictureCallback& callback, const common::PropertyMap& properties) const
{
  impl->LoadUserPicture (user, callback, properties);
}

/*
   Друзья
*/

void Session::LoadFriendsIds (const User& user, const LoadFriendsIdsCallback& callback, const common::PropertyMap& properties) const
{
  impl->LoadFriendsIds (user, callback, properties);
}

void Session::LoadFriends (const User& user, const LoadFriendsCallback& callback, const common::PropertyMap& properties) const
{
  impl->LoadFriends (user, callback, properties);
}

/*
   Достижения
*/

void Session::LoadAchievements (const LoadAchievementsCallback& callback, const common::PropertyMap& properties) const
{
  impl->LoadAchievements (callback, properties);
}

void Session::LoadAchievementPicture (const Achievement& achievement, const LoadAchievementPictureCallback& callback, const common::PropertyMap& properties) const
{
  impl->LoadAchievementPicture (achievement, callback, properties);
}

void Session::SendAchievement (const Achievement& achievement, const SendAchievementCallback& callback, const common::PropertyMap& properties) const
{
  impl->SendAchievement (achievement, callback, properties);
}

/*
   Таблицы рекордов
*/

void Session::LoadLeaderboardsIds (const LoadLeaderboardsIdsCallback& callback, const common::PropertyMap& properties) const
{
  impl->LoadLeaderboardsIds (callback, properties);
}

void Session::LoadLeaderboard (const char* leaderboard_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties) const
{
  impl->LoadLeaderboard (leaderboard_id, callback, properties);
}

void Session::LoadLeaderboard (const char* leaderboard_id, const char* user_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties) const
{
  impl->LoadLeaderboard (leaderboard_id, user_id, callback, properties);
}

void Session::SendScore (const Score& score, const SendScoreCallback& callback, const common::PropertyMap& properties) const
{
  impl->SendScore (score, callback, properties);
}

/*
   Обмен
*/

void Session::Swap (Session& source)
{
  stl::swap (impl, source.impl);
}

namespace social
{

/*
   Обмен
*/

void swap (Session& session1, Session& session2)
{
  session1.Swap (session2);
}

}
