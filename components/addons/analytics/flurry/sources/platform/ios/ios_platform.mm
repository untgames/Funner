#include "shared.h"

using namespace analytics::flurry;

namespace
{

const char* LOG_NAME = "analytics::flurry::IOsPlatform";

NSDictionary* property_map_to_ns_dictionary (const common::PropertyMap& parameters)
{
  size_t parameters_count = parameters.Size ();

  if (!parameters_count)
    return [NSDictionary dictionary];

  NSMutableDictionary* return_value = [NSMutableDictionary dictionaryWithCapacity:parameters_count];

  for (size_t i = 0; i < parameters_count; i++)
  {
    NSString* key   = [NSString stringWithUTF8String:parameters.PropertyName (i)];
    NSString* value = [NSString stringWithUTF8String:parameters.GetString (i)];

    [return_value setObject:value forKey:key];
  }

  return return_value;
}

}

/*
   iOS platform
*/

/*
   Is analytics supported on current platform
*/

bool IOsPlatform::IsSupported ()
{
  return true;
}

/*
   Get version name of underlying library
*/

void IOsPlatform::GetReleaseVersion (stl::string& version)
{
  version = [[::Flurry getFlurryAgentVersion] UTF8String];
}

/*
   Start analytics
*/

void IOsPlatform::StartSession (const char* api_key)
{
  NSDictionary* launch_options = syslib::iphone::ApplicationManager::GetLaunchOptions ();

  if (launch_options)
    [::Flurry startSession:[NSString stringWithUTF8String:api_key] withOptions:launch_options];
  else
    [::Flurry startSession:[NSString stringWithUTF8String:api_key]];
}

/*
   Set analytics additional params
*/

void IOsPlatform::SetUserId (const char* user_id)
{
  [::Flurry setUserID:[NSString stringWithUTF8String:user_id]];
}

void IOsPlatform::SetAge (size_t age)
{
  [::Flurry setAge:(int)age];
}

void IOsPlatform::SetGender (Gender gender)
{
  NSString* flurry_gender;

  switch (gender)
  {
    case Gender_Male:
      flurry_gender = @"m";
      break;
    case Gender_Female:
      flurry_gender = @"f";
      break;
    default:
      throw xtl::make_argument_exception ("analytics::flurry::IOsPlatform::SetGender", "gender", gender);
  }

  [::Flurry setGender:flurry_gender];
}

/*
   Events tracking
*/

void IOsPlatform::LogEvent (const char* event, const common::PropertyMap& parameters, bool timed)
{
  switch ([::Flurry logEvent:[NSString stringWithUTF8String:event] withParameters:property_map_to_ns_dictionary (parameters) timed:timed])
  {
    case FlurryEventFailed:
      common::Log (LOG_NAME).Printf ("Log event '%s' failed", event);
      break;
    case FlurryEventRecorded:  //do nothing
      break;
    case FlurryEventUniqueCountExceeded:
      common::Log (LOG_NAME).Printf ("Log event '%s' failed, unique count exceeded", event);
      break;
    case FlurryEventParamsCountExceeded:
      common::Log (LOG_NAME).Printf ("Log event '%s' failed, params count exceeded", event);
      break;
    case FlurryEventLogCountExceeded:
      common::Log (LOG_NAME).Printf ("Log event '%s' failed, log count exceeded", event);
      break;
    case FlurryEventLoggingDelayed:
      common::Log (LOG_NAME).Printf ("Log event '%s' logging delayed", event);
      break;
  }
}

void IOsPlatform::EndTimedEvent (const char* event, const common::PropertyMap& parameters)
{
  [::Flurry endTimedEvent:[NSString stringWithUTF8String:event] withParameters:property_map_to_ns_dictionary (parameters)];
}

void IOsPlatform::LogPageView ()
{
  [::Flurry logPageView];
}

/*
   Logging management
*/

void IOsPlatform::SetDebugLogEnabled (bool state)
{
  [::Flurry setDebugLogEnabled:state];
}

void IOsPlatform::SetLogLevel (LogLevel level)
{
  FlurryLogLevel flurry_level;

  switch (level)
  {
    case LogLevel_None:
      flurry_level = FlurryLogLevelNone;
      break;
    case LogLevel_CriticalOnly:
      flurry_level = FlurryLogLevelCriticalOnly;
      break;
    case LogLevel_Debug:
      flurry_level = FlurryLogLevelDebug;
      break;
    case LogLevel_All:
      flurry_level = FlurryLogLevelAll;
      break;
    default:
      throw xtl::make_argument_exception ("analytics::flurry::IOsPlatform::SetLogLevel", "level", level);
  }

  [::Flurry setLogLevel:flurry_level];
}
