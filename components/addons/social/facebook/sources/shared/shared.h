#ifndef SOCIAL_FACEBOOK_SHARED_HEADER
#define SOCIAL_FACEBOOK_SHARED_HEADER

#include <stl/list>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/shared_ptr.h>

#include <common/action_queue.h>
#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/application.h>
#include <syslib/cookie.h>
#include <syslib/web_view.h>

#include <social/session.h>

#include <shared/platform.h>

namespace social
{

namespace facebook
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Facebook сессия
///////////////////////////////////////////////////////////////////////////////////////////////////
class FacebookSessionImpl: public IAchievementManager, public ILeaderboardManager, public IUserManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/Деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    FacebookSessionImpl  ();
    ~FacebookSessionImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка наличия в PropertyMap неизвестных полей
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void CheckUnknownProperties (const char* source, const common::PropertyMap& properties,
                                        size_t known_properties_count, const char** known_properties_names);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание сессии
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Показ стандартных окон
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ShowWindow (const char* window_name, const WindowCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Логин
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogIn          (const LoginCallback& callback, const common::PropertyMap& properties);
    void LogOut         ();
    bool IsUserLoggedIn ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение залогиненного пользователя
///////////////////////////////////////////////////////////////////////////////////////////////////
    User& CurrentUser ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Достижения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadAchievements (const LoadAchievementsCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Иконка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadAchievementPicture (const Achievement& achievement, const LoadAchievementPictureCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Публикация
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SendAchievement (const Achievement& achievement, const SendAchievementCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблицы рекордов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadLeaderboards (const LoadLeaderboardsCallback& callback, const common::PropertyMap& properties);
    void LoadLeaderboard  (const char* leaderboard_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties);
    void LoadLeaderboard  (const char* leaderboard_id, const char* user_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Публикация
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SendScore (const Score& score, const SendScoreCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка пользователя по идентификатору
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadUser (const char* id, const LoadUserCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Аватар
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadUserPicture (const User& user, const LoadUserPictureCallback& callback, const common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Друзья
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadFriendsIds (const User& user, const LoadFriendsIdsCallback& callback, const common::PropertyMap& properties);
    void LoadFriends    (const User& user, const LoadFriendsCallback& callback, const common::PropertyMap& properties);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Закрытие сессии
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CloseSession ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выполнение запроса
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (bool succeeded, const stl::string& status, common::ParseNode response)> RequestCallback;

    void PerformRequest         (const char* method_name, const char* params, const RequestCallback& callback);
    void CleanupRequestsActions ();

    static void PerformRequestNotify   (const RequestCallback& callback, bool succeeded, const char* status, const common::ParseNode& response);
    static void PerformRequestImpl     (common::Action& action, const stl::string& method_name, const stl::string& params, const stl::string& token, const RequestCallback& callback, FacebookSessionImpl* session, common::Log log, bool after_relogin);
           void RequestTryRelogin      (const stl::string& method_name, const stl::string& params, const RequestCallback& callback);
           void RequestReloginCallback (OperationStatus status, const char* error, const stl::string& method_name, const stl::string& params, const RequestCallback& callback);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка события активации приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void OnActivate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Закрытие диалога
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CloseDialogWebView ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка событий логина
///////////////////////////////////////////////////////////////////////////////////////////////////
    void OnCurrentUserInfoLoaded (bool succeeded, const stl::string& status, common::ParseNode response, const LoginCallback& callback);
    bool ProcessLoginRequest     (const char* request, const LoginCallback& callback);
    void ProcessLoginFail        (const LoginCallback& callback);
    void OnPlatformLogInFinished (bool platform_login_result, OperationStatus status, const char* error, const char* token, const LoginCallback& callback);
    void HandleLoginResultUrl    (const char* url, const LoginCallback& callback);
    void OnLoginTokenUpdated     (const LoginCallback& callback);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка ответов запросов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void OnFriendsInfoLoaded (bool succeeded, const stl::string& status, common::ParseNode response, const LoadFriendsCallback& callback);
    void OnUserInfoLoaded    (bool succeeded, const stl::string& status, common::ParseNode response, const LoadUserCallback& callback);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка событий диалогов
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool ProcessDialogRequest (const char* request, const WindowCallback& callback);
    void ProcessDialogFail    (const WindowCallback& callback);

  private:
    FacebookSessionImpl (const FacebookSessionImpl& source);              //no impl
    FacebookSessionImpl& operator = (const FacebookSessionImpl& source);  //no impl

  private:
    typedef stl::list<common::Action>        ActionsList;
    typedef xtl::shared_ptr<syslib::WebView> WebViewPtr;

  private:
    common::Log          log;                                      //logging
    common::PropertyMap  login_properties;                         //custom login properties
    stl::string          app_id;                                   //FB application id
    stl::string          token;                                    //user token
    stl::string          last_dialog_base_request;                 //last dialog web-view request
    bool                 logged_in;                                //is logged in
    User                 current_user;                             //logged in user info
    WebViewPtr           dialog_web_view;                          //web view used for dialogs
    xtl::auto_connection dialog_web_view_filter_connection;        //dialog web view requests filter connection
    xtl::auto_connection dialog_web_view_load_start_connection;    //dialog web view requests load connection
    xtl::auto_connection dialog_web_view_load_fail_connection;     //dialog web view requests error connection
    xtl::auto_connection on_activate_connection;                   //application activate connection
    ActionsList          pending_actions;                          //actions queue
    common::Action       activate_after_dialog_show_action;        //action for web view window activation
};

//utils

//replace percent escapes using utf-8 encoding
stl::string replace_percent_escapes (const char* str);

//make percent escaped string using utf-8 encoding
stl::string percent_escape (const char* str);

//return parameter from url, or empty string if not found
stl::string get_url_parameter (const char* url, const char* param_name);

//parse user
User parse_user (common::ParseNode node);

//get first attribute value for tag with name if it is present
const char* get_named_attribute (common::ParseNode node, const char* name, const char* default_value);

}

}

#endif
