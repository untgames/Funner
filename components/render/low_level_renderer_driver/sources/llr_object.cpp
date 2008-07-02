#include "shared.h"

using namespace render::mid_level::low_level_driver;

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
