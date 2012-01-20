#include "shared.h"

using namespace script;
using namespace social;

namespace
{

/*
    Константы
*/

const char* OPERATION_STATUS_LIBRARY     = "Social.OperationStatus";
const char* ACHIEVEMENT_LIBRARY          = "Social.Achievement";
const char* ACHIEVEMENT_LIST_LIBRARY     = "Social.AchievementList";
const char* LEADERBOARD_LIBRARY          = "Social.Leaderboard";
const char* LEADERBOARD_LIST_LIBRARY     = "Social.LeaderboardList";
const char* SCORE_LIBRARY                = "Social.Score";
const char* SCORE_LIST_LIBRARY           = "Social.ScoreList";
const char* USER_LIBRARY                 = "Social.User";
const char* USER_LIST_LIBRARY            = "Social.UserList";
const char* SESSION_LIBRARY              = "Social.Session";
const char* COMPONENT_NAME               = "script.binds.Social";
const char* BINDER_NAME                  = "Social";

template<class T>
const T& get_collection_item (const Collection<T>& collection, size_t index)
{
  return collection [index];
}

template<class T>
Collection<T> create_collection ()
{
  return Collection<T> ();
}

template<class T>
void bind_collection (Environment& environment, const char* library_name)
{
  InvokerRegistry lib = environment.Library (library_name);

  lib.Register ("Create",       make_invoker (&create_collection<T>));
  lib.Register ("get_Size",     make_invoker (&Collection<T>::Size));
  lib.Register ("get_IsEmpty",  make_invoker (&Collection<T>::IsEmpty));
  lib.Register ("get_Capacity", make_invoker (&Collection<T>::Capacity));

  lib.Register ("Get",     make_invoker (&get_collection_item<T>));
  lib.Register ("Reserve", make_invoker (&Collection<T>::Reserve));
  lib.Register ("Add",     make_invoker (&Collection<T>::Add));
  lib.Register ("Remove",  make_invoker (&Collection<T>::Remove));
  lib.Register ("Clear",   make_invoker (&Collection<T>::Clear));

  environment.RegisterType<Collection<T> > (library_name);
}

void bind_operation_status_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (OPERATION_STATUS_LIBRARY);

  lib.Register ("get_Success", make_const (OperationStatus_Success));
  lib.Register ("get_Failure", make_const (OperationStatus_Failure));
}

Achievement create_achievement ()
{
  return Achievement ();
}

void bind_achievement_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (ACHIEVEMENT_LIBRARY);

  lib.Register ("Create",         make_invoker (&create_achievement));
  lib.Register ("get_Id",         make_invoker (&Achievement::Id));
  lib.Register ("get_Title",      make_invoker (&Achievement::Title));
  lib.Register ("get_IsHidden",   make_invoker (&Achievement::IsHidden));
  lib.Register ("get_Progress",   make_invoker (&Achievement::Progress));
  lib.Register ("get_Properties", make_invoker (xtl::implicit_cast<const common::PropertyMap& (Achievement::*) () const> (&Achievement::Properties)));
  lib.Register ("set_Id",         make_invoker (&Achievement::SetId));
  lib.Register ("set_Title",      make_invoker (&Achievement::SetTitle));
  lib.Register ("set_IsHidden",   make_invoker (&Achievement::SetHidden));
  lib.Register ("set_Progress",   make_invoker (&Achievement::SetProgress));
  lib.Register ("set_Properties", make_invoker (&Achievement::SetProperties));

  environment.RegisterType<Achievement> (ACHIEVEMENT_LIBRARY);
}

Score create_score ()
{
  return Score ();
}

