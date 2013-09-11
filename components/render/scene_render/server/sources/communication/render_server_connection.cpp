#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

namespace
{

/// Контекст обработки
class ContextImpl: public Context
{
  public:
    ContextImpl (ConnectionState& in_state) : Context (in_state), state (in_state) {}

    using Context::SetCounterparty;
    using Context::ProcessCommands;

    bool DeserializeUnknownCommand (interchange::CommandId id, InputStream& stream) 
    { 
      return ServerLoopbackConnection::ProcessIncomingCommand (static_cast<InternalCommandId> (id), stream, state); 
    }

  private:
    ConnectionState& state;
};

typedef xtl::com_ptr<interchange::IConnection> ConnectionPtr;

/// Внутреннее состояние соединения
struct ConnectionInternalState: public xtl::reference_counter
{
  ServerImpl&     server;               //ссылка на сервер
  ConnectionState state;                //состояние соединения
  ContextImpl     context;              //контекст
  ConnectionPtr   response_connection;  //обратное соединение с клиентом

/// Конструктор
  ConnectionInternalState (ServerImpl& in_server, const char* init_string)
    : server (in_server)
    , state (in_server)
    , context (state)
  {
    common::PropertyMap properties = common::parse_init_string (init_string);

    if (properties.IsPresent ("initiator"))
    {
      stl::string initiator = properties.GetString ("initiator");

      response_connection = ConnectionPtr (interchange::ConnectionManager::CreateConnection (initiator.c_str (), "initiator=render::scene::server::Connection"), false);
   
      context.SetCounterparty (response_connection.get ());
    }
  } 
};

typedef xtl::intrusive_ptr<ConnectionInternalState> StatePtr;

}

/*
    Описание реализации соединения
*/

struct Connection::Impl: public xtl::trackable
{
  StatePtr state; //внутреннее состояние соединения  

/// Конструктор
  Impl (ServerImpl& server, const char* init_string)
    : state (new ConnectionInternalState (server, init_string), false)
  {
    connect_tracker (server.connect_tracker (xtl::bind (&Impl::OnServerShutdown, this)));
  } 

/// Оповещение об удалении сервера
  void OnServerShutdown ()
  {
    state = StatePtr ();
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
    Обработка входного потока данных
*/

void Connection::ProcessCommands (const interchange::CommandBuffer& commands)
{
  try
  {
    if (!impl->state)
      throw xtl::format_operation_exception ("", "Server is lost");

    impl->state->context.ProcessCommands (commands);
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
