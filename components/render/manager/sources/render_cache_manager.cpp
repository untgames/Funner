#include "shared.h"

using namespace render::manager;

/*
    Константы
*/

namespace
{

const size_t DEFAULT_TIME_DELAY  = 3600 * 1000; //задержка по времени до удаления объекта из кэша по умолчанию
const size_t DEFAULT_FRAME_DELAY = ~0u - 1;     //задержка по количеству кадров до удаления объекта из кэша по умолчанию

}

/*
===================================================================================================
    CacheManager
===================================================================================================
*/

/*
    Описание реализации кэша
*/

typedef stl::list<Cache*> CacheList;

struct CacheManager::Impl
{
  CacheList caches;        //список кэшей
  size_t    time_delay;    //задержка по времени до удаления объекта из кэша
  size_t    frame_delay;   //задержка по количеству кадров до удаления объекта из кэша
  size_t    current_frame; //текущий кадр
  size_t    current_time;  //текущее время
  
  Impl ()
    : time_delay (DEFAULT_TIME_DELAY)
    , frame_delay (DEFAULT_FRAME_DELAY)
    , current_frame (0)
    , current_time (0)
  {
  }
};

/*
    Конструктор / деструктор
*/

CacheManager::CacheManager ()
  : impl (new Impl)
{
}

CacheManager::~CacheManager ()
{
}

/*
    Параметры кэширования
*/

void CacheManager::SetTimeDelay (size_t milliseconds)
{
  impl->time_delay = milliseconds;
}

void CacheManager::SetFrameDelay (size_t frames_count)
{
  impl->frame_delay = frames_count;
}

size_t CacheManager::TimeDelay ()
{
  return impl->time_delay;
}

size_t CacheManager::FrameDelay ()
{
  return impl->frame_delay;
}

/*
    Обновление маркеров времени
*/

void CacheManager::UpdateMarkers ()
{
  impl->current_time = common::milliseconds ();

  for (CacheList::iterator iter=impl->caches.begin (), end=impl->caches.end (); iter!=end; ++iter)
  {
    (*iter)->current_frame = impl->current_frame;
    (*iter)->current_time  = impl->current_time;
  }
  
  impl->current_frame++;
}

/*
    Сброс кэшей
*/

void CacheManager::FlushCaches ()
{
  for (CacheList::iterator iter=impl->caches.begin (), end=impl->caches.end (); iter!=end; ++iter)
    (*iter)->FlushCache ();
}

/*
    Добавление / удаление кэша
*/

void CacheManager::AddCache (Cache& cache)
{
  RemoveCache (cache);
  
  impl->caches.push_back (&cache);
}

void CacheManager::RemoveCache (Cache& cache)
{
  for (CacheList::iterator iter=impl->caches.begin (), end=impl->caches.end (); iter!=end; ++iter)
    if (*iter == &cache)
    {
      impl->caches.erase (iter);
      return;
    }
}

/*
===================================================================================================
    Cache
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

Cache::Cache (const CacheManagerPtr& in_manager)
  : manager (in_manager)
{  
  if (manager)
  {
    current_frame = manager->impl->current_frame;
    current_time  = manager->impl->current_time;
  
    manager->AddCache (*this);
  }
}

Cache::~Cache ()
{
  if (manager)
    manager->RemoveCache (*this);
}

/*
    Параметры кэширования
*/

size_t Cache::TimeDelay ()
{
  return manager ? manager->TimeDelay () : 0;
}

size_t Cache::FrameDelay ()
{
  return manager ? manager->FrameDelay () : 0;
}
