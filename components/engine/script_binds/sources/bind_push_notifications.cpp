#include "shared.h"

using namespace script;
using namespace push_notifications;

namespace
{

/*
    Константы
*/

const char* BINDER_NAME          = "PushNotifications";
const char* COMPONENT_NAME       = "script.binds.PushNotifications";
const char* CENTER_LIBRARY       = "PushNotifications.Center";

PushNotificationsCenter create_center (const char* name)
{
  return PushNotificationsCenter (name);
}

void bind_center_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (CENTER_LIBRARY);

  lib.Register ("Create", make_invoker (&create_center));

  lib.Register ("get_Description", make_invoker (&PushNotificationsCenter::Description));

  lib.Register ("CreateRegisterCallback",   make_callback_invoker<PushNotificationsCenter::RegisterCallback::signature_type> ());
  lib.Register ("RegisterForNotifications", make_invoker (
      make_invoker (xtl::implicit_cast<void (PushNotificationsCenter::*)(const PushNotificationsCenter::RegisterCallback&, const common::PropertyMap&)> (&PushNotificationsCenter::RegisterForNotifications)),
      make_invoker<void (PushNotificationsCenter&, const PushNotificationsCenter::RegisterCallback&)> (xtl::bind (xtl::implicit_cast<void (PushNotificationsCenter::*)(const PushNotificationsCenter::RegisterCallback&, const common::PropertyMap&)> (&PushNotificationsCenter::RegisterForNotifications), _1, _2, common::PropertyMap ()))
  ));
  lib.Register ("UnregisterForNotifications",   make_invoker (&PushNotificationsCenter::UnregisterForNotifications));
  lib.Register ("CreateNotificationsHandler",   make_callback_invoker<PushNotificationsCenter::NotificationsHandler::signature_type> ());
  lib.Register ("RegisterNotificationsHandler", make_invoker (&PushNotificationsCenter::RegisterNotificationsHandler));

  environment.RegisterType<PushNotificationsCenter> (CENTER_LIBRARY);
}

void bind_push_notifications_libraries (Environment& environment)
{
  bind_center_library (environment);
}

}

namespace components
{

namespace push_notifications_script_binds
{

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_push_notifications_libraries (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> PushNotificationsScriptBind (COMPONENT_NAME);

}

}

}
