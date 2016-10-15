#include "shared.h"

using namespace analytics::flurry;

namespace
{

const char* LOG_NAME = "analytics.flurry.FlurrySessionImpl";

/*
   Flurry
*/

class FlurrySessionImpl
{
  public:
    ///Constructor
    FlurrySessionImpl ()
      : started (false)
    {
      Platform::GetReleaseVersion (release_version); //this must be called before 'StartSession' according to flurry documentation
    }

    ///Get version name of underlying library
    const char* GetReleaseVersion ()
    {
      return release_version.c_str ();
    }

    ///Start analytics
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

    ///Set analytics additional params
    void SetUserId (const char* user_id)
    {
      try
      {
        if (!user_id)
          throw xtl::make_null_argument_exception ("", "user_id");

        if (started)
          common::Log (LOG_NAME).Printf ("UserId can be set only before StartSession");

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
          common::Log (LOG_NAME).Printf ("Age can be set only before StartSession");

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
          common::Log (LOG_NAME).Printf ("Gender can be set only before StartSession");

        Platform::SetGender (gender);
      }
      catch (xtl::exception& e)
      {
        e.touch ("analytics::flurry::SetGender");
        throw;
      }
    }

    ///Events tracking
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

    ///Logging management
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
    bool        started;           //was session started
    stl::string release_version;   //version name of flurry library
};

typedef common::Singleton<FlurrySessionImpl> FlurrySessionSingleton;

}

/*
   Flurry
*/

/*
   Is analytics supported on current platform
*/

bool Flurry::IsSupported ()
{
  return Platform::IsSupported ();
}

/*
   Get version name of underlying library
*/

const char* Flurry::GetReleaseVersion ()
{
  return FlurrySessionSingleton::Instance ()->GetReleaseVersion ();
}

/*
   Start analytics
*/

void Flurry::StartSession (const char* api_key)
{
  FlurrySessionSingleton::Instance ()->StartSession (api_key);
}

/*
   Set analytics additional params
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

/*
   Events tracking
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
   Logging management
*/

void Flurry::SetLogLevel (LogLevel level)
{
  FlurrySessionSingleton::Instance ()->SetLogLevel (level);
}
