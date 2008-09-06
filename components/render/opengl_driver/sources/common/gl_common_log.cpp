#include "shared.h"

using namespace render::low_level::opengl;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "render.low_level.opengl"; //имя протокола

}

/*
    Описание реализации протокола
*/

struct Log::Impl: public xtl::reference_counter
{
  common::Log log; //протокол
  
  Impl () : log (LOG_NAME) {}
};

/*
    Конструкторы / деструктор / присваивание
*/

Log::Log ()
{
  struct ImplHolder
  {
    ImplHolder () : impl (new Impl, false) {}
    
    xtl::intrusive_ptr<Impl> impl;
  };

  typedef common::Singleton<ImplHolder> ImplSingleton;

  impl = ImplSingleton::Instance ().impl.get ();

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
