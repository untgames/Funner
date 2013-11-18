#include "shared.h"

using namespace analytics::flurry;

namespace
{

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
   Платформа по умолчанию
*/

/*
   Поддерживается ли аналитика на данной платформе
*/

bool IOsPlatform::IsSupported ()
{
  return true;
}

/*
   Получение имени версии используемой библиотеки
*/

void IOsPlatform::GetReleaseVersion (stl::string& version)
{
  version = [[::Flurry getFlurryAgentVersion] UTF8String];
}

/*
   Запуск аналитики
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
   Установка дополнительных параметров аналитики
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

void IOsPlatform::SetUseHttps (bool use_https)
{
  [::Flurry setSecureTransportEnabled:use_https];
}

/*
   Трекинг событий
*/

void IOsPlatform::LogEvent (const char* event, const common::PropertyMap& parameters, bool timed)
{
  [::Flurry logEvent:[NSString stringWithUTF8String:event] withParameters:property_map_to_ns_dictionary (parameters) timed:timed];
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
   Управление протоколированием
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
