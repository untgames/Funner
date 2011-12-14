#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "render.mid_level.Debug"; //имя потока протоколирования

}

/*
    Конструктор
*/

Object::Object ()
  : log (LOG_NAME)
{
  static size_t global_id = 1;

  id = global_id++;
  
  if (!global_id)
  {
    --global_id;
    throw xtl::format_operation_exception ("render::mid_level::debug::Object::Object", "No enough identifiers");
  }
}

/*
    Подсчёт ссылок
*/

void Object::AddRef ()
{
  addref (this);
}

void Object::Release ()
{
  release (this);
}
