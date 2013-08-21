#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

namespace
{

/// Контекст обработки
class ContextImpl: public Context
{
  public:
    ContextImpl (ConnectionState& state) : Context (state) {}

    using Context::SetCounterparty;
    using Context::ProcessCommands;
};

}

/*
    Описание реализации соединения
*/

struct Connection::Impl: public xtl::trackable
{
  xtl::trackable_ptr<ServerImpl> server;  //ссылка на сервер
  ConnectionState                state;   //состояние соединения
  ContextImpl                    context; //контекст

/// Конструктор
  Impl (ServerImpl& in_server, const char* init_string)
    : server (&in_server)
    , context (state)
  {
  } 
};

/*
    Конструктор / деструктор
*/

Connection::Connection (ServerImpl& server, const char* init_string)
{
  try
  {
      //must be thread neutral

    if (!init_string)
      init_string = "";

    impl = new Impl (server, init_string);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Connection::Connection");
    throw;
  }
}

Connection::~Connection ()
{
}

/*
    Рендер сцены
*/

ServerImpl& Connection::Server ()
{
  return *impl->server;
}

/*
    Контекст
*/

server::Context& Connection::Context ()
{
  return impl->context;
}

/*
    Состояние соединения
*/

ConnectionState& Connection::State ()
{
  return impl->state;
}

/*
    Обработка входного потока данных
*/

void Connection::ProcessCommands (const interchange::CommandBuffer& commands)
{
  try
  {
    if (!impl->server)
      throw xtl::format_operation_exception ("", "Server is lost");

    impl->context.ProcessCommands (commands);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::server::Connection::ProcessCommands");
    throw;
  }
}

/*
    Получение события оповещения об удалении
*/

xtl::trackable& Connection::GetTrackable ()
{
  return *impl;
}

/*
    Подсчет ссылок
*/

void Connection::AddRef  ()
{
  addref (this);
}

void Connection::Release ()
{
  release (this);
}
