#include "shared.h"

using namespace social;
using namespace social::facebook;

namespace
{
/*
//Синглтон платформы
class IOsPlatformImpl
{
  public:
//Конструктор
    IOsPlatformImpl ()
    {

    }

  private:
    xtl::auto_connection open_url_connection;
};

typedef common::Singleton<IOsPlatformImpl> IOsPlatformSingleton;*/

}

/*
   iOs платформа
*/

/*
   Логин (возвращает true, если началось выполнение специфичного для данной платформы логина)
*/

void IOsPlatform::Login (const char* app_id, const PlatformLoginCallback& callback, const common::PropertyMap& properties)
{
/*  //Facebook application auth
  NSString *scheme            = @"fbauth";
  NSString *url_scheme_suffix = @"";  //TODO

  /* //TODO
     if (_urlSchemeSuffix) {
          scheme = [scheme stringByAppendingString:@"2"];
      }*/

/*  NSString *permissions = @"";

  if (properties.IsPresent ("Permissions"))
    permissions = [NSString stringWithFormat:@"&scope=%s", properties.GetString ("Permissions")];

  NSString *url = [NSString stringWithFormat:@"%@://authorize?client_id=%s%@", scheme, app_id, permissions];

  if ([[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]])
    return;

  NSString *redirect_uri = [NSString stringWithFormat:@"fb%s%@://authorize", app_id, url_scheme_suffix];

  url = [NSString stringWithFormat:@"https://m.facebook.com/dialog/oauth?client_id=%s&redirect_uri=%s&display=touch&response_type=token%@", app_id, redirect_uri, permissions);

  if ([[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]])
    return;*/

  callback (false, OperationStatus_Failure, "", "", User ());
}

void IOsPlatform::CancelLogin ()
{
}
