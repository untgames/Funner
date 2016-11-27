#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Получение семейства объекта
*/

void Object::GetFamily (ObjectFamily& family, void*& root)
{
  family.id     = (size_t)&typeid (Object);
  family.sub_id = 0;
  root          = static_cast<Object*> (this);
}
