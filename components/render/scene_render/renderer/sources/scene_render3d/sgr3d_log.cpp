#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "render.scene_render";

}

namespace render
{

namespace scene_render3d
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

}

namespace
{

/*
    Хранилище для реализации протокола
*/

struct ImplHolder
{
  ImplHolder () : impl (new LogImpl (LOG_NAME), false) {}

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
