#include "shared.h"

using namespace analytics::flurry::android;

namespace
{

const char* LOAD_HANDLER_COMPONENT_NAME = "common.syslib.android.load_handlers.android_flurry_analytics"; //application launch handler component name
const char* LOAD_HANDLER_ID             = "AndroidFlurryAnalytics";                                       //application launch handler id

}

/*
   Analytics component
*/

namespace components
{

namespace android_flurry_analytics
{

class AndroidFlurryAnalyticsLoadHandlerComponent
{
  public:
    AndroidFlurryAnalyticsLoadHandlerComponent ()
    {
      syslib::android::ApplicationManager::RegisterLoadHandler (LOAD_HANDLER_ID, xtl::bind (&AndroidFlurryAnalyticsLoadHandlerComponent::OnApplicationLoad, this, _1));
    }

  private:
    void OnApplicationLoad (JNIEnv* env)
    {
      FlurrySingleton::Instance ()->InitJavaBindings (env);
    }
};

extern "C"
{

common::ComponentRegistrator<AndroidFlurryAnalyticsLoadHandlerComponent> AndroidFlurryAnalyticsLoadHandler (LOAD_HANDLER_COMPONENT_NAME);

}

}

}
