#include "shared.h"

using namespace analytics::flurry;

/*
   Default platform
*/

/*
   Is analytics supported on current platform
*/

bool DefaultPlatform::IsSupported ()
{
  return false;
}

/*
   Get version name of underlying library
*/

void DefaultPlatform::GetReleaseVersion (stl::string& version)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::GetReleaseVersion");
}

/*
   Start analytics
*/

void DefaultPlatform::StartSession (const char* api_key)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::StartSession");
}

/*
   Set analytics additional params
*/

void DefaultPlatform::SetUserId (const char* user_id)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::SetUserId");
}

void DefaultPlatform::SetAge (size_t age)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::SetAge");
}

void DefaultPlatform::SetGender (Gender gender)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::SetGender");
}

/*
   Events tracking
*/

void DefaultPlatform::LogEvent (const char* event, const common::PropertyMap& parameters, bool timed)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::LogEvent");
}

void DefaultPlatform::EndTimedEvent (const char* event, const common::PropertyMap& parameters)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::EndTimedEvent");
}

void DefaultPlatform::LogPageView ()
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::LogPageView");
}

/*
   Logging management
*/

void DefaultPlatform::SetDebugLogEnabled (bool state)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::SetDebugLogEnabled");
}

void DefaultPlatform::SetLogLevel (LogLevel level)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::SetLogLevel");
}
