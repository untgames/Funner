#include "shared.h"

using namespace social;
using namespace social::facebook;

namespace
{

const char* OPEN_URL_NOTIFICATION_PREFIX = "ApplicationOpenURL ";

const float WAIT_FOR_URL_AFTER_ACTIVATE_DELAY = 0.1f; //Длительность ожидания URL после активации до сообщения о ошибке

//Синглтон платформы
class IOsPlatformImpl
{
  public:
    //Деструктор
    ~IOsPlatformImpl ()
    {
      RemoveLoginHandler ();
    }

    //Добавление/удаление обработчиков открытия ссылок
    void SetLoginHandler (const char* app_base_uri, const social::facebook::DefaultPlatform::PlatformLoginCallback& handler)
    {
      try
      {
        if (!app_base_uri)
          throw xtl::make_null_argument_exception ("", "app_base_uri");

        open_url_connection    = syslib::Application::RegisterNotificationHandler (common::format ("%s%s*", OPEN_URL_NOTIFICATION_PREFIX, app_base_uri).c_str (), xtl::bind (&IOsPlatformImpl::OnOpenUrl, this, _1));
        on_activate_connection = syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnResume, xtl::bind (&IOsPlatformImpl::OnApplicationActivated, this));

        login_handler = handler;
      }
      catch (xtl::exception& e)
      {
        e.touch ("social::facebook::IOsPlatformImpl::AddOpenUrlHandler");
        throw;
      }
    }

    void RemoveLoginHandler ()
    {
      open_url_connection.disconnect ();
      on_activate_connection.disconnect ();

      login_handler = social::facebook::DefaultPlatform::PlatformLoginCallback ();

      login_error_action.Cancel ();
    }

  private:
    //Обработка открытия приложения по ссылке
    void OnOpenUrl (const char* notification)
    {
      login_handler (true, OperationStatus_Success, "", notification + strlen (OPEN_URL_NOTIFICATION_PREFIX));

      RemoveLoginHandler ();
    }

    //Обработка активации приложения во время ожидания данных логина от внешнего приложения
    void OnApplicationActivated ()
    {
      login_error_action = common::ActionQueue::PushAction (xtl::bind (&IOsPlatformImpl::OnLoginUrlNotReceived, this), common::ActionThread_Current, WAIT_FOR_URL_AFTER_ACTIVATE_DELAY);
    }

    //Оповещение о ошибке логина, если приложение было активировано не по внешней ссылке во время ожидания результатов логина
    void OnLoginUrlNotReceived ()
    {
      login_handler (true, OperationStatus_Failure, "Login URL not received", "");

      RemoveLoginHandler ();
    }

  private:
    xtl::auto_connection                                     open_url_connection;     //соединение открытия приложения по вшеншней ссылке
    xtl::auto_connection                                     on_activate_connection;  //соединение активации приложения
    common::Action                                           login_error_action;      //экшн отложенного оповещения о ошибке логина, если приложение было активировано не по внешней ссылке во время ожидания результатов логина
    social::facebook::DefaultPlatform::PlatformLoginCallback login_handler;           //обработчик результатов логина
};

typedef common::Singleton<IOsPlatformImpl> IOsPlatformSingleton;

}

/*
   iOs платформа
*/

/*
   Логин (возвращает true, если началось выполнение специфичного для данной платформы логина)
*/

void IOsPlatform::Login (const char* app_id, const PlatformLoginCallback& callback, const common::PropertyMap& properties)
{
  //Facebook application auth
  stl::string scheme = "fbauth";
  stl::string url_scheme_suffix;

  if (properties.IsPresent ("UrlSchemeSuffix"))
  {
    url_scheme_suffix = properties.GetString ("UrlSchemeSuffix");

    scheme.append ("2");
  }

  stl::string params;

  if (properties.IsPresent ("Permissions"))
    params = common::format ("&scope=%s", properties.GetString ("Permissions"));
  if (!url_scheme_suffix.empty ())
    params.append (common::format ("&local_client_id=%s", url_scheme_suffix.c_str ()));

  NSString *url = [NSString stringWithFormat:@"%s://authorize?client_id=%s%s", scheme.c_str (), app_id, params.c_str ()];

  stl::string app_base_uri = common::format ("fb%s%s://authorize", app_id, url_scheme_suffix.c_str ());

  IOsPlatformSingleton::Instance ()->SetLoginHandler (app_base_uri.c_str (), callback);

  if ([[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]])
    return;

  url = [NSString stringWithFormat:@"https://m.facebook.com/dialog/oauth?client_id=%s&redirect_uri=%s&display=touch&response_type=token%s", app_id, app_base_uri.c_str (), params.c_str ()];

  if ([[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]])
    return;

  IOsPlatformSingleton::Instance ()->RemoveLoginHandler ();

  callback (false, OperationStatus_Failure, "", "");
}

void IOsPlatform::CancelLogin ()
{
  IOsPlatformSingleton::Instance ()->RemoveLoginHandler ();
}
