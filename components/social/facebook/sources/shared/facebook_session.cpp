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

FacebookSessionImpl::FacebookSessionImpl (const common::PropertyMap& properties)
  : log (LOG_NAME)
  , logged_in (false)
  , dialog_web_view_active (false)
{
  LogIn (LoginCallback (), properties);
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
    if (!window_name)
      throw xtl::make_null_argument_exception ("", "window_name");
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
    throw;
  }
}

/*
   Логин
*/

void FacebookSessionImpl::LogIn (const LoginCallback& callback, const common::PropertyMap& properties)
{
  static const char* METHOD_NAME = "social::facebook::FacebookSessionImpl::LogIn";

  static const char* LOGIN_PROPERTIES [] = { "AppId" };

  FacebookSessionImpl::CheckUnknownProperties (METHOD_NAME, properties, sizeof (LOGIN_PROPERTIES) / sizeof (*LOGIN_PROPERTIES), LOGIN_PROPERTIES);

  //TODO check that we are already logged in
  //TODO refresh access token

  const char* app_id = properties.GetString ("AppId");

  stl::string url = common::format ("https://m.facebook.com/dialog/oauth?client_id=%s&redirect_uri=https://www.facebook.com/connect/login_success.html&display=touch&response_type=token", app_id);

  /*
      //TODO permissions
      if (permissions != nil) {
          NSString* scope = [permissions componentsJoinedByString:@","];
          [params setValue:scope forKey:@"scope"];
      }

      //TODO urlSchemeSuffix
      if (_urlSchemeSuffix) {
          [params setValue:_urlSchemeSuffix forKey:@"local_client_id"];
      }
  */

  CloseDialogWebView ();

//  LogOut ();  //DEBUG

  dialog_web_view_filter_connection     = dialog_web_view.RegisterFilter (xtl::bind (&FacebookSessionImpl::ProcessLoginRequest, this, _2));
  dialog_web_view_load_start_connection = dialog_web_view.RegisterEventHandler (syslib::WebViewEvent_OnLoadStart, xtl::bind (&FacebookSessionImpl::ProcessLoginRequest, this, (const char*)0));
  dialog_web_view_load_fail_connection  = dialog_web_view.RegisterEventHandler (syslib::WebViewEvent_OnLoadFail, xtl::bind (&FacebookSessionImpl::ProcessLoginFail, this));

  dialog_web_view.LoadRequest (url.c_str ());

  dialog_web_view_active = true;

  OnActivate ();
}

/*
   Обработка события логина
*/

bool FacebookSessionImpl::ProcessLoginRequest (const char* request)
{
  if (!request)
    request = dialog_web_view.Request ();

  if (request)
  {
    log.Printf ("Login load request '%s'", request);

    if (strstr (request, "https://www.facebook.com/connect/login_success.html") == request)
    {
      token = get_url_parameter (request, "access_token=");

      //TODO extract token, extract cancel event

      CloseDialogWebView ();

      if (token.empty ())  //login failed
      {
        stl::string error = get_url_parameter (request, "error_reason=");

        if (error == "user_denied")
        {
          //TODO

          log.Printf ("Login canceled");
        }
        else
        {
          //TODO

          log.Printf ("Login failed, error '%s'", error.c_str ());
        }
      }
      else
      {
        log.Printf ("Logged in");

        PerformRequest ("", "fields=id,username", xtl::bind (&FacebookSessionImpl::OnUserInfoLoaded, this, _1, _2, _3));
        //TODO setup current user
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

void FacebookSessionImpl::OnUserInfoLoaded (bool succeeded, const stl::string& status, common::ParseNode response)
{
  log.Printf ("User info load status '%s'", status.c_str ());

  if (!succeeded)
  {
    //TODO login failed
    return;
  }

  //TODO fill current user info

  logged_in = true;

  //TODO callback
}

void FacebookSessionImpl::ProcessLoginFail ()
{
  log.Printf ("Login load failed\n");

  CloseDialogWebView ();
  //TODO
}

void FacebookSessionImpl::LogOut ()
{
  CloseSession ();

  syslib::CookieManager::DeleteAllCookies ();

  logged_in = false;

  token.clear ();
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
  CloseDialogWebView ();

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
  //TODO who will call this method?????

  if (!dialog_web_view_active)
    return;

  syslib::Window& window = dialog_web_view.Window ();

  window.SetVisible (false);
  window.SetVisible (true);
  window.SetActive  (true);
  window.SetFocus   (true);
//  window.Maximize   ();

  //TODO read window size and position from property map

  //DEBUG
  window.SetSize (800, 600);
  window.SetPosition (100, 100);
  window.Show ();

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
