#include <stl/string>
#include <stl/hash_map>

#include <xtl/connection.h>
#include <xtl/reference_counter.h>
#include <xtl/signal.h>
#include <xtl/shared_ptr.h>
#include <xtl/string.h>
#include <xtl/uninitialized_storage.h>

#include <common/log.h>
#include <common/singleton.h>
#include <common/exception.h>
#include <common/strlib.h>

using namespace common;

namespace
{

/*
   Реализация системы логгирования
*/
class LogSystemImpl
{  
  public:
/*
   Конструктор / деструктор
*/
    LogSystemImpl ()
    {
    }

    ~LogSystemImpl ()
    {
    }

/*
   Регистрация подписчиков на сообщения от групп
*/
    xtl::connection RegisterLogHandler (const char* log_name_mask, const LogSystem::LogHandler& handler)
    {
      LogSignalMap::iterator iter = log_signals_map.find (log_name_mask);

      if (iter == log_signals_map.end ())
        log_signals_map.insert_pair (log_name_mask, LogSignalPtr (new LogSignal));

      return log_signals_map[log_name_mask]->connect (handler);
    }
    
/*
   Отсылка лог сообщения
*/
    void Print (const char* log_name, const char* message)
    {
      for (LogSignalMap::iterator iter = log_signals_map.begin (), end = log_signals_map.end (); iter != end; ++iter)
        if (wcmatch (log_name, iter->first.c_str ()))
        {
          try
          {
            (*iter->second) (log_name, message);
          }
          catch (...)
          {
          }
        }
    }

    void VPrintf (const char* log_name, const char* message, va_list list)
    {
      xtl::uninitialized_storage<char> message_buf (xtl::xvsnprintf (NULL, 0, message, list) + 1);

      xtl::xvsnprintf (message_buf.data (), message_buf.size (), message, list);

      Print (log_name, message_buf.data ());
    }

  private:
    typedef xtl::signal<void (const char*, const char*)> LogSignal;
    typedef xtl::shared_ptr<LogSignal>                   LogSignalPtr;
    typedef stl::hash_map<stl::string, LogSignalPtr>     LogSignalMap;

  private:
    LogSignalMap log_signals_map;
};

/*
   Синглтон системы логгирования
*/
typedef Singleton<LogSystemImpl> LogSystemSingleton;

}

/*
   Описание реализации класса Log
*/

struct Log::Impl : public xtl::reference_counter
{
  public:
    Impl (const char* in_name) : name (in_name) {}

    const char* Name () const
    {
      return name.c_str ();
    }

    void Print (const char* message) const
    {
      LogSystemSingleton::Instance ().Print (name.c_str (), message);
    }

    void VPrintf (const char* message, va_list list) const
    {
      LogSystemSingleton::Instance ().VPrintf (name.c_str (), message, list);
    }

  public:
    stl::string name;
};

/*
   Именованный генератор лог-сообщений
*/

Log::Log (const char* name)
{
  if (!name)
    raise_null_argument ("common::Log::Log", "message");

  impl = new Impl (name);
}

Log::Log (const Log& source)
  : impl (source.impl)
{
  addref (impl);
}

Log::~Log ()
{
  release (impl);
}

/*
   Присваивание
*/

Log& Log::operator = (const Log& source)
{
  Log (source).Swap (*this);
    
  return *this;
}

/*
   Получение имени
*/

const char* Log::Name () const
{
  return impl->Name ();
}

/*
   Печать сообщений
*/

void Log::Print (const char* message) const
{
  if (!message)
    raise_null_argument ("common::Log::Print", "message");

  impl->Print (message);
}

void Log::Printf (const char* message, ...) const
{
  if (!message)
    raise_null_argument ("common::Log::Printf", "message");

  va_list list;
  
  va_start (list,message);

  impl->VPrintf (message, list);
}

void Log::VPrintf (const char* message, va_list list) const
{
  if (!message)
    raise_null_argument ("common::Log::VPrintf", "message");

  impl->VPrintf (message, list);
}

/*
   Обмен
*/

void Log::Swap (Log& source)
{
  stl::swap (impl, source.impl);
}

namespace common
{

/*
   Обмен
*/

void swap (Log& source1, Log& source2)
{
  source1.Swap (source2);
}

}

/*
   Система логгирования
*/

/*
   Регистрация подписчиков на сообщения от групп
*/

xtl::connection LogSystem::RegisterLogHandler (const char* log_name_mask, const LogHandler& handler)
{
  if (!log_name_mask)
    raise_null_argument ("common::LogSystem::RegisterLogHandler", "log_name_mask");

  return LogSystemSingleton::Instance ().RegisterLogHandler (log_name_mask, handler);
}
    
/*
   Отсылка лог сообщения
*/

void LogSystem::Print (const char* log_name, const char* message)
{
  static const char* METHOD_NAME = "common::LogSystem::Print";

  if (!log_name)
    raise_null_argument (METHOD_NAME, "log_name");

  if (!message)
    raise_null_argument (METHOD_NAME, "message");

  LogSystemSingleton::Instance ().Print (log_name, message);
}

void LogSystem::Printf (const char* log_name, const char* message, ...)
{
  static const char* METHOD_NAME = "common::LogSystem::Printf";

  if (!log_name)
    raise_null_argument (METHOD_NAME, "log_name");

  if (!message)
    raise_null_argument (METHOD_NAME, "message");

  va_list list;
  
  va_start (list,message);

  LogSystemSingleton::Instance ().VPrintf (log_name, message, list);
}

void LogSystem::VPrintf (const char* log_name, const char* message, va_list list)
{
  static const char* METHOD_NAME = "common::LogSystem::VPrintf";

  if (!log_name)
    raise_null_argument (METHOD_NAME, "log_name");

  if (!message)
    raise_null_argument (METHOD_NAME, "message");

  LogSystemSingleton::Instance ().VPrintf (log_name, message, list);
}
