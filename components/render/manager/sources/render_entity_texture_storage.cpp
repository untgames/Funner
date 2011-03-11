#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    Описание реализации хранилища текстур объекта
*/

struct EntityTextureStorage::Impl
{
  EntityImpl& owner; //владелец хранилища текстур
  
///Конструктор
  Impl (EntityImpl& in_owner) : owner (in_owner) {}
};

/*
    Конструктор / деструктор
*/

EntityTextureStorage::EntityTextureStorage (EntityImpl& entity)
  : impl (new Impl (entity))
{
}

EntityTextureStorage::~EntityTextureStorage ()
{
}

/*
    Начало и конец обновления
*/

void EntityTextureStorage::BeginUpdate ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void EntityTextureStorage::EndUpdate ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение текстур для материала
*/

size_t EntityTextureStorage::GetTextures (const MaterialPtr& material, TexturePtr textures [low_level::DEVICE_SAMPLER_SLOTS_COUNT])
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Обновление текстур
*/

void EntityTextureStorage::Update (const FramePtr& frame)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
