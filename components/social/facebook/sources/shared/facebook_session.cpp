#include "shared.h"

using namespace social;
using namespace social::facebook;

namespace
{

const char* LOG_NAME            = "social.facebook.FacebookSession";
const char* SESSION_DESCRIPTION = "Facebook";

}

/*
   Конструктор / Деструктор
*/

FacebookSessionImpl::FacebookSessionImpl ()
  : log (LOG_NAME)
  , logged_in (false)
  , dialog_web_view_active (false)
{
  on_activate_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnResume, xtl::bind (&FacebookSessionImpl::OnActivate, this));
}

FacebookSessionImpl::~FacebookSessionImpl ()
{
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

void FacebookSessionImpl::ShowWindow (const char* window_name, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::ShowWindow";

  try
  {
    if (!IsUserLoggedIn ())
      throw xtl::format_operation_exception ("", "Can't show window before log in");

    if (dialog_web_view_active)
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

    CloseDialogWebView ();

    log.Printf ("Show dialog '%s'", url.c_str ());

    dialog_web_view_filter_connection     = dialog_web_view.RegisterFilter (xtl::bind (&FacebookSessionImpl::ProcessDialogRequest, this, _2));
    dialog_web_view_load_start_connection = dialog_web_view.RegisterEventHandler (syslib::WebViewEvent_OnLoadStart, xtl::bind (&FacebookSessionImpl::ProcessDialogRequest, this, (const char*)0));
    dialog_web_view_load_fail_connection  = dialog_web_view.RegisterEventHandler (syslib::WebViewEvent_OnLoadFail, xtl::bind (&FacebookSessionImpl::ProcessDialogFail, this));

    dialog_web_view.LoadRequest (url.c_str ());

    dialog_web_view_active = true;

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

bool FacebookSessionImpl::ProcessDialogRequest (const char* request)
{
  if (!request)
    request = dialog_web_view.Request ();

  if (request)
  {
    log.Printf ("Dialog load request '%s'", request);

    if (!strstr (request, "?"))
    {
      CloseDialogWebView ();
      return false;
    }
  }
  else
    log.Printf ("Dialog load request");

  return true;
}

void FacebookSessionImpl::ProcessDialogFail ()
{
  log.Printf ("Dialog load failed\n");

  CloseDialogWebView ();
}

/*
   Логин
*/

void FacebookSessionImpl::LogIn (const LoginCallback& callback, const common::PropertyMap& properties)
{
  try
  {
    //TODO check that we are already logged in
    //TODO refresh access token

    app_id = properties.GetString ("AppId");

    Platform::Login (app_id.c_str (), xtl::bind (&FacebookSessionImpl::OnPlatformLogInFinished, this, _1, _2, _3, _4, _5, properties, callback), properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("social::facebook::FacebookSessionImpl::LogIn");
    throw;
  }
}

void FacebookSessionImpl::OnPlatformLogInFinished (bool platform_login_result, OperationStatus status, const char* error, const char* in_token, const User& logged_in_user, const common::PropertyMap& properties, const LoginCallback& callback)
{
  try
  {
    //TODO check that we are already logged in
    //TODO refresh access token

    if (platform_login_result)
    {
      if (status == OperationStatus_Success)
      {
        logged_in    = true;
        token        = in_token;
        current_user = logged_in_user;

        current_user.Properties ().SetProperty ("Token", token.c_str ());
      }

      callback (status, error);
      return;
    }

    stl::string url = common::format ("https://m.facebook.com/dialog/oauth?client_id=%s&redirect_uri=https://www.facebook.com/connect/login_success.html&display=touch&response_type=token", app_id.c_str ());

    if (properties.IsPresent ("Permissions"))
      url.append (common::format ("&scope=%s", properties.GetString ("Permissions")));

/*    //TODO urlSchemeSuffix
    if (_urlSchemeSuffix) {
        [params setValue:_urlSchemeSuffix forKey:@"local_client_id"];
    }*/

    CloseDialogWebView ();

    dialog_web_view_filter_connection     = dialog_web_view.RegisterFilter (xtl::bind (&FacebookSessionImpl::ProcessLoginRequest, this, _2, callback));
    dialog_web_view_load_start_connection = dialog_web_view.RegisterEventHandler (syslib::WebViewEvent_OnLoadStart, xtl::bind (&FacebookSessionImpl::ProcessLoginRequest, this, (const char*)0, callback));
    dialog_web_view_load_fail_connection  = dialog_web_view.RegisterEventHandler (syslib::WebViewEvent_OnLoadFail, xtl::bind (&FacebookSessionImpl::ProcessLoginFail, this, callback));

    dialog_web_view.LoadRequest (url.c_str ());

    dialog_web_view_active = true;

    OnActivate ();
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

bool FacebookSessionImpl::ProcessLoginRequest (const char* request, const LoginCallback& callback)
{
  if (!request)
    request = dialog_web_view.Request ();

  if (request)
  {
    log.Printf ("Login load request '%s'", request);

    if (strstr (request, "https://www.facebook.com/connect/login_success.html") == request)
    {
      token = get_url_parameter (request, "access_token=");

      CloseDialogWebView ();

      if (token.empty ())  //login failed
      {
        stl::string error = get_url_parameter (request, "error_reason=");

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

        PerformRequest ("me/", "fields=id,username", xtl::bind (&FacebookSessionImpl::OnCurrentUserInfoLoaded, this, _1, _2, _3, callback));
      }

      return false;
    }

      //block unwanted links, ignore both http and https urls
    if (strstr (request, "://m.facebook.com/help") ||
        strstr (request, "://m.facebook.com/login/identify?ctx=recover"))
      return false;
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

  current_user = parse_user (response);

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

  logged_in = false;

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
  if (!dialog_web_view_active)
    return;

  syslib::Window& window = dialog_web_view.Window ();

  window.SetVisible (false);
  window.SetVisible (true);
  window.SetActive  (true);
  window.SetFocus   (true);
  window.Maximize   ();

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

  dialog_web_view.Window ().Hide ();

  dialog_web_view_active = false;
}
