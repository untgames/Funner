#include "shared.h"

using namespace network;

namespace
{

/*
    Обработчик URL запроса
*/

struct UrlQueryHandler: public xtl::reference_counter
{
  stl::string                         query_regexp;   //регулярное выражение, соответствующее запросу
  UrlStreamManagerImpl::StreamCreator stream_creator; //создатель потока
  
  UrlQueryHandler (const char* regexp, const UrlStreamManager::StreamCreator& creator)
    : query_regexp (regexp)
    , stream_creator (creator)
  {
  }
};

typedef xtl::intrusive_ptr<UrlQueryHandler> UrlQueryHandlerPtr;
typedef stl::list<UrlQueryHandlerPtr>       UrlQueryHandlerList;

}

/*
    Описание реализации менеджера URL потоков
*/

struct UrlStreamManagerImpl::Impl
{
  UrlQueryHandlerList query_handlers; //обработчики запросов
};

/*
    Конструкторы / деструктор
*/

UrlStreamManagerImpl::UrlStreamManagerImpl ()
  : impl (new Impl)
{
}

UrlStreamManagerImpl::~UrlStreamManagerImpl ()
{
}

/*
    Регистрация обработчиков URL запросов
*/

void UrlStreamManagerImpl::RegisterStreamCreator (const char* url_regexp, const StreamCreator& creator)
{
  static const char* METHOD_NAME = "network::UrlStreamManagerImpl::RegisterStreamCreator";
  
  if (!url_regexp)
    throw xtl::make_null_argument_exception (METHOD_NAME, "url_regexp");
    
  for (UrlQueryHandlerList::iterator iter=impl->query_handlers.begin (), end=impl->query_handlers.end (); iter!=end; ++iter)
    if ((*iter)->query_regexp == url_regexp)
      throw xtl::make_argument_exception (METHOD_NAME, "url_regexp", url_regexp, "Stream creator has already registered");
      
  impl->query_handlers.push_front (UrlQueryHandlerPtr (new UrlQueryHandler (url_regexp, creator), false));
}

void UrlStreamManagerImpl::UnregisterStreamCreator (const char* url_regexp)
{
  if (!url_regexp)
    return;
    
  for (UrlQueryHandlerList::iterator iter=impl->query_handlers.begin (), end=impl->query_handlers.end (); iter!=end; ++iter)
    if ((*iter)->query_regexp == url_regexp)
    {
      impl->query_handlers.erase (iter);
      return;
    }
}

void UrlStreamManagerImpl::UnregisterAllStreamCreators ()
{
  impl->query_handlers.clear ();
}

/*
    Создание потока
*/

IUrlStream* UrlStreamManagerImpl::CreateStream (const char* url, const char* params, IUrlStream::IListener& stream)
{
  try
  {
    if (!url)
      throw xtl::make_null_argument_exception ("", "url");
      
    if (!params)
      params = "";
      
    for (UrlQueryHandlerList::iterator iter=impl->query_handlers.begin (), end=impl->query_handlers.end (); iter!=end; ++iter)
      if (common::rematch (url, (*iter)->query_regexp.c_str ()))
        return (*iter)->stream_creator (url, params, stream);
    
    throw xtl::format_operation_exception ("", "Stream creator for URL '%s' not found", url);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlStreamManagerImpl::CreateStream");
    throw;
  }
}
