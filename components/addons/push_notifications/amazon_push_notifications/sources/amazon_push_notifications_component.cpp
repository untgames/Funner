#include "shared.h"

using namespace push_notifications;
using namespace push_notifications::amazon_push_notifications;

namespace
{

const char* COMPONENT_NAME              = "push_notifications.centers.amazon_center";                      //component namt
const char* LOAD_HANDLER_COMPONENT_NAME = "common.syslib.android.load_handlers.amazon_push_notifications"; //application load handler component name
const char* LOAD_HANDLER_ID             = "AmazonCenter";                                                  //application load handler id
const char* CENTER_ID                   = "AmazonCenter";                                                  //push notifications processing center id
const char* CENTER_NAME_MASK            = "AmazonCenter";                                                  //push notifications processing centers names wildcard

}

/*
   Push notifications processing center component
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
