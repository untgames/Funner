#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

using interchange::uint32;

/*
    Константы
*/

const size_t DEFAULT_BUFFER_SIZE = 128;

namespace
{

inline void write (interchange::OutputStream& s, void* value)
{
  s.Write (value);
}

inline void* read (interchange::InputStream& s, xtl::type<void*>)
{
  return s.Read<void*> ();
}

}

/*
    Описание реализации соединения
*/


typedef xtl::com_ptr<interchange::IConnection> ConnectionPtr;

struct ServerLoopbackConnection::Impl
{
  ConnectionPtr                  connection; //внутреннее соединение от сервера к серверу
  interchange::CommandBufferPool pool;       //пул буферов
  interchange::OutputStream      stream;     //выходной поток

/// Конструктор
  Impl ()
  {
    pool.SetDefaultBufferSize (DEFAULT_BUFFER_SIZE);

    SendCommands ();
  }

/// Отсылка команд
  void SendCommands ()
  {
    interchange::CommandBuffer buffer = pool.CreateBuffer ();

    stream.Swap (buffer);

    if (buffer.Size ())
      connection->ProcessCommands (buffer);
  }
};

/*
    Конструктор / деструктор
*/

ServerLoopbackConnection::ServerLoopbackConnection (const char* name)
  : impl (new Impl)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl->connection = ConnectionPtr (interchange::ConnectionManager::CreateConnection (name, ""), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::ServerLoopbackConnection");
    throw;
  }
}

ServerLoopbackConnection::~ServerLoopbackConnection ()
{
}

/*
    Сообщения серверу
*/

void ServerLoopbackConnection::OnSizeChanged (size_t width, size_t height)
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnSizeChanged);
    
    write (stream, uint32 (width));
    write (stream, uint32 (height));

    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnSizeChanged");
    throw;
  }
}

void ServerLoopbackConnection::OnViewportChanged (size_t left, size_t top, size_t right, size_t bottom)
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnViewportChanged);
    
    write (stream, uint32 (left));
    write (stream, uint32 (top));
    write (stream, uint32 (right));
    write (stream, uint32 (bottom));

    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnViewportChanged");
    throw;
  }
}

void ServerLoopbackConnection::OnHandleChanged (void* handle)
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnHandleChanged);
    
    write (stream, handle);

    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnHandleChanged");
    throw;
  }
}

void ServerLoopbackConnection::OnPaint ()
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnPaint);
    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnPaint");
    throw;
  }
}

/*
    Обработка входящей команды
*/

bool ServerLoopbackConnection::ProcessIncomingCommand (InternalCommandId id, interchange::InputStream& stream, ConnectionState& state)
{
  try
  {
    switch (id)
    {
      case InternalCommandId_OnSizeChanged:
      {
        size_t width  = read (stream, xtl::type<uint32> ()), 
               height = read (stream, xtl::type<uint32> ());

        state.OnSizeChanged (width, height);

        return true;
      }
      case InternalCommandId_OnViewportChanged:
      {
        size_t left   = read (stream, xtl::type<uint32> ()), 
               top    = read (stream, xtl::type<uint32> ()),
               right  = read (stream, xtl::type<uint32> ()),
               bottom = read (stream, xtl::type<uint32> ());

        state.OnViewportChanged (left, top, right, bottom);

        return true;
      }
      case InternalCommandId_OnHandleChanged:
      {
        void* handle = read (stream, xtl::type<void*> ()); 

        state.OnHandleChanged (handle);

        return true;
      }
      case InternalCommandId_OnPaint:
        state.OnPaint ();
        return true;
      default:
        return false;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::ProcessIncomingCommand");
    throw;
  }
}
