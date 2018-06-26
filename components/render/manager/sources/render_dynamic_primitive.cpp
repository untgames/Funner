#include "shared.h"

using namespace render::manager;

/*
    DynamicPrimitiveEntityStorage
*/

const size_t CACHE_VALUE_RESERVE_SIZE = 8;

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
  struct CacheValueDesc
  {
    void*           source_tag;
    size_t          update_id;
    CacheValueBase* value;

    CacheValueDesc (CacheValueBase* in_value, void* in_source_tag, size_t in_update_id)
      : source_tag (in_source_tag)
      , update_id (in_update_id)
      , value (in_value)
    {
    }
  };

  typedef stl::vector<CacheValueDesc> CacheValueArray;

  EntityImpl&            entity;       //ссылка на объект
  DynamicPrimitiveArray  primitives;   //список динамических примитивов
  GroupArray             groups;       //группы примитивов рендеринга
  CacheValueArray        cache_values; //кэш временных значений
  size_t                 update_id;    //идентификатор транзакции обновления

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
  RemoveAllCacheValues ();
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

  for (Impl::CacheValueArray::iterator iter=impl->cache_values.begin (), end=impl->cache_values.end (); iter!=end;)
    if (iter->update_id != impl->update_id)
    {
      delete iter->value;

      impl->cache_values.erase (iter);
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
    {
      const RendererPrimitiveGroup* group = iter->primitive->RendererPrimitiveGroup ();

      if (group)
        impl->groups.push_back (RendererDynamicPrimitiveGroup (*group, iter->primitive.get ()));
    }

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
    Работа с кэшем временных значений
*/

DynamicPrimitiveEntityStorage::CacheValueBase* DynamicPrimitiveEntityStorage::FindCacheValueCore (const std::type_info& type, void* source_tag, bool touch)
{
  for (Impl::CacheValueArray::iterator it=impl->cache_values.begin (), end=impl->cache_values.end (); it!=end; ++it)
    if (it->source_tag == source_tag && it->value->type == &type)
    {
      if (touch)
        it->update_id = impl->update_id;

      return it->value;
    }

  return 0;
}

void DynamicPrimitiveEntityStorage::AddCacheValueCore (CacheValueBase* cache_value, void* source_tag)
{
  if (impl->cache_values.empty ())
    impl->cache_values.reserve (CACHE_VALUE_RESERVE_SIZE);

  impl->cache_values.push_back (Impl::CacheValueDesc (cache_value, source_tag, impl->update_id));
}

void DynamicPrimitiveEntityStorage::RemoveCacheValueCore (const std::type_info& type, void* source_tag)
{
  for (Impl::CacheValueArray::iterator it=impl->cache_values.begin (), end=impl->cache_values.end (); it!=end; ++it)
    if (it->value->type == &type && it->source_tag == source_tag)
    {
      delete it->value;

      impl->cache_values.erase (it);

      break;
    }
}

void DynamicPrimitiveEntityStorage::RemoveAllCacheValues ()
{
  for (Impl::CacheValueArray::iterator it=impl->cache_values.begin (), end=impl->cache_values.end (); it!=end; ++it)
    delete it->value;

  impl->cache_values.clear ();
}

/*
    DynamicPrimitive
*/

DynamicPrimitive::DynamicPrimitive (const render::manager::RendererPrimitiveGroup* in_group, size_t flags)
  : group (in_group)
  , frame_dependent ((flags & DynamicPrimitiveFlag_FrameDependent) != 0)
  , entity_dependent ((flags & DynamicPrimitiveFlag_EntityDependent) != 0)
  , cached_frame_id ()
  , cached_entity ()
  , cached_frame ()
{
}
