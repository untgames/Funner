#include "shared.h"

using namespace render::mid_level;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "render.mid_level"; //имя протокола

}

namespace render
{

namespace mid_level
{

/*
    Описание реализации протокола
*/

class LogImpl: public xtl::reference_counter
{
  public:
    common::Log log; //протокол

    LogImpl () : log (LOG_NAME) {}
};

}

}

namespace
{

/*
    Хранилище для реализации протокола
*/

struct ImplHolder
{
  ImplHolder () : impl (new LogImpl, false) {}

  xtl::intrusive_ptr<LogImpl> impl;
};

}

/*
    Конструкторы / деструктор / присваивание
*/

Log::Log ()
{
  typedef common::Singleton<ImplHolder> ImplSingleton;

  impl = ImplSingleton::Instance ()->impl.get ();

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
