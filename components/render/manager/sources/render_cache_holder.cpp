#include "shared.h"

using namespace render::manager;

/*
===================================================================================================
    CacheHolder
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

CacheHolder::CacheHolder ()
  : state (CacheState_Reset)
  , need_update_this (true)  
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
    throw xtl::format_operation_exception ("render::manager::CacheHolder::AttachCacheSource", "Cache source has been already attached (to this holder or it's children)");
    
  if (source.IsParentOf (*this))
    throw xtl::format_operation_exception ("render::manager::CacheHolder::AttachCacheSource", "Cache source is a parent of this cache holder");  
    
  sources.push_back (&source);
  
  try
  {
    source.dependencies.push_back (this);

    InvalidateCache (false);
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
  source.dependencies.remove (this);

  InvalidateCache (false);
}

void CacheHolder::DetachAllCacheSources ()
{
  while (!sources.empty ())
    DetachCacheSource (*sources.back ());

  while (!dependencies.empty ())
    dependencies.back ()->DetachCacheSource (*this);
    
  InvalidateCache (false);    
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
    Установка флага необходимости обновления кэша / зависимых кэшей
*/

void CacheHolder::InvalidateFlags ()
{
  for (HolderList::iterator iter=dependencies.begin (), end=dependencies.end (); iter!=end; ++iter)
    (*iter)->InvalidateFlags ();

  switch (state)
  {
    case CacheState_Valid:
    case CacheState_Broken:    
      break;
    case CacheState_Reset:
    case CacheState_Invalid:
      return;
  }

  state = CacheState_Invalid;
}

void CacheHolder::InvalidateCache (bool invalidate_dependencies)
{
  if (need_update_this && !invalidate_dependencies)
    return;

  need_update_this = true;

  if (invalidate_dependencies)
  {
    for (HolderList::iterator iter=dependencies.begin (), end=dependencies.end (); iter!=end; ++iter)
      (*iter)->need_update_this = true;  
  }

  InvalidateFlags ();
}

/*
    Сброс кэша
*/

void CacheHolder::ResetCache ()
{
  switch (state)
  {
    case CacheState_Valid:
    case CacheState_Invalid:
    case CacheState_Broken:
      break;
    case CacheState_Reset:
      return;
  }    
  
  if (state != CacheState_Reset)
  {
    need_update_this = true;
        
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
    
    state = CacheState_Reset;
    
    for (HolderList::iterator iter=dependencies.begin (), end=dependencies.end (); iter!=end; ++iter)
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

  for (HolderList::iterator iter=dependencies.begin (), end=dependencies.end (); iter!=end; ++iter)
    (*iter)->UpdateCacheAfterReset ();
}

/*
    Обновление кэша. Необходимо вызывать перед каждым использованием ресурса
*/

void CacheHolder::UpdateCache ()
{
  switch (state)
  {    
    case CacheState_Invalid:
    case CacheState_Reset:    
      break;
    case CacheState_Broken:
    case CacheState_Valid:   
      return;
  }    

  for (HolderList::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
    (*iter)->UpdateCache ();
    
  if (need_update_this)
  {   
    try
    {
      UpdateCacheCore ();
    }
    catch (...)
    {
      InvalidateCacheDependencies ();
      
      state            = CacheState_Broken;
      need_update_this = false;

      throw;
    }
    
    need_update_this = false;
  } 
    
  state = CacheState_Valid;
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
