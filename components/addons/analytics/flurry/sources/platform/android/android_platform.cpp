#include "shared.h"

using namespace analytics::flurry;
using namespace analytics::flurry::android;

/*
   Android platform
*/

/*
   Is analytics supported on current platform
*/

bool AndroidPlatform::IsSupported ()
{
  return FlurrySingleton::Instance ()->IsSupported ();
}

/*
   Get version name of underlying library
*/

void AndroidPlatform::GetReleaseVersion (stl::string& version)
{
  FlurrySingleton::Instance ()->GetReleaseVersion (version);
}

/*
   Start analytics
*/

void AndroidPlatform::StartSession (const char* api_key)
{
  FlurrySingleton::Instance ()->StartSession (api_key);
}

/*
   Set analytics additional params
*/

void AndroidPlatform::SetUserId (const char* user_id)
{
  FlurrySingleton::Instance ()->SetUserId (user_id);
}

void AndroidPlatform::SetAge (size_t age)
{
  FlurrySingleton::Instance ()->SetAge (age);
}

void AndroidPlatform::SetGender (Gender gender)
{
  FlurrySingleton::Instance ()->SetGender (gender);
}

/*
   Events tracking
*/

void AndroidPlatform::LogEvent (const char* event, const common::PropertyMap& parameters, bool timed)
{
  FlurrySingleton::Instance ()->LogEvent (event, parameters, timed);
}

void AndroidPlatform::EndTimedEvent (const char* event, const common::PropertyMap& parameters)
{
  FlurrySingleton::Instance ()->EndTimedEvent (event, parameters);
}

void AndroidPlatform::LogPageView ()
{
  FlurrySingleton::Instance ()->LogPageView ();
}

/*
   Logging management
*/

void AndroidPlatform::SetDebugLogEnabled (bool state)
{
  FlurrySingleton::Instance ()->SetDebugLogEnabled (state);
}

void AndroidPlatform::SetLogLevel (LogLevel level)
{
  FlurrySingleton::Instance ()->SetLogLevel (level);
}
