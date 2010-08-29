#ifndef NETWORK_WRAPPERS_SHARED_HEADER
#define NETWORK_WRAPPERS_SHARED_HEADER

#include <stl/auto_ptr.h>
#include <stl/list>
#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/singleton.h>
#include <common/strlib.h>

#include <syslib/thread.h>
#include <syslib/mutex.h>

#include <network/inet_address.h>
#include <network/socket.h>
#include <network/socket_address.h>
#include <network/url.h>
#include <network/url_connection.h>

#include <platform/platform.h>

namespace network
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация менеджера URL потоков
///////////////////////////////////////////////////////////////////////////////////////////////////
class UrlStreamManagerImpl
{
  public:
    typedef UrlStreamManager::StreamCreator StreamCreator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    UrlStreamManagerImpl  ();
    ~UrlStreamManagerImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация обработчиков URL запросов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterStreamCreator       (const char* url_regexp, const StreamCreator& creator);
    void UnregisterStreamCreator     (const char* url_regexp);
    void UnregisterAllStreamCreators ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание потока
///////////////////////////////////////////////////////////////////////////////////////////////////
    IUrlStream* CreateStream (const char* url, const char* params, IUrlStream::IListener& stream);
  
  private:
    UrlStreamManagerImpl (const UrlStreamManagerImpl&); //no impl
    UrlStreamManagerImpl& operator = (const UrlStreamManagerImpl&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

typedef common::Singleton<UrlStreamManagerImpl> UrlStreamManagerSingleton;

}

#endif
