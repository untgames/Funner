#include "shared.h"

using namespace analytics::flurry;

/*
   Платформа по умолчанию
*/

/*
   Поддерживается ли аналитика на данной платформе
*/

bool DefaultPlatform::IsSupported ()
{
  return false;
}

/*
   Получение имени версии используемой библиотеки
*/

void DefaultPlatform::GetReleaseVersion (stl::string& version)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::GetReleaseVersion");
}

/*
   Запуск аналитики
*/

void DefaultPlatform::StartSession (const char* api_key)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::StartSession");
}

/*
   Установка дополнительных параметров аналитики
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

void DefaultPlatform::SetUseHttps (bool use_https)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::SetUseHttps");
}

/*
   Трекинг событий
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
   Управление протоколированием
*/

void DefaultPlatform::SetDebugLogEnabled (bool state)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::SetDebugLogEnabled");
}

void DefaultPlatform::SetLogLevel (LogLevel level)
{
  throw xtl::make_not_implemented_exception ("analytics::flurry::DefaultPlatform::SetLogLevel");
}
