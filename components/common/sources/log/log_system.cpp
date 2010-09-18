#include "shared.h"

using namespace common;

/*
===================================================================================================
    LogSystemImpl
===================================================================================================
*/

namespace
{

Lockable       instance_lock;
LogSystemImpl* instance = 0; //экземпляр системы протоколирования

}

/*
    Конструктор / деструктор
*/

LogSystemImpl::LogSystemImpl ()
{
}

LogSystemImpl::~LogSystemImpl ()
{
  try
  {
    common::Lock lock1 (instance_lock);
    
    instance = 0;
  }
  catch (...)
  {
  }
}

/*
    Регистрация источников протоколирования
*/

void LogSystemImpl::Register (LogImpl* source)
{
  common::Lock lock (*this);

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
  common::Lock lock (*this);

  sources.remove (source);
}

/*
    Регистрация фильтров протоколирования
*/

void LogSystemImpl::Register (LogFilterImpl* filter)
{
  common::Lock lock (*this);

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
  common::Lock lock (*this);

  filters.remove (filter);
}

/*
    Получение и освобождение экземпляра системы протоколирования
*/

LogSystemPtr LogSystemImpl::Instance ()
{
  common::Lock lock (instance_lock);
  
  if (!instance)
  {
    instance = new LogSystemImpl;
    
    return LogSystemPtr (instance, false);
  }

  return instance;
}
