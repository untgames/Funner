#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Получение семейства объекта
*/

void Object::GetFamily (ObjectFamily& family, void*& root)
{
  family.id     = FAMILY_ID;
  family.sub_id = 0;
  root          = static_cast<Object*> (this);
}
