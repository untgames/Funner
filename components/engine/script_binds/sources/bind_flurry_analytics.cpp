#include "shared.h"

#include <analytics/flurry.h>

using namespace script;
using namespace analytics::flurry;

namespace
{

/*
    Константы
*/

const char* BINDER_NAME              = "Analytics.Flurry";
const char* COMPONENT_NAME           = "script.binds.Analytics.Flurry";
const char* FLURRY_LIBRARY           = "Analytics.Flurry";
const char* FLURRY_GENDER_LIBRARY    = "Analytics.FlurryGender";
const char* FLURRY_LOG_LEVEL_LIBRARY = "Analytics.FlurryLogLevel";

void bind_gender_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (FLURRY_GENDER_LIBRARY);

  lib.Register ("get_Male",   make_const (Gender_Male));
  lib.Register ("get_Female", make_const (Gender_Female));
}

void bind_log_level_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (FLURRY_LOG_LEVEL_LIBRARY);

  lib.Register ("get_None",         make_const (LogLevel_None));
  lib.Register ("get_CriticalOnly", make_const (LogLevel_CriticalOnly));
  lib.Register ("get_Debug",        make_const (LogLevel_Debug));
  lib.Register ("get_All",          make_const (LogLevel_All));
}

void bind_flurry_library (Environment& environment)
{
  InvokerRegistry lib = environment.Library (FLURRY_LIBRARY);

  lib.Register ("get_IsSupported",     make_invoker (&Flurry::IsSupported));
  lib.Register ("get_ReleaseVersion",  make_invoker (&Flurry::GetReleaseVersion));
  lib.Register ("set_UserId",          make_invoker (&Flurry::SetUserId));
  lib.Register ("set_Age",             make_invoker (&Flurry::SetAge));
  lib.Register ("set_Gender",          make_invoker (&Flurry::SetGender));
  lib.Register ("set_UseHttps",        make_invoker (&Flurry::SetUseHttps));
  lib.Register ("set_DebugLogEnabled", make_invoker (&Flurry::SetDebugLogEnabled));
  lib.Register ("set_LogLevel",        make_invoker (&Flurry::SetLogLevel));
  lib.Register ("StartSession",        make_invoker (&Flurry::StartSession));
  lib.Register ("LogEvent",            make_invoker (
      make_invoker (xtl::implicit_cast<void (*)(const char*, const common::PropertyMap&, bool)> (&Flurry::LogEvent)),
      make_invoker<void (const char*, const common::PropertyMap&)> (xtl::bind (xtl::implicit_cast<void (*)(const char*, const common::PropertyMap&, bool)> (&Flurry::LogEvent), _1, _2, false)),
      make_invoker (xtl::implicit_cast<void (*)(const char*, bool)> (&Flurry::LogEvent)),
      make_invoker<void (const char*)> (xtl::bind (xtl::implicit_cast<void (*)(const char*, bool)> (&Flurry::LogEvent), _1, false))
  ));
  lib.Register ("EndTimedEvent",       make_invoker (
      make_invoker (xtl::implicit_cast<void (*)(const char*, const common::PropertyMap&)> (&Flurry::EndTimedEvent)),
      make_invoker (xtl::implicit_cast<void (*)(const char*)> (&Flurry::EndTimedEvent))
  ));
  lib.Register ("LogPageView",         make_invoker (&Flurry::LogPageView));
}

void bind_flurry_analytics_libraries (Environment& environment)
{
  bind_gender_library    (environment);
  bind_log_level_library (environment);
  bind_flurry_library    (environment);
}

}

namespace components
{

namespace flurry_analytics_script_binds
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
      bind_flurry_analytics_libraries (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> FlurryAnalyticsScriptBind (COMPONENT_NAME);

}

}

}
