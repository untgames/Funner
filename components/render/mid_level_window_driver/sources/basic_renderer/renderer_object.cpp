#include "shared.h"

using namespace render::mid_level::window_driver;

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
