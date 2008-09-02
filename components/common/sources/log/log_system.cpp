#include "shared.h"

using namespace common;

/*
===================================================================================================
    LogSystemImpl
===================================================================================================
*/

namespace
{

LogSystemImpl* instance           = 0; //экземпляр системы протоколирования
size_t         instance_ref_count = 0; //количество ссылок на систему протоколирования

}

/*
    Конструктор / деструктор
*/

LogSystemImpl::LogSystemImpl ()
{
}

LogSystemImpl::~LogSystemImpl ()
{
  instance           = 0;
  instance_ref_count = 0;
}

/*
    Регистрация источников протоколирования
*/

void LogSystemImpl::Register (LogImpl* source)
{
  sources.push_back (source);
  
  try
  {  
    for (FilterList::iterator iter=filters.begin (), end=filters.end (); iter!=end; ++iter)
      if (wcmatch (source->Name (), (*iter)->Mask ()))
        source->AddFilter (*iter);
  }
  catch (...)
  {
    sources.pop_back ();
    throw;
  }  
}

void LogSystemImpl::Unregister (LogImpl* source)
{
  sources.remove (source);
}

/*
    Регистрация фильтров протоколирования
*/

void LogSystemImpl::Register (LogFilterImpl* filter)
{
  filters.push_back (filter);

  try
  {  
    for (LogList::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
      if (wcmatch ((*iter)->Name (), filter->Mask ()))
        (*iter)->AddFilter (filter);
  }
  catch (...)
  {
    filters.pop_back ();
    throw;
  }
}

void LogSystemImpl::Unregister (LogFilterImpl* filter)
{
  filters.remove (filter);
}

/*
    Получение и освобождение экземпляра системы протоколирования
*/

LogSystemImpl& LogSystemImpl::Instance ()
{
  return *instance;
}

LogSystemImpl& LogSystemImpl::Lock ()
{
  if (!instance)
  {
    instance           = new LogSystemImpl;
    instance_ref_count = 1;
  }
  else ++instance_ref_count;
  
  return *instance;
}

void LogSystemImpl::Unlock ()
{
  if (!instance || !instance_ref_count)
    return;
    
  if (!--instance_ref_count)
  {
    delete instance;
    
    instance           = 0;
    instance_ref_count = 0;
  }
}
