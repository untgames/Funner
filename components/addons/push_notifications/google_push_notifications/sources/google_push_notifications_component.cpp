#include "shared.h"

using namespace push_notifications;
using namespace push_notifications::google_push_notifications;

namespace
{

const char* COMPONENT_NAME              = "push_notifications.centers.google_center";                      //имя компонента
const char* LOAD_HANDLER_COMPONENT_NAME = "common.syslib.android.load_handlers.google_push_notifications"; //имя компонента обработчика загрузки приложения
const char* LOAD_HANDLER_ID             = "GoogleCenter";                                                  //идентификатор обработчика загрузки приложения
const char* CENTER_ID                   = "GoogleCenter";                                                  //идентификатор центра обработки пуш-сообщений
const char* CENTER_NAME_MASK            = "GoogleCenter";                                                  //маска имен центров обработки пуш-сообщений

}

/*
   Компонент центра обработки пуш-сообщений
*/

namespace components
{

namespace google_push_notifications
{

class GooglePushNotificationsComponent
{
  public:
    GooglePushNotificationsComponent ()
    {
      PushNotificationsCentersManager::RegisterCenter (CENTER_ID, CENTER_NAME_MASK, xtl::bind (&GooglePushNotificationsComponent::CreateCenter, this, _1));
    }

  private:
    IPushNotificationsCenter* CreateCenter (const char* center_name)
    {
      static const char* METHOD_NAME = "push_notifications::google_push_notifications::GooglePushNotificationsComponent::CreateCenter";

      if (xtl::xstrcmp (center_name, CENTER_NAME_MASK))
        throw xtl::make_argument_exception (METHOD_NAME, "center_name", center_name);

      return new PushNotificationsCenterImpl ();
    }
};

class GooglePushNotificationsLoadHandlerComponent
{
  public:
    GooglePushNotificationsLoadHandlerComponent ()
    {
      syslib::android::ApplicationManager::RegisterLoadHandler (LOAD_HANDLER_ID, xtl::bind (&GooglePushNotificationsLoadHandlerComponent::OnApplicationLoad, this, _1));
    }

  private:
    void OnApplicationLoad (JNIEnv* env)
    {
      PushNotificationsCenterImpl::InitJavaBindings (env);
    }
};

extern "C"
{

common::ComponentRegistrator<GooglePushNotificationsComponent> GooglePushNotifications (COMPONENT_NAME);
common::ComponentRegistrator<GooglePushNotificationsLoadHandlerComponent> GooglePushNotificationsLoadHandler (LOAD_HANDLER_COMPONENT_NAME);

}

}

}
