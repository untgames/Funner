#include "shared.h"

using namespace render::manager;

/*
===================================================================================================
    CacheHolder
===================================================================================================
*/

typedef stl::list<CacheHolder*> HolderList;

struct CacheHolder::Impl: public xtl::reference_counter
{
  HolderList dependencies;
  HolderList sources;
  CacheState state;
  bool       need_update_this;

  Impl ()
    : state (CacheState_Reset)
    , need_update_this (true)  
  {
  }  
};

/*
    Конструктор / деструктор
*/

CacheHolder::CacheHolder ()
  : impl (new Impl)
{
}

CacheHolder::~CacheHolder ()
{
  DetachAllCacheSources ();

  release (impl);
}

/*
    Состояние
*/

CacheState CacheHolder::State ()
{
  return impl->state;
}

/*
    Построение списка связанных хранителей кэша
*/

void CacheHolder::AttachCacheSource (CacheHolder& source)
{
  if (IsParentOf (source))
    throw xtl::format_operation_exception ("render::manager::CacheHolder::AttachCacheSource", "Cache source has been already attached (to this holder or it's children)");
    
  if (source.IsParentOf (*this))
    throw xtl::format_operation_exception ("render::manager::CacheHolder::AttachCacheSource", "Cache source is a parent of this cache holder");  
    
  impl->sources.push_back (&source);
  
  try
  {
    source.impl->dependencies.push_back (this);

    InvalidateCache (false);
  }
  catch (...)
  {
    impl->sources.pop_back ();
    throw;
  }
}

void CacheHolder::DetachCacheSource (CacheHolder& source)
{
  impl->sources.remove (&source);
  source.impl->dependencies.remove (this);

  InvalidateCache (false);
}

void CacheHolder::DetachAllCacheSources ()
{
  while (!impl->sources.empty ())
    DetachCacheSource (*impl->sources.back ());

  while (!impl->dependencies.empty ())
    impl->dependencies.back ()->DetachCacheSource (*this);
    
  InvalidateCache (false);    
}

/*
    Является ли данный объект родительским по отношению к переданному
*/

bool CacheHolder::IsParentOf (CacheHolder& child)
{
  for (HolderList::iterator iter=impl->sources.begin (), end=impl->sources.end (); iter!=end; ++iter)
  {
    if (*iter == &child)
      return true;
      
    if ((*iter)->IsParentOf (child))
      return true;
  }
  
  return false;
}

/*
    Установка флага необходимости обновления кэша / зависимых кэшей
*/

void CacheHolder::InvalidateFlags ()
{
  for (HolderList::iterator iter=impl->dependencies.begin (), end=impl->dependencies.end (); iter!=end; ++iter)
    (*iter)->InvalidateFlags ();

  switch (impl->state)
  {
    case CacheState_Valid:
    case CacheState_Broken:    
      break;
    case CacheState_Reset:
    case CacheState_Invalid:
      return;
  }

  impl->state = CacheState_Invalid;
}

void CacheHolder::InvalidateCache (bool invalidate_dependencies)
{
  if (impl->need_update_this && !invalidate_dependencies)
    return;

  impl->need_update_this = true;

  if (invalidate_dependencies)
  {
    for (HolderList::iterator iter=impl->dependencies.begin (), end=impl->dependencies.end (); iter!=end; ++iter)
      (*iter)->impl->need_update_this = true;  
  }

  InvalidateFlags ();
}

/*
    Сброс кэша
*/

void CacheHolder::ResetCache ()
{
  switch (impl->state)
  {
    case CacheState_Valid:
    case CacheState_Invalid:
    case CacheState_Broken:
      break;
    case CacheState_Reset:
      return;
  }    
  
  if (impl->state != CacheState_Reset)
  {
    impl->need_update_this = true;
        
    try
    {
      ResetCacheCore ();
    }
    catch (std::exception& e)
    {
      Log ().Printf ("Unexpected exception: %s\n    at render::manager::CacheHolder::ResetCache", e.what ());
    }
    catch (...)
    {
      Log ().Printf ("Unexpected exception at render::manager::CacheHolder::ResetCache");
    }
    
    impl->state = CacheState_Reset;
    
    for (HolderList::iterator iter=impl->dependencies.begin (), end=impl->dependencies.end (); iter!=end; ++iter)
      (*iter)->UpdateCacheAfterReset ();    
  }
}

void CacheHolder::UpdateCacheAfterReset ()
{
  try
  {
    UpdateCache ();
  }
  catch (std::exception& e)
  {
    Log ().Printf ("Unexpected exception: %s\n    at render::manager::CacheHolder::UpdateCacheAfterReset", e.what ());
  }
  catch (...)
  {
    Log ().Printf ("Unexpected exception at render::manager::CacheHolder::UpdateCacheAfterReset");
  }    

  for (HolderList::iterator iter=impl->dependencies.begin (), end=impl->dependencies.end (); iter!=end; ++iter)
    (*iter)->UpdateCacheAfterReset ();
}

/*
    Обновление кэша. Необходимо вызывать перед каждым использованием ресурса
*/

void CacheHolder::UpdateCache ()
{
  switch (impl->state)
  {    
    case CacheState_Invalid:
    case CacheState_Reset:    
      break;
    case CacheState_Broken:
    case CacheState_Valid:   
      return;
  }    

  for (HolderList::iterator iter=impl->sources.begin (), end=impl->sources.end (); iter!=end; ++iter)
    (*iter)->UpdateCache ();
    
  if (impl->need_update_this)
  {   
    try
    {
      UpdateCacheCore ();
    }
    catch (...)
    {
      InvalidateCacheDependencies ();
      
      impl->state            = CacheState_Broken;
      impl->need_update_this = false;

      throw;
    }
    
    impl->need_update_this = false;
  } 
    
  impl->state = CacheState_Valid;
}

/*
===================================================================================================
    CacheSource
===================================================================================================
*/

/*
    Кэшируемый источник требует обновления кэша
*/

void CacheSource::UpdateCacheCore ()
{
}

void CacheSource::ResetCacheCore ()
{
}
