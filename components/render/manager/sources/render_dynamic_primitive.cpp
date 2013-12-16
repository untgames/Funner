#include "shared.h"

using namespace render::manager;

/*
    Описание реализации хранилища динамических примитивов
*/

namespace
{

struct DynamicPrimitiveDesc
{
  DynamicPrimitive*    primitive;
  xtl::auto_connection trackable_connection;

  DynamicPrimitiveDesc () : primitive () { }
};

}

typedef stl::vector<DynamicPrimitiveDesc>   DynamicPrimitiveArray;
typedef stl::vector<RendererPrimitiveGroup> GroupArray;

struct DynamicPrimitiveEntityStorage::Impl
{
  EntityImpl&           entity;             //ссылка на объект
  DynamicPrimitiveArray primitives;         //список динамических примитивов
  GroupArray            groups;             //группы примитивов рендеринга

/// Конструктор
  Impl (EntityImpl& in_entity)
    : entity (in_entity)
  {
  }
};

/*
    Конструктор / деструктор
*/

DynamicPrimitiveEntityStorage::DynamicPrimitiveEntityStorage (EntityImpl& entity)
  : impl (new Impl (entity))
{
}

DynamicPrimitiveEntityStorage::~DynamicPrimitiveEntityStorage ()
{
  RemoveAllPrimitives ();
}

/*
    Обновление
*/

void DynamicPrimitiveEntityStorage::UpdateOnPrerender ()
{
  try
  {
      //обновление примитивов

    for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (), end=impl->primitives.end (); iter!=end; ++iter)
      iter->primitive->UpdateOnPrerender (impl->entity);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveEntityStorage::UpdateOnPrerender");
    throw;
  }
}

void DynamicPrimitiveEntityStorage::UpdateOnRender (FrameImpl& frame)
{
  try
  {
      //обновление примитивов

    for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (), end=impl->primitives.end (); iter!=end; ++iter)
      iter->primitive->UpdateOnRender (impl->entity, frame);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveEntityStorage::UpdateOnRender");
    throw;
  }
}

/*
    Добавление динамических примитивов
*/

void DynamicPrimitiveEntityStorage::AddPrimitive (DynamicPrimitive* primitive)
{
  try
  {
    if (!primitive)
      throw xtl::make_null_argument_exception ("", "primitive");

    impl->primitives.push_back ();

    DynamicPrimitiveDesc& desc = impl->primitives.back ();

    try
    {
      desc.primitive            = primitive;
      desc.trackable_connection = primitive->connect_tracker (xtl::bind (&DynamicPrimitiveEntityStorage::RemovePrimitive, this, primitive));

      AttachCacheSource (*primitive);

      InvalidateCacheDependencies ();
    }
    catch (...)
    {
      impl->primitives.pop_back ();
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveEntityStorage::AddPrimitive");
    throw;
  }
}

void DynamicPrimitiveEntityStorage::RemovePrimitive (DynamicPrimitive* primitive)
{
  if (!primitive)
    return;

  for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (), end=impl->primitives.end (); iter!=end; ++iter)
    if (iter->primitive == primitive)
    {
      DetachCacheSource (*primitive);

      impl->primitives.erase (iter);

      InvalidateCacheDependencies ();

      return;
    }
}

void DynamicPrimitiveEntityStorage::RemoveAllPrimitives ()
{
  DetachAllCacheSources ();

  impl->primitives.clear ();

  InvalidateCacheDependencies ();
}

/*
    Группы примитивов рендеринга
*/

size_t DynamicPrimitiveEntityStorage::RendererPrimitiveGroupsCount ()
{
  UpdateCache ();

  return impl->groups.size ();
}

RendererPrimitiveGroup* DynamicPrimitiveEntityStorage::RendererPrimitiveGroups ()
{
  UpdateCache ();

  if (impl->groups.empty ())
    return 0;

  return &impl->groups [0];
}

/*
    Управление кэшированием
*/
      
void DynamicPrimitiveEntityStorage::UpdateCacheCore ()
{
  try
  {
      //очистка групп

    impl->groups.clear ();

      //обновление групп

    impl->groups.reserve (impl->primitives.size ());

    for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (), end=impl->primitives.end (); iter!=end; ++iter)
      impl->groups.push_back (iter->primitive->RendererPrimitiveGroup ());

      //обновление зависимостей

    InvalidateCacheDependencies ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicPrimitiveEntityStorage::UpdateCacheCore");
    throw;
  }
}

void DynamicPrimitiveEntityStorage::ResetCacheCore ()
{
  impl->groups.clear ();
}