void bind_score_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SCORE_LIBRARY);

  lib.Register ("Create",             make_invoker (&create_score));
  lib.Register ("get_UserId",         make_invoker (&Score::UserId));
  lib.Register ("get_LeaderboardId",  make_invoker (&Score::LeaderboardId));
  lib.Register ("get_UserData",       make_invoker (&Score::UserData));
  lib.Register ("get_Value",          make_invoker (&Score::Value));
  lib.Register ("get_FormattedValue", make_invoker (&Score::FormattedValue));
  lib.Register ("get_Rank",           make_invoker (&Score::Rank));
  lib.Register ("get_Properties",     make_invoker (xtl::implicit_cast<const common::PropertyMap& (Score::*) () const> (&Score::Properties)));
  lib.Register ("set_UserId",         make_invoker (&Score::SetUserId));
  lib.Register ("set_LeaderboardId",  make_invoker (&Score::SetLeaderboardId));
  lib.Register ("set_UserData",       make_invoker (&Score::SetUserData));
  lib.Register ("set_Value",          make_invoker (&Score::SetValue));
  lib.Register ("set_FormattedValue", make_invoker (&Score::SetFormattedValue));
  lib.Register ("set_Rank",           make_invoker (&Score::SetRank));
  lib.Register ("set_Properties",     make_invoker (&Score::SetProperties));

  environment.RegisterType<Score> (SCORE_LIBRARY);
}

Leaderboard create_leaderboard ()
{
  return Leaderboard ();
}

void bind_leaderboard_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (LEADERBOARD_LIBRARY);

  lib.Register ("Create",         make_invoker (&create_leaderboard));
  lib.Register ("get_Id",         make_invoker (&Leaderboard::Id));
  lib.Register ("get_Title",      make_invoker (&Leaderboard::Title));
  lib.Register ("get_UserScore",  make_invoker (&Leaderboard::UserScore));
  lib.Register ("get_Scores",     make_invoker (xtl::implicit_cast<const ScoreList& (Leaderboard::*) () const> (&Leaderboard::Scores)));
  lib.Register ("get_Properties", make_invoker (xtl::implicit_cast<const common::PropertyMap& (Leaderboard::*) () const> (&Leaderboard::Properties)));
  lib.Register ("set_Id",         make_invoker (&Leaderboard::SetId));
  lib.Register ("set_Title",      make_invoker (&Leaderboard::SetTitle));
  lib.Register ("set_UserScore",  make_invoker (&Leaderboard::SetUserScore));
  lib.Register ("set_Scores",     make_invoker (&Leaderboard::SetScores));
  lib.Register ("set_Properties", make_invoker (&Leaderboard::SetProperties));

  environment.RegisterType<Leaderboard> (LEADERBOARD_LIBRARY);
}

User create_user ()
{
  return User ();
}

void bind_user_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (USER_LIBRARY);

  lib.Register ("Create",         make_invoker (&create_user));
  lib.Register ("get_Id",         make_invoker (&User::Id));
  lib.Register ("get_Nickname",   make_invoker (&User::Nickname));
  lib.Register ("get_IsFriend",   make_invoker (&User::IsFriend));
  lib.Register ("get_Properties", make_invoker (xtl::implicit_cast<const common::PropertyMap& (User::*) () const> (&User::Properties)));
  lib.Register ("set_Id",         make_invoker (&User::SetId));
  lib.Register ("set_Nickname",   make_invoker (&User::SetNickname));
  lib.Register ("set_IsFriend",   make_invoker (&User::SetFriend));
  lib.Register ("set_Properties", make_invoker (&User::SetProperties));

  environment.RegisterType<User> (USER_LIBRARY);
}

Session create_session (const char* session_name)
{
  return Session (session_name);
}

typedef xtl::function<void (const xtl::any&, OperationStatus, const char* error)> LoadCallback;

void on_user_loaded (const User& user, OperationStatus status, const char* error, const LoadCallback& callback)
{
  if (callback)
    callback (xtl::any (user, true), status, error);
}

void load_user (Session& session, const char* id, const LoadCallback& callback, const common::PropertyMap& properties = common::PropertyMap ())
{
  session.LoadUser (id, xtl::bind (&on_user_loaded, _1, _2, _3, callback), properties);
}

void on_friends_loaded (const UserList& friends, OperationStatus status, const char* error, const LoadCallback& callback)
{
  if (callback)
    callback (xtl::any (friends, true), status, error);
}

