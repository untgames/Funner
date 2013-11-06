#include "shared.h"

using namespace analytics::flurry;

namespace
{

/*
   Flurry
*/

class FlurrySessionImpl
{
  public:
    ///Конструктор
    FlurrySessionImpl ()
      : started (false)
    {
      Platform::GetReleaseVersion (release_version); //this must be called before 'StartSession' according to flurry documentation
    }

    ///Получение имени версии используемой библиотеки
    const char* GetReleaseVersion ()
    {
      return release_version.c_str ();
    }

    ///Запуск аналитики
    void StartSession (const char* api_key)
    {
      try
      {
        if (!api_key)
          throw xtl::make_null_argument_exception ("", "api_key");

        if (started)
          throw xtl::format_operation_exception ("", "Session already started");

        Platform::StartSession (api_key);

        started = true;
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::StartSession");
        throw;
      }
    }

    ///Установка дополнительных параметров аналитики
    void SetUserId (const char* user_id)
    {
      try
      {
        if (!user_id)
          throw xtl::make_null_argument_exception ("", "user_id");

        if (started)
          throw xtl::format_operation_exception ("", "UserId can be set only before StartSession");

        Platform::SetUserId (user_id);
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::SetUserId");
        throw;
      }
    }

    void SetAge (size_t age)
    {
      try
      {
        if (started)
          throw xtl::format_operation_exception ("", "Age can be set only before StartSession");

        Platform::SetAge (age);
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::SetAge");
        throw;
      }
    }

    void SetGender (Gender gender)
    {
      try
      {
        if (started)
          throw xtl::format_operation_exception ("", "Gender can be set only before StartSession");

        Platform::SetGender (gender);
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::SetGender");
        throw;
      }
    }

    void SetUseHttps (bool use_https)
    {
      try
      {
        Platform::SetUseHttps (use_https);
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::SetUseHttps");
        throw;
      }
    }

    ///Трекинг событий
    void LogEvent (const char* event, const common::PropertyMap& parameters, bool timed)
    {
      try
      {
        if (!event)
          throw xtl::make_null_argument_exception ("", "event");

        if (!started)
          throw xtl::format_operation_exception ("", "Can't log event before StartSession");

        Platform::LogEvent (event, parameters, timed);
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::LogEvent (const char*, const common::PropertyMap&, bool)");
        throw;
      }
    }

    void EndTimedEvent (const char* event, const common::PropertyMap& parameters)
    {
      try
      {
        if (!event)
          throw xtl::make_null_argument_exception ("", "event");

        if (!started)
          throw xtl::format_operation_exception ("", "Can't log event before StartSession");

        Platform::EndTimedEvent (event, parameters);
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::EndTimedEvent");
        throw;
      }
    }

    void LogPageView ()
    {
      try
      {
        if (!started)
          throw xtl::format_operation_exception ("", "Can't log page view before StartSession");

        Platform::LogPageView ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::LogPageView");
        throw;
      }
    }

    ///Управление протоколированием
    void SetDebugLogEnabled (bool state)
    {
      try
      {
        Platform::SetDebugLogEnabled (state);
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::SetDebugLogEnabled");
        throw;
      }
    }

    void SetLogLevel (LogLevel level)
    {
      try
      {
        Platform::SetLogLevel (level);
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::SetLogLevel");
        throw;
      }
    }

  private:
    bool        started;           //запущена ли сессия
    stl::string release_version;   //версия используемой библиотеки flurry
};

typedef common::Singleton<FlurrySessionImpl> FlurrySessionSingleton;

}

/*
   Flurry
*/

/*
   Поддерживается ли аналитика на данной платформе
*/

bool Flurry::IsSupported ()
{
  return Platform::IsSupported ();
}

/*
   Получение имени версии используемой библиотеки
*/

const char* Flurry::GetReleaseVersion ()
{
  return FlurrySessionSingleton::Instance ()->GetReleaseVersion ();
}

/*
   Запуск аналитики
*/

void Flurry::StartSession (const char* api_key)
{
  FlurrySessionSingleton::Instance ()->StartSession (api_key);
}

/*
   Установка дополнительных параметров аналитики
*/

void Flurry::SetUserId (const char* user_id)
{
  FlurrySessionSingleton::Instance ()->SetUserId (user_id);
}

void Flurry::SetAge (size_t age)
{
  FlurrySessionSingleton::Instance ()->SetAge (age);
}

void Flurry::SetGender (Gender gender)
{
  FlurrySessionSingleton::Instance ()->SetGender (gender);
}

void Flurry::SetUseHttps (bool use_https)
{
  FlurrySessionSingleton::Instance ()->SetUseHttps (use_https);
}

/*
   Трекинг событий
*/

void Flurry::LogEvent (const char* event, bool timed)
{
  FlurrySessionSingleton::Instance ()->LogEvent (event, common::PropertyMap (), timed);
}

void Flurry::LogEvent (const char* event, const common::PropertyMap& parameters, bool timed)
{
  FlurrySessionSingleton::Instance ()->LogEvent (event, parameters, timed);
}

void Flurry::EndTimedEvent (const char* event)
{
  FlurrySessionSingleton::Instance ()->EndTimedEvent (event, common::PropertyMap ());
}

void Flurry::EndTimedEvent (const char* event, const common::PropertyMap& parameters)
{
  FlurrySessionSingleton::Instance ()->EndTimedEvent (event, parameters);
}

void Flurry::LogPageView ()
{
  FlurrySessionSingleton::Instance ()->LogPageView ();
}

/*
   Управление протоколированием
*/

void Flurry::SetDebugLogEnabled (bool state)
{
  FlurrySessionSingleton::Instance ()->SetDebugLogEnabled (state);
}

void Flurry::SetLogLevel (LogLevel level)
{
  FlurrySessionSingleton::Instance ()->SetLogLevel (level);
}
