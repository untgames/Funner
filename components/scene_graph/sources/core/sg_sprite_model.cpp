#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации спрайтовой модели
*/

struct SpriteModel::Impl
{
  stl::string material; //имя материала
};

/*
    Конструктор / деструктор
*/

SpriteModel::SpriteModel ()
  : impl (new Impl)
{
}

SpriteModel::~SpriteModel ()
{
}

/*
    Материал
*/

void SpriteModel::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::SpriteModel::SetMaterial", "material");

  impl->material = in_material;
  
  UpdateNotify ();
}

const char* SpriteModel::Material () const
{
  return impl->material.c_str ();
}

/*
    Количество спрайтов / получение массива спрайтов
*/

size_t SpriteModel::SpriteDescsCount () const
{
  return const_cast<SpriteModel&> (*this).SpriteDescsCountCore ();
}

const SpriteModel::SpriteDesc* SpriteModel::SpriteDescs () const
{
  return const_cast<SpriteModel&> (*this).SpriteDescsCore ();
}

/*
    Динамическая диспетчеризация
*/

void SpriteModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
