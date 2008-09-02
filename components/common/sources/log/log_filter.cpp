#include "shared.h"

using namespace common;

/*
===================================================================================================
    LogFilterImpl
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

LogFilterImpl::LogFilterImpl (const char* in_mask, const LogHandler& handler)
  : mask (in_mask ? in_mask : ""),
    log_handler (handler)
{
  if (!in_mask)
    throw xtl::make_null_argument_exception ("common::LogFilterImpl::LogFilterImpl", "mask");

  LogSystemImpl& log_system = LogSystemImpl::Lock ();

  try
  {
    log_system.Register (this);
  }
  catch (...)
  {
    throw;
  }
}

LogFilterImpl::~LogFilterImpl ()
{
  for (LogList::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
    (*iter)->RemoveFilter (this);

  LogSystemImpl::Instance ().Unregister (this);
  LogSystemImpl::Unlock ();
}

/*
    Протоколирование
*/

void LogFilterImpl::Print (const char* log_name, const char* log_message)
{
  try
  {
    log_handler (log_name, log_message);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Добавление / удаление источников
*/

void LogFilterImpl::AddSource (LogImpl* source)
{
  sources.push_back (source);
}

void LogFilterImpl::RemoveSource (LogImpl* source)
{
  sources.remove (source);
}

/*
===================================================================================================
    LogFilterImpl
===================================================================================================
*/

/*
    Конструктор/деструктор
*/

LogFilter::LogFilter (const char* log_name_mask, const LogHandler& handler)
  : impl (new LogFilterImpl (log_name_mask, handler))
{
}

LogFilter::LogFilter (const LogFilter& filter)
  : impl (filter.impl)
{
  addref (impl);
}

LogFilter::~LogFilter ()
{
  release (impl);
}

/*
    Присваивание
*/

LogFilter& LogFilter::operator = (const LogFilter& filter)
{
  LogFilter (filter).Swap (*this);

  return *this;
}

/*
    Получение маски
*/

const char* LogFilter::Mask () const
{
  return impl->Mask ();
}

/*
    Обмен
*/

void LogFilter::Swap (LogFilter& filter)
{
  stl::swap (impl, filter.impl);
}

namespace common
{

void swap (LogFilter& filter1, LogFilter& filter2)
{
  filter1.Swap (filter2);
}

}
