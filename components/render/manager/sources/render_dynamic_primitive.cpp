#include "shared.h"

using namespace render::manager;

/*
    DynamicPrimitiveEntityStorage
*/

/*
    Описание реализации хранилища динамических примитивов
*/

namespace
{

struct DynamicPrimitiveDesc
{
  DynamicPrimitivePtr primitive;
  void*               source_tag;
  size_t              update_id;

  DynamicPrimitiveDesc (const DynamicPrimitivePtr& in_primitive, void* in_source_tag, size_t in_update_id)
    : primitive (in_primitive) 
    , source_tag (in_source_tag)
    , update_id (in_update_id)
  {
  }
};

}

typedef stl::vector<DynamicPrimitiveDesc>          DynamicPrimitiveArray;
typedef stl::vector<RendererDynamicPrimitiveGroup> GroupArray;

struct DynamicPrimitiveEntityStorage::Impl
{
  EntityImpl&           entity;     //ссылка на объект
  DynamicPrimitiveArray primitives; //список динамических примитивов
  GroupArray            groups;     //группы примитивов рендеринга
  size_t                update_id;  //идентификатор транзакции обновления

/// Конструктор
  Impl (EntityImpl& in_entity)
    : entity (in_entity)
    , update_id ()
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
    Добавление динамических примитивов
*/

void DynamicPrimitiveEntityStorage::AddPrimitive (const DynamicPrimitivePtr& primitive, void* source_tag)
{
  try
  {
    if (!primitive)
      throw xtl::make_null_argument_exception ("", "primitive");

    impl->primitives.push_back (DynamicPrimitiveDesc (primitive, source_tag, impl->update_id));

    try
    {
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

void DynamicPrimitiveEntityStorage::RemovePrimitive (const DynamicPrimitivePtr& primitive)
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
    Поиск примитива по тэгу
*/

DynamicPrimitivePtr DynamicPrimitiveEntityStorage::FindPrimitive (void* source_tag, bool touch)
{
  for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (), end=impl->primitives.end (); iter!=end; ++iter)
    if (iter->source_tag == source_tag)
    {
      if (touch)
        iter->update_id = impl->update_id;

      return iter->primitive;
    }

  return DynamicPrimitivePtr ();
}

/*
    Управление автоматической сборкой неиспользуемых динамических примитивов
*/

void DynamicPrimitiveEntityStorage::BeginUpdate ()
{
  impl->update_id++;
}

void DynamicPrimitiveEntityStorage::EndUpdate ()
{
  bool need_invalidate = false;

  for (DynamicPrimitiveArray::iterator iter=impl->primitives.begin (); iter!=impl->primitives.end ();)
    if (iter->update_id != impl->update_id)
    {
      DetachCacheSource (*iter->primitive);

      impl->primitives.erase (iter);

      need_invalidate = true;
    }
    else ++iter;
  
  if (need_invalidate)
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

RendererDynamicPrimitiveGroup* DynamicPrimitiveEntityStorage::RendererPrimitiveGroups ()
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
      impl->groups.push_back (RendererDynamicPrimitiveGroup (iter->primitive->RendererPrimitiveGroup (), &*iter->primitive));

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

/*
    DynamicPrimitive
*/

DynamicPrimitive::DynamicPrimitive (const render::manager::RendererPrimitiveGroup& in_group, size_t flags)
  : group (in_group)
  , frame_dependent ((flags & DynamicPrimitiveFlag_FrameDependent) != 0)
  , entity_dependent ((flags & DynamicPrimitiveFlag_EntityDependent) != 0)
  , cached_frame_id ()
  , cached_entity ()
  , cached_frame ()
{
}
