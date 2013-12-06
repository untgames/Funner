#include "shared.h"

using namespace social;
using namespace social::facebook;

namespace
{

const common::ActionQueue::time_t DESTROY_WEB_VIEW_DELAY     = 5;  //Задержка удаления web-view после скрытия
const common::ActionQueue::time_t ACTIVATE_AFTER_LOGIN_DELAY = 3;  //Задержка показа web-view логина (для логина без показа окна в случае автопродления токена)

const char* LOG_NAME            = "social.facebook.FacebookSession";
const char* SESSION_DESCRIPTION = "Facebook";

bool session_created = false;  //Существует ли созданная сессия

//Функция, необходимая для удаления web-view после выполнения колбека
void delete_web_view (xtl::shared_ptr<syslib::WebView> web_view)
{
}

}

/*
   Конструктор / Деструктор
*/

FacebookSessionImpl::FacebookSessionImpl ()
  : log (LOG_NAME)
  , logged_in (false)
{
  if (session_created)
    throw xtl::format_operation_exception ("social::facebook::FacebookSessionImpl::FacebookSessionImpl", "Another session already created");

  on_activate_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnResume, xtl::bind (&FacebookSessionImpl::OnActivate, this));

  session_created = true;
}

FacebookSessionImpl::~FacebookSessionImpl ()
{
  session_created = false;

  CloseSession ();
}

/*
   Описание сессии
*/

const char* FacebookSessionImpl::GetDescription ()
{
  return SESSION_DESCRIPTION;
}

/*
   Показ стандартных окон
*/

void FacebookSessionImpl::ShowWindow (const char* window_name, const WindowCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::ShowWindow";

  try
  {
    if (!IsUserLoggedIn ())
      throw xtl::format_operation_exception ("", "Can't show window before log in");

    if (dialog_web_view)
      throw xtl::format_operation_exception ("", "Another window already shown");

    if (!window_name)
      throw xtl::make_null_argument_exception ("", "window_name");

    stl::string url = common::format ("https://m.facebook.com/dialog/%s?app_id=%s&display=touch&redirect_uri=fbconnect://success&access_token=%s", window_name, app_id.c_str (), token.c_str ());

    if (properties.Size ())
    {
      stl::string params;

      for (size_t i = 0, count = properties.Size (); i < count; i++)
        params += common::format ("&%s=%s", properties.PropertyName (i), properties.GetString (i));

      url += params;
    }

    url = percent_escape (url.c_str ());

    log.Printf ("Show dialog '%s'", url.c_str ());

    dialog_web_view.reset (new syslib::WebView);

    last_dialog_base_request = common::format ("https://m.facebook.com/dialog/%s", window_name);

    dialog_web_view_filter_connection     = dialog_web_view->RegisterFilter (xtl::bind (&FacebookSessionImpl::ProcessDialogRequest, this, _2, callback));
    dialog_web_view_load_start_connection = dialog_web_view->RegisterEventHandler (syslib::WebViewEvent_OnLoadStart, xtl::bind (&FacebookSessionImpl::ProcessDialogRequest, this, (const char*)0, callback));
    dialog_web_view_load_fail_connection  = dialog_web_view->RegisterEventHandler (syslib::WebViewEvent_OnLoadFail, xtl::bind (&FacebookSessionImpl::ProcessDialogFail, this, callback));

    dialog_web_view->LoadRequest (url.c_str ());

    OnActivate ();
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
    throw;
  }
}

/*
   Обработка событий диалогов
*/

bool FacebookSessionImpl::ProcessDialogRequest (const char* request, const WindowCallback& callback)
{
  if (!request)
    request = dialog_web_view->Request ();

  if (request)
  {
    log.Printf ("Dialog load request '%s'", request);

    if (request == last_dialog_base_request) //this url loads when user taps action button, so we needs to hide window after this
    {
      dialog_web_view->Window ().Hide ();
      dialog_web_view->Window ().SetFocus (false);
    }

    if (strstr (request, "fbconnect://success") == request) //dialog finished
    {
      stl::string request_copy = request;

      CloseDialogWebView ();

      if (strstr (request, "?"))
      {
        stl::string error = get_url_parameter (request_copy.c_str (), "error_code=");

        if (error.empty ())
          callback (OperationStatus_Success, replace_percent_escapes (request).c_str ());
        else if (error == "4201")
          callback (OperationStatus_Canceled, "");
        else
          callback (OperationStatus_Failure, error.c_str ());
      }
      else
        callback (OperationStatus_Canceled, "");

      return true;
    }

    if (strstr (request, "://m.facebook.com/home.php"))  //Error occured
    {
      CloseDialogWebView ();
      callback (OperationStatus_Failure, "Load failed");
      return false;
    }
  }
  else
    log.Printf ("Dialog load request");

  return true;
}

