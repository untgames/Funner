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

typedef xtl::com_ptr<interchange::IConnection> ConnectionPtr;

struct Connection::Impl: public xtl::trackable
{
  xtl::trackable_ptr<ServerImpl> server;               //ссылка на сервер
  ConnectionState                state;                //состояние соединения
  ContextImpl                    context;              //контекст
  ConnectionPtr                  response_connection;  //обратное соединение с клиентом

/// Конструктор
  Impl (ServerImpl& in_server, const char* init_string)
    : server (&in_server)
    , context (state)
  {
    common::PropertyMap properties = common::parse_init_string (init_string);

    stl::string initiator = properties.GetString ("initiator");

    response_connection = ConnectionPtr (interchange::ConnectionManager::CreateConnection (initiator.c_str (), "initiator=render::scene::server::Connection"), false);

    context.SetCounterparty (response_connection.get ());
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
