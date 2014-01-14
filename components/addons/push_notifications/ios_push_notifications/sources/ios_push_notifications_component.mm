#include "shared.h"

using namespace push_notifications;
using namespace push_notifications::ios_push_notifications;

namespace
{

const char* COMPONENT_NAME  = "push_notifications.centers.ios_center"; //имя компонента
const char* CENTER_ID        = "IOSCenter";                            //идентификатор центра обработки пуш-сообщений
const char* CENTER_NAME_MASK = "IOSCenter";                            //маска имен центров обработки пуш-сообщений

}

/*
   Компонент магазина
*/

namespace components
{

namespace ios_push_notifications
{

class IOSPushNotificationsComponent
{
  public:
    IOSPushNotificationsComponent ()
    {
      PushNotificationsCentersManager::RegisterCenter (CENTER_ID, CENTER_NAME_MASK, xtl::bind (&IOSPushNotificationsComponent::CreateCenter, this, _1));
    }

  private:
    IPushNotificationsCenter* CreateCenter (const char* center_name)
    {
      static const char* METHOD_NAME = "push_notifications::ios_push_notifications::IOSPushNotificationsComponent::CreateCenter";

      if (xtl::xstrcmp (center_name, CENTER_NAME_MASK))
        throw xtl::make_argument_exception (METHOD_NAME, "center_name", center_name);

      return new PushNotificationsCenterImpl ();
    }
};

extern "C"
{

common::ComponentRegistrator<IOSPushNotificationsComponent> IOSPushNotifications (COMPONENT_NAME);

}

}

}
