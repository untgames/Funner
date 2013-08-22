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

void ServerLoopbackConnection::OnWindowAttached (size_t id, const char* name, void* handle, size_t width, size_t height, size_t left, size_t top, size_t right, size_t bottom)
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnWindowAttached);
    
    write (stream, uint32 (id));
    write (stream, name);
    write (stream, handle);
    write (stream, uint32 (width));
    write (stream, uint32 (height));
    write (stream, uint32 (left));
    write (stream, uint32 (top));
    write (stream, uint32 (right));
    write (stream, uint32 (bottom));

    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnWindowAttached");
    throw;
  }
}

void ServerLoopbackConnection::OnWindowDetached (size_t id)
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnWindowDetached);
    
    write (stream, uint32 (id));

    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnWindowDetached");
    throw;
  }
}

void ServerLoopbackConnection::OnWindowSizeChanged (size_t id, size_t width, size_t height)
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnWindowSizeChanged);
    
    write (stream, uint32 (id));
    write (stream, uint32 (width));
    write (stream, uint32 (height));

    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnWindowSizeChanged");
    throw;
  }
}

void ServerLoopbackConnection::OnWindowViewportChanged (size_t id, size_t left, size_t top, size_t right, size_t bottom)
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnWindowViewportChanged);
    
    write (stream, uint32 (id));
    write (stream, uint32 (left));
    write (stream, uint32 (top));
    write (stream, uint32 (right));
    write (stream, uint32 (bottom));

    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnWindowViewportChanged");
    throw;
  }
}

void ServerLoopbackConnection::OnWindowHandleChanged (size_t id, void* handle)
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnWindowHandleChanged);
    
    write (stream, uint32 (id));
    write (stream, handle);

    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnWindowHandleChanged");
    throw;
  }
}

void ServerLoopbackConnection::OnWindowPaint (size_t id)
{
  try
  {
    interchange::OutputStream& stream = impl->stream;

    stream.BeginCommand (InternalCommandId_OnWindowPaint);

    write (stream, uint32 (id));

    stream.EndCommand ();

    impl->SendCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ServerLoopbackConnection::OnWindowPaint");
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
      case InternalCommandId_OnWindowAttached:
      {
        size_t      id     = read (stream, xtl::type<uint32> ());
        const char* name   = read (stream, xtl::type<const char*> ());
        void*       handle = read (stream, xtl::type<void*> ());
        size_t      width  = read (stream, xtl::type<uint32> ()), 
                    height = read (stream, xtl::type<uint32> ()),
                    left   = read (stream, xtl::type<uint32> ()),
                    top    = read (stream, xtl::type<uint32> ()),
                    right  = read (stream, xtl::type<uint32> ()),
                    bottom = read (stream, xtl::type<uint32> ());

        state.OnWindowAttached (id, name, handle, width, height, left, top, right, bottom);

        return true;
      }
      case InternalCommandId_OnWindowDetached:
      {
        size_t id = read (stream, xtl::type<uint32> ());

        state.OnWindowDetached (id);

        return true;
      }
      case InternalCommandId_OnWindowSizeChanged:
      {
        size_t id     = read (stream, xtl::type<uint32> ()),
               width  = read (stream, xtl::type<uint32> ()), 
               height = read (stream, xtl::type<uint32> ());

        state.OnWindowSizeChanged (id, width, height);

        return true;
      }
      case InternalCommandId_OnWindowViewportChanged:
      {
        size_t id     = read (stream, xtl::type<uint32> ()),
               left   = read (stream, xtl::type<uint32> ()), 
               top    = read (stream, xtl::type<uint32> ()),
               right  = read (stream, xtl::type<uint32> ()),
               bottom = read (stream, xtl::type<uint32> ());

        state.OnWindowViewportChanged (id, left, top, right, bottom);

        return true;
      }
      case InternalCommandId_OnWindowHandleChanged:
      {
        size_t id     = read (stream, xtl::type<uint32> ());
        void*  handle = read (stream, xtl::type<void*> ()); 

        state.OnWindowHandleChanged (id, handle);

        return true;
      }
      case InternalCommandId_OnWindowPaint:
      {
        size_t id = read (stream, xtl::type<uint32> ());

        state.OnWindowPaint (id);

        return true;
      }
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
