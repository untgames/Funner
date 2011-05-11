#include "shared.h"

using namespace render;

namespace
{

/*
    Константы
*/

const char* COMMON_LOG_NAME = "render.manager";       //имя протокола общих сообщений
const char* DEBUG_LOG_NAME  = "render.manager.debug"; //имя протокола отладочных сообщений

}

namespace render
{

/*
    Описание реализации протокола
*/

class LogImpl: public xtl::reference_counter
{
  public:
    common::Log log; //протокол

    LogImpl (const char* log_name) : log (log_name) {}
};

}

namespace
{

/*
    Хранилище для реализации протокола
*/

struct CommonImplHolder
{
  CommonImplHolder () : impl (new LogImpl (COMMON_LOG_NAME), false) {}

  xtl::intrusive_ptr<LogImpl> impl;
};

struct DebugImplHolder
{
  DebugImplHolder () : impl (new LogImpl (DEBUG_LOG_NAME), false) {}

  xtl::intrusive_ptr<LogImpl> impl;
};

}

/*
    Конструкторы / деструктор / присваивание
*/

Log::Log (LogSeverity severity)
{
  switch (severity)
  {
    case LogSeverity_Common:
    {
      typedef common::Singleton<CommonImplHolder> ImplSingleton;

      impl = ImplSingleton::Instance ()->impl.get ();
      break;
    }
    case LogSeverity_Debug:
    {
      typedef common::Singleton<DebugImplHolder> ImplSingleton;

      impl = ImplSingleton::Instance ()->impl.get ();
      break;
    }
    default:
      throw xtl::make_argument_exception ("render::Log::Log", "severity", severity);
  }

  addref (impl);
}

Log::Log (const Log& log)
  : impl (log.impl)
{
  addref (impl);
}

Log::~Log ()
{
  release (impl);
}

Log& Log::operator = (const Log& log)
{
  Log tmp = log;

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Печать сообщений
*/

void Log::Print (const char* message) const
{
  impl->log.Print (message);
}

void Log::Printf (const char* message, ...) const
{
  va_list list;
  
  va_start (list, message);  
  VPrintf  (message, list);
}

void Log::VPrintf (const char* message, va_list list) const
{
  impl->log.VPrintf (message, list);
}
