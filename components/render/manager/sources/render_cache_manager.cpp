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
  FrameTime time_delay;    //задержка по времени до удаления объекта из кэша
  FrameId   frame_delay;   //задержка по количеству кадров до удаления объекта из кэша
  FrameId   current_frame; //текущий кадр
  FrameTime current_time;  //текущее время
  
  Impl ()
    : time_delay (DEFAULT_TIME_DELAY)
    , frame_delay (DEFAULT_FRAME_DELAY)
    , current_frame (1)
    , current_time ()
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

void CacheManager::SetTimeDelay (FrameTime milliseconds)
{
  impl->time_delay = milliseconds;
}

void CacheManager::SetFrameDelay (FrameId frames_count)
{
  impl->frame_delay = frames_count;
}

FrameTime CacheManager::TimeDelay ()
{
  return impl->time_delay;
}

FrameId CacheManager::FrameDelay ()
{
  return impl->frame_delay;
}

/*
    Текущий кадр / текущий фрейм
*/

FrameTime CacheManager::CurrentTime ()
{
  return impl->current_time;
}

FrameId CacheManager::CurrentFrame ()
{
  return impl->current_frame - 1;
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

FrameTime Cache::TimeDelay ()
{
  return manager ? manager->TimeDelay () : 0;
}

FrameId Cache::FrameDelay ()
{
  return manager ? manager->FrameDelay () : 0;
}
