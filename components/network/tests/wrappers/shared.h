#ifndef NETWORK_WRAPPERS_TESTS_SHARED_HEADER
#define NETWORK_WRAPPERS_TESTS_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <mongoose.h>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/log.h>
#include <common/time.h>

#include <network/inet_address.h>
#include <network/url_connection.h>

using namespace network;

void print_log (const char* stream, const char* message)
{
  printf ("%s: %s\n", stream, message);
  fflush (stdout);
}

///Тестовый HTTP сервер
class TestHttpServer
{
  public:
    TestHttpServer ()
    {
       static const char *options[] = {
         "document_root", "./www",
         "listening_ports", "81",
         0
       };
    
      context = mg_start (0, options);
      
      if (!context)
        throw xtl::format_operation_exception ("network::TestHttpServer::TestHttpServer", "Can't create mongoose context");
    }
    
    ~TestHttpServer ()
    {
      mg_stop (context);
    }
  
  private:
    TestHttpServer (const TestHttpServer&); //no impl
    TestHttpServer& operator = (const TestHttpServer&); //no impl
  
  private:
    mg_context* context;
};

#endif
