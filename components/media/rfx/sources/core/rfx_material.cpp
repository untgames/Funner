#include "shared.h"

using namespace media::rfx;
using namespace media;
using namespace common;

/*
    Описание реализации Material
*/

typedef stl::bitset<MaterialPin_Num> PinSet;

struct Material::Impl: public xtl::reference_counter
{
  stl::string name;       //имя материала
  size_t      name_hash;  //хэш имени
  PinSet      pins;       //пины  
  int         sort_group; //группа сортировки  

  Impl () : sort_group (0), name_hash (strhash ("")) {}
};

/*
    Конструкторы / деструктор
*/

Material::Material ()
  : impl (new Impl)
  {}

Material::Material (const Material& material)
  : impl (new Impl (*material.impl))
  {}

Material::~Material ()
{
}

/*
    Копирование
*/

Material::Pointer Material::Clone () const
{
  return Pointer (CloneCore (), false);
}

Material* Material::CloneCore () const
{
  return new Material (*this);
}

/*
    Имя материала
*/

const char* Material::Name () const
{
  return impl->name.c_str ();
}

size_t Material::NameHash () const
{
  return impl->name_hash;
}

void Material::Rename (const char* name)
{
  if (!name)
    raise_null_argument ("media::rfx::Material::Rename", "name");
        
  impl->name      = name;
  impl->name_hash = strhash (name);
}

/*
    Работа с логическими свойствами
*/

bool Material::IsEnabled (MaterialPin pin) const
{
  if (pin < 0 || pin >= MaterialPin_Num)
    raise_invalid_argument ("media::rfx::Material::IsEnabled", "pin", pin);
    
  return impl->pins.test (pin);
}

void Material::SetPin (MaterialPin pin, bool state)
{
  if (pin < 0 || pin >= MaterialPin_Num)
    raise_invalid_argument ("media::rfx::Material::SetPin", "pin", pin); 
    
  impl->pins.set (pin, state);
}

/*
    Динамическая диспетчеризация
*/

void Material::Accept (Visitor& visitor)
{
  AcceptCore (visitor);
}

void Material::AcceptCore (Visitor& visitor)
{
  visitor (*this);
}

/*
    Установка группы сортировки
*/

void Material::SetSortGroup (int group)
{
  impl->sort_group = group;
}

int Material::SortGroup () const
{
  return impl->sort_group;
}

/*
    Подсчёт ссылок
*/

void Material::AddRef () const
{
  addref (*impl);
}

void Material::Release () const
{
  release (impl.get ());
}

/*
    Утилиты
*/

namespace media
{

namespace rfx
{

/*
    Получение имени пина
*/

const char* get_name (MaterialPin pin)
{
  switch (pin)
  {
    case MaterialPin_TwoSided:        return "two_sided";
    case MaterialPin_Wireframe:       return "wireframe";
    case MaterialPin_Lighting:        return "lighting";
    case MaterialPin_CastShadows:     return "cast_shadows";
    case MaterialPin_ReceiveShadows:  return "recv_shadows";
    case MaterialPin_SelfShadow:      return "self_shadow";
    default:                          raise_invalid_argument ("media::rfx::get_name(MaterialPin)", "pin", pin);
  }

  return "";
}

}

}