void FacebookSessionImpl::ProcessDialogFail (const WindowCallback& callback)
{
  log.Printf ("Dialog load failed\n");

  CloseDialogWebView ();

  callback (OperationStatus_Failure, "Load failed");
}

/*
   Логин
*/

void FacebookSessionImpl::LogIn (const LoginCallback& callback, const common::PropertyMap& properties)
{
  try
  {
    bool has_previous_token = properties.IsPresent ("Token");

    if (has_previous_token && logged_in)
      throw xtl::format_operation_exception ("", "Login properties must not include 'UserId' or 'Token' for refresh token login");

    app_id           = properties.GetString ("AppId");
    login_properties = properties.Clone ();

    try
    {
      Platform::PublishAppInstallEvent (app_id.c_str ());
    }
    catch (...)
    {
    }

    if (has_previous_token)
    {
      if (properties.IsPresent ("UserId"))
        current_user.SetId (properties.GetString ("UserId"));

      token = properties.GetString ("Token");

      login_properties.RemoveProperty ("UserId");
      login_properties.RemoveProperty ("Token");

      OnLoginTokenUpdated (callback);

      return;
    }

    Platform::Login (app_id.c_str (), xtl::bind (&FacebookSessionImpl::OnPlatformLogInFinished, this, _1, _2, _3, _4, callback), properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("social::facebook::FacebookSessionImpl::LogIn");
    throw;
  }
}

void FacebookSessionImpl::OnPlatformLogInFinished (bool platform_login_result, OperationStatus status, const char* error, const char* in_token, const LoginCallback& callback)
{
  try
  {
    //TODO check that we are already logged in

    if (platform_login_result)
    {
      token = in_token;

      if (token.empty ())
        callback (status, error);
      else
        OnLoginTokenUpdated (callback);

      return;
    }

    stl::string url = common::format ("https://m.facebook.com/dialog/oauth?client_id=%s&redirect_uri=https://www.facebook.com/connect/login_success.html&display=touch&response_type=token", app_id.c_str ());

    if (login_properties.IsPresent ("Permissions"))
      url.append (common::format ("&scope=%s", login_properties.GetString ("Permissions")));

    CloseDialogWebView ();

    dialog_web_view.reset (new syslib::WebView);

    dialog_web_view_filter_connection     = dialog_web_view->RegisterFilter (xtl::bind (&FacebookSessionImpl::ProcessLoginRequest, this, _2, callback));
    dialog_web_view_load_start_connection = dialog_web_view->RegisterEventHandler (syslib::WebViewEvent_OnLoadStart, xtl::bind (&FacebookSessionImpl::ProcessLoginRequest, this, (const char*)0, callback));
    dialog_web_view_load_fail_connection  = dialog_web_view->RegisterEventHandler (syslib::WebViewEvent_OnLoadFail, xtl::bind (&FacebookSessionImpl::ProcessLoginFail, this, callback));

    dialog_web_view->LoadRequest (url.c_str ());

    activate_after_dialog_show_action = common::ActionQueue::PushAction (xtl::bind (&FacebookSessionImpl::OnActivate, this), common::ActionThread_Main, ACTIVATE_AFTER_LOGIN_DELAY);
  }
  catch (xtl::exception& e)
  {
    e.touch ("social::facebook::FacebookSessionImpl::OnPlatformLogInFinished");
    throw;
  }
}

/*
   Обработка события логина
*/

void FacebookSessionImpl::OnLoginTokenUpdated (const LoginCallback& callback)
{
  PerformRequest ("me/", "fields=id,username", xtl::bind (&FacebookSessionImpl::OnCurrentUserInfoLoaded, this, _1, _2, _3, callback));
}

void FacebookSessionImpl::HandleLoginResultUrl (const char* url, const LoginCallback& callback)
{
  if (!url)
    throw xtl::make_null_argument_exception ("social::facebook::FacebookSessionImpl::HadleLoginResultUrl", "url");

  log.Printf ("Handle login result url '%s'", url);

  token = get_url_parameter (url, "access_token=");

  stl::string request_copy (url);

  CloseDialogWebView ();

  if (token.empty ())  //login failed
  {
    stl::string error = get_url_parameter (request_copy.c_str (), "error_reason=");

    if (error == "user_denied")
    {
      log.Printf ("Login canceled");

      callback (OperationStatus_Canceled, "");
    }
    else
    {
      log.Printf ("Login failed, error '%s'", error.c_str ());

      callback (OperationStatus_Failure, error.c_str ());
    }
  }
  else
  {
    log.Printf ("Logged in");

    OnLoginTokenUpdated (callback);
  }
}

bool FacebookSessionImpl::ProcessLoginRequest (const char* request, const LoginCallback& callback)
{
  if (!request)
    request = dialog_web_view->Request ();

  if (request)
  {
    log.Printf ("Login load request '%s'", request);

    if (strstr (request, "https://www.facebook.com/connect/login_success.html") == request)
    {
      HandleLoginResultUrl (request, callback);

      return false;
    }

    if (strstr (request, "://m.facebook.com/home.php"))  //Leave app button
    {
      CloseDialogWebView ();

      callback (OperationStatus_Canceled, "");
      return false;
    }

      //block unwanted links, ignore both http and https urls
    if (strstr (request, "://m.facebook.com/help") ||
        strstr (request, "://m.facebook.com/login/identify?ctx=recover"))
      return false;

    if (strstr (request, "://m.facebook.com/dialog/oauth"))
      OnActivate ();
  }
  else
    log.Printf ("Login load request");

  return true;
}

void FacebookSessionImpl::OnCurrentUserInfoLoaded (bool succeeded, const stl::string& status, common::ParseNode response, const LoginCallback& callback)
{
  log.Printf ("User info load status '%s'", status.c_str ());

  if (!succeeded)
  {
    callback (OperationStatus_Failure, status.c_str ());
    return;
  }

  User logged_in_user = parse_user (response);

  if (xtl::xstrlen (current_user.Id ()) && xtl::xstrcmp (current_user.Id (), logged_in_user.Id ()))
  {
    callback (OperationStatus_Failure, "Token refreshed for other user");
    return;
  }

  current_user = logged_in_user;

  current_user.Properties ().SetProperty ("Token", token.c_str ());

  logged_in = true;

  callback (OperationStatus_Success, status.c_str ());
}

void FacebookSessionImpl::ProcessLoginFail (const LoginCallback& callback)
{
  log.Printf ("Login load failed\n");

  CloseDialogWebView ();

  app_id.clear ();

  callback (OperationStatus_Failure, "Can't load login page");
}

void FacebookSessionImpl::LogOut ()
{
  CloseSession ();

  Platform::LogOut ();

  syslib::CookieManager::DeleteAllCookies ();
}

bool FacebookSessionImpl::IsUserLoggedIn ()
{
  return logged_in;
}

/*
   Закрытие сессии
*/

void FacebookSessionImpl::CloseSession ()
{
  Platform::CancelLogin ();

  CloseDialogWebView ();

    //TODO remove this!!!!!
  for (ActionsList::iterator iter = pending_actions.begin (), end = pending_actions.end (); iter != end; ++iter)
    iter->Cancel ();

    //wating for all pending actions to complete
  for (;;)
  {
    bool has_unfinished_actions = false;

    for (ActionsList::iterator iter = pending_actions.begin (), end = pending_actions.end (); iter != end; ++iter)
    {
      if (!iter->IsCompleted ())
      {
        has_unfinished_actions = true;
        break;
      }
    }

    if (!has_unfinished_actions)
      break;

    syslib::Application::Sleep (1);
  }

  pending_actions.clear ();

  activate_after_dialog_show_action.Cancel ();

  logged_in = false;

  login_properties.Clear ();
  app_id.clear ();
  token.clear ();
}

/*
   Проверка наличия в PropertyMap неизвестных полей
*/

void FacebookSessionImpl::CheckUnknownProperties (const char* source, const common::PropertyMap& properties,
                                                 size_t known_properties_count, const char** known_properties_names)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::CheckUnknownProperties";

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

/*
   Обработка события активации приложения
*/

void FacebookSessionImpl::OnActivate ()
{
  if (!dialog_web_view)
    return;

  syslib::Window& window = dialog_web_view->Window ();

  window.SetVisible (false);
  window.SetVisible (true);
  window.SetActive  (true);
  window.SetFocus   (true);
  window.Maximize   ();

  activate_after_dialog_show_action = common::Action ();
  //TODO read window size and position from property map

  /*  //DEBUG
  window.SetSize (800, 600);
  window.SetPosition (100, 100);
  window.Show ();*/
}

/*
   Закрытие диалога
*/

void FacebookSessionImpl::CloseDialogWebView ()
{
  dialog_web_view_filter_connection.disconnect ();
  dialog_web_view_load_start_connection.disconnect ();
  dialog_web_view_load_fail_connection.disconnect ();

  if (dialog_web_view)
  {
    try
    {
      dialog_web_view->Window ().Hide ();
      dialog_web_view->Window ().SetFocus (false);
    }
    catch (xtl::exception& e)
    {
      log.Printf ("Exception while closing dialog web view: '%s'", e.what ());
    }
    catch (...)
    {
      log.Printf ("Unknown exception while closing dialog web view");
    }

    common::ActionQueue::PushAction (xtl::bind (&delete_web_view, dialog_web_view), common::ActionThread_Main, DESTROY_WEB_VIEW_DELAY);

    dialog_web_view.reset ();
  }
}
