#include "shared.h"

using namespace render;

/*
===================================================================================================
    CacheHolder
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

CacheHolder::CacheHolder ()
  : need_update (true)
{
}

CacheHolder::~CacheHolder ()
{
  DetachAllCacheSources ();
}

/*
    Построение списка связанных хранителей кэша
*/

void CacheHolder::AttachCacheSource (CacheHolder& source)
{
  if (IsParentOf (source))
    throw xtl::format_operation_exception ("render::CacheHolder::AttachCacheSource", "source", "Cache source has been already attached (to this holder or it's children)");
    
  if (source.IsParentOf (*this))
    throw xtl::format_operation_exception ("render::CacheHolder::AttachCacheSource", "source", "Cache source is a parent of this cache holder");  

  sources.push_back (&source);
  
  try
  {
    source.parent_holders.push_back (this);

    Invalidate ();
  }
  catch (...)
  {
    sources.pop_back ();
    throw;
  }
}

void CacheHolder::DetachCacheSource (CacheHolder& source)
{
  sources.remove (&source);
  source.parent_holders.remove (this);
}

void CacheHolder::DetachAllCacheSources ()
{
  while (!sources.empty ())
    DetachCacheSource (*sources.back ());

  while (!parent_holders.empty ())
    parent_holders.back ()->DetachCacheSource (*this);
}

/*
    Является ли данный объект родительским по отношению к переданному
*/

bool CacheHolder::IsParentOf (CacheHolder& child)
{
  for (HolderList::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
  {
    if (*iter == &child)
      return true;
      
    if ((*iter)->IsParentOf (child))
      return true;
  }
  
  return false;
}

/*
    Отметка о необходимости обновления кэша источника
*/

void CacheHolder::Invalidate ()
{
  if (need_update)
    return;
    
  need_update = true;
  
  for (HolderList::iterator iter=parent_holders.begin (), end=parent_holders.end (); iter!=end; ++iter)
    (*iter)->Invalidate ();
}

/*
    Обновление кэша. Необходимо вызывать перед каждым использованием ресурса
*/

void CacheHolder::UpdateCache ()
{
  if (!need_update)
    return;
    
  UpdateCacheCore ();
  
  need_update = false;
}

/*
===================================================================================================
    CacheSource
===================================================================================================
*/

/*
    Кэшируемый источник требует обновления кэша.
*/

void CacheSource::UpdateCacheCore ()
{
}

void CacheSource::ResetCacheCore ()
{
  UpdateCache ();
}
