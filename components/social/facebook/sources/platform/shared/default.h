#ifndef SOCIAL_FACEBOOK_DEFAULT_PLATFORM_HEADER
#define SOCIAL_FACEBOOK_DEFAULT_PLATFORM_HEADER

#include <xtl/function.h>

#include <common/property_map.h>

#include <social/session.h>

namespace social
{

namespace facebook
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Платформа по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultPlatform
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Логин
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (bool platform_login_result, OperationStatus status, const char* error, const char* in_token, const User& logged_in_user)> PlatformLoginCallback;

    static void Login       (const char* app_id, const PlatformLoginCallback& callback, const common::PropertyMap& properties);
    static void CancelLogin ();
};

}

}

#endif