void load_friends (Session& session, const User& user, const LoadCallback& callback, const common::PropertyMap& properties = common::PropertyMap ())
{
  session.LoadFriends (user, xtl::bind (&on_friends_loaded, _1, _2, _3, callback), properties);
}

void on_achievements_loaded (const AchievementList& achievements, OperationStatus status, const char* error, const LoadCallback& callback)
{
  if (callback)
    callback (xtl::any (achievements, true), status, error);
}

void load_achievements (Session& session, const LoadCallback& callback, const common::PropertyMap& properties = common::PropertyMap ())
{
  session.LoadAchievements (xtl::bind (&on_achievements_loaded, _1, _2, _3, callback), properties);
}

void on_achievement_sent (OperationStatus status, const char* error, const LoadCallback& callback)
{
  if (callback)
    callback (xtl::any (), status, error);
}

void send_achievement (Session& session, const Achievement& achievement, const LoadCallback& callback, const common::PropertyMap& properties = common::PropertyMap ())
{
  session.SendAchievement (achievement, xtl::bind (&on_achievement_sent, _1, _2, callback), properties);
}

void on_leaderboards_loaded (const LeaderboardList& leaderboards, OperationStatus status, const char* error, const LoadCallback& callback)
{
  if (callback)
    callback (xtl::any (leaderboards, true), status, error);
}

void load_leaderboards (Session& session, const LoadCallback& callback, const common::PropertyMap& properties = common::PropertyMap ())
{
  session.LoadLeaderboards (xtl::bind (&on_leaderboards_loaded, _1, _2, _3, callback), properties);
}

void on_leaderboard_loaded (const Leaderboard& leaderboard, OperationStatus status, const char* error, const LoadCallback& callback)
{
  if (callback)
    callback (xtl::any (leaderboard, true), status, error);
}

void load_user_leaderboard (Session& session, const char* leaderboard_id, const char* user_id, const LoadCallback& callback, const common::PropertyMap& properties = common::PropertyMap ())
{
  session.LoadLeaderboard (leaderboard_id, user_id, xtl::bind (&on_leaderboard_loaded, _1, _2, _3, callback), properties);
}

void load_leaderboard (Session& session, const char* leaderboard_id, const LoadCallback& callback, const common::PropertyMap& properties = common::PropertyMap ())
{
  session.LoadLeaderboard (leaderboard_id, xtl::bind (&on_leaderboard_loaded, _1, _2, _3, callback), properties);
}

void on_score_sent (OperationStatus status, const char* error, const LoadCallback& callback)
{
  if (callback)
    callback (xtl::any (), status, error);
}

void send_score (Session& session, const Score& score, const LoadCallback& callback, const common::PropertyMap& properties = common::PropertyMap ())
{
  session.SendScore (score, xtl::bind (&on_score_sent, _1, _2, callback), properties);
}

