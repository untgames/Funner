#include "shared.h"

using namespace render;

/*
    Описание реализации настроек менеджера рендеринга
*/

struct Settings::Impl
{
  render::LogLevel log_level; //уровень протоколирования
  
  Impl ()
    : log_level (LogLevel_Default)
  {
  }
};

/*
    Конструктор / деструктор
*/

Settings::Settings ()
  : impl (new Impl)
{
}

Settings::~Settings ()
{
}

/*
    Управление протоколированием
*/

void Settings::SetLogLevel (render::LogLevel level)
{
  switch (level)
  {
    case LogLevel_Info:
    case LogLevel_Debug:
      break;
    default:
      throw xtl::make_argument_exception ("render::Settings::SetLogLevel", "level", level);
  }
  
  impl->log_level = level;
}

LogLevel Settings::LogLevel ()
{
  return impl->log_level;
}
