#include "shared.h"

using namespace push_notifications;
using namespace push_notifications::amazon_push_notifications;

namespace
{

const char* COMPONENT_NAME              = "push_notifications.centers.amazon_center";                      //имя компонента
const char* LOAD_HANDLER_COMPONENT_NAME = "common.syslib.android.load_handlers.amazon_push_notifications"; //имя компонента обработчика загрузки приложения
const char* LOAD_HANDLER_ID             = "AmazonCenter";                                                  //идентификатор обработчика загрузки приложения
const char* CENTER_ID                   = "AmazonCenter";                                                  //идентификатор центра обработки пуш-сообщений
const char* CENTER_NAME_MASK            = "AmazonCenter";                                                  //маска имен центров обработки пуш-сообщений

}

/*
   Компонент центра обработки пуш-сообщений
*/

namespace components
{

namespace amazon_push_notifications
{

class AmazonPushNotificationsComponent
{
  public:
    AmazonPushNotificationsComponent ()
    {
      PushNotificationsCentersManager::RegisterCenter (CENTER_ID, CENTER_NAME_MASK, xtl::bind (&AmazonPushNotificationsComponent::CreateCenter, this, _1));
    }

  private:
    IPushNotificationsCenter* CreateCenter (const char* center_name)
    {
      static const char* METHOD_NAME = "push_notifications::amazon_push_notifications::AmazonPushNotificationsComponent::CreateCenter";

      if (xtl::xstrcmp (center_name, CENTER_NAME_MASK))
        throw xtl::make_argument_exception (METHOD_NAME, "center_name", center_name);

      return new PushNotificationsCenterImpl ();
    }
};

class AmazonPushNotificationsLoadHandlerComponent
{
  public:
    AmazonPushNotificationsLoadHandlerComponent ()
    {
      syslib::android::ApplicationManager::RegisterLoadHandler (LOAD_HANDLER_ID, xtl::bind (&AmazonPushNotificationsLoadHandlerComponent::OnApplicationLoad, this, _1));
    }

  private:
    void OnApplicationLoad (JNIEnv* env)
    {
      PushNotificationsCenterImpl::InitJavaBindings (env);
    }
};

extern "C"
{

common::ComponentRegistrator<AmazonPushNotificationsComponent> AmazonPushNotifications (COMPONENT_NAME);
common::ComponentRegistrator<AmazonPushNotificationsLoadHandlerComponent> AmazonPushNotificationsLoadHandler (LOAD_HANDLER_COMPONENT_NAME);

}

}

}