void bind_session_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (SESSION_LIBRARY);

  lib.Register ("Create",             make_invoker (&create_session));
  lib.Register ("get_Description",    make_invoker (&Session::Description));
  lib.Register ("get_CurrentUser",    make_invoker (xtl::implicit_cast<const User& (Session::*) () const> (&Session::CurrentUser)));
  lib.Register ("get_IsUserLoggedIn", make_invoker (&Session::IsUserLoggedIn));
  lib.Register ("LogIn",              make_invoker (&Session::LogIn));
  lib.Register ("LogOut",             make_invoker (&Session::LogOut));
  lib.Register ("ShowWindow",         make_invoker (
      make_invoker (&Session::ShowWindow),
      make_invoker<void (Session&, const char*)> (xtl::bind (&Session::ShowWindow, _1, _2, common::PropertyMap ()))
  ));
  lib.Register ("CreateLoadCallback", make_callback_invoker<LoadCallback::signature_type> ());
  lib.Register ("LoadUser",           make_invoker (
      make_invoker (&load_user),
      make_invoker<void (Session&, const char*, const LoadCallback&)> (xtl::bind (&load_user, _1, _2, _3, common::PropertyMap ())),
      make_invoker<void (Session&, const char*)> (xtl::bind (&load_user, _1, _2, LoadCallback (), common::PropertyMap ()))
  ));
  lib.Register ("LoadFriends",        make_invoker (
      make_invoker (&load_friends),
      make_invoker<void (Session&, const User&, const LoadCallback&)> (xtl::bind (&load_friends, _1, _2, _3, common::PropertyMap ())),
      make_invoker<void (Session&, const User&)> (xtl::bind (&load_friends, _1, _2, LoadCallback (), common::PropertyMap ()))
  ));
  lib.Register ("LoadAchievements",   make_invoker (
      make_invoker (&load_achievements),
      make_invoker<void (Session&, const LoadCallback&)> (xtl::bind (&load_achievements, _1, _2, common::PropertyMap ())),
      make_invoker<void (Session&)> (xtl::bind (&load_achievements, _1, LoadCallback (), common::PropertyMap ()))
  ));
  lib.Register ("SendAchievement",    make_invoker (
      make_invoker (&send_achievement),
      make_invoker<void (Session&, const Achievement&, const LoadCallback&)> (xtl::bind (&send_achievement, _1, _2, _3, common::PropertyMap ())),
      make_invoker<void (Session&, const Achievement&)> (xtl::bind (&send_achievement, _1, _2, LoadCallback (), common::PropertyMap ()))
  ));
  lib.Register ("LoadLeaderboards",    make_invoker (
      make_invoker (&load_leaderboards),
      make_invoker<void (Session&, const LoadCallback&)> (xtl::bind (&load_leaderboards, _1, _2, common::PropertyMap ())),
      make_invoker<void (Session&)> (xtl::bind (&load_leaderboards, _1, LoadCallback (), common::PropertyMap ()))
  ));
  lib.Register ("LoadLeaderboard",    make_invoker (
      make_invoker (&load_user_leaderboard),
      make_invoker<void (Session&, const char*, const char*, const LoadCallback&)> (xtl::bind (&load_user_leaderboard, _1, _2, _3, _4, common::PropertyMap ())),
      make_invoker<void (Session&, const char*, const char*)> (xtl::bind (&load_user_leaderboard, _1, _2, _3, LoadCallback (), common::PropertyMap ())),
      make_invoker (&load_leaderboard),
      make_invoker<void (Session&, const char*, const LoadCallback&)> (xtl::bind (&load_leaderboard, _1, _2, _3, common::PropertyMap ())),
      make_invoker<void (Session&, const char*)> (xtl::bind (&load_leaderboard, _1, _2, LoadCallback (), common::PropertyMap ()))
  ));
  lib.Register ("SendScore",          make_invoker (
      make_invoker (&send_score),
      make_invoker<void (Session&, const Score&, const LoadCallback&)> (xtl::bind (&send_score, _1, _2, _3, common::PropertyMap ())),
      make_invoker<void (Session&, const Score&)> (xtl::bind (&send_score, _1, _2, LoadCallback (), common::PropertyMap ()))
  ));

  environment.RegisterType<Session> (SESSION_LIBRARY);
}

void bind_social_library (Environment& environment)
{
  bind_operation_status_library (environment);
  bind_achievement_library      (environment);
  bind_collection<Achievement>  (environment, ACHIEVEMENT_LIST_LIBRARY);
  bind_score_library            (environment);
  bind_collection<Score>        (environment, SCORE_LIST_LIBRARY);
  bind_leaderboard_library      (environment);
  bind_collection<Leaderboard>  (environment, LEADERBOARD_LIST_LIBRARY);
  bind_user_library             (environment);
  bind_collection<User>         (environment, USER_LIST_LIBRARY);
  bind_session_library          (environment);
}

}

namespace components
{

namespace social_script_binds
{

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_social_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> SocialScriptBind (COMPONENT_NAME);

}

}

}
