#include "shared.h"

using namespace network;

namespace
{

/*
    Константы
*/

const char* COMPONENT_MASK = "network.url_stream.*"; //маска имени компонентов URL потоков

/*
    Обработчик URL запроса
*/

struct UrlQueryHandler: public xtl::reference_counter
{
  stl::string                         query_wcmask;   //регулярное выражение, соответствующее запросу
  UrlStreamManagerImpl::StreamCreator stream_creator; //создатель потока
  
  UrlQueryHandler (const char* wcmask, const UrlStreamManager::StreamCreator& creator)
    : query_wcmask (wcmask)
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

void UrlStreamManagerImpl::RegisterStreamCreator (const char* url_wcmask, const StreamCreator& creator)
{
  static const char* METHOD_NAME = "network::UrlStreamManagerImpl::RegisterStreamCreator";
  
  if (!url_wcmask)
    throw xtl::make_null_argument_exception (METHOD_NAME, "url_wcmask");
    
  for (UrlQueryHandlerList::iterator iter=impl->query_handlers.begin (), end=impl->query_handlers.end (); iter!=end; ++iter)
    if ((*iter)->query_wcmask == url_wcmask)
      throw xtl::make_argument_exception (METHOD_NAME, "url_wcmask", url_wcmask, "Stream creator has already registered");
      
  impl->query_handlers.push_front (UrlQueryHandlerPtr (new UrlQueryHandler (url_wcmask, creator), false));
}

void UrlStreamManagerImpl::UnregisterStreamCreator (const char* url_wcmask)
{
  if (!url_wcmask)
    return;
    
  for (UrlQueryHandlerList::iterator iter=impl->query_handlers.begin (), end=impl->query_handlers.end (); iter!=end; ++iter)
    if ((*iter)->query_wcmask == url_wcmask)
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
      
    static common::ComponentLoader loader (COMPONENT_MASK);
      
    for (UrlQueryHandlerList::iterator iter=impl->query_handlers.begin (), end=impl->query_handlers.end (); iter!=end; ++iter)
    {
      if (common::wcmatch (url, (*iter)->query_wcmask.c_str ()))
        return (*iter)->stream_creator (url, params, stream);
    }
    
    throw xtl::format_operation_exception ("", "Stream creator for URL '%s' not found", url);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlStreamManagerImpl::CreateStream");
    throw;
  }
}

/*
    Обёртки
*/

void UrlStreamManager::RegisterStreamCreator (const char* url_wcmask, const StreamCreator& creator)
{
  UrlStreamManagerSingleton::Instance ()->RegisterStreamCreator (url_wcmask, creator);
}

void UrlStreamManager::UnregisterStreamCreator (const char* url_wcmask)
{
  UrlStreamManagerSingleton::Instance ()->UnregisterStreamCreator (url_wcmask);
}

void UrlStreamManager::UnregisterAllStreamCreators ()
{
  UrlStreamManagerSingleton::Instance ()->UnregisterAllStreamCreators ();
}
