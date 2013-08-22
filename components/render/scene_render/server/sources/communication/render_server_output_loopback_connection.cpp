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

}

/*
    Описание реализации соединения
*/


typedef xtl::com_ptr<interchange::IConnection> ConnectionPtr;

struct OutputServerLoopbackConnection::Impl
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

OutputServerLoopbackConnection::OutputServerLoopbackConnection (const char* name)
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
    e.touch ("render::scene::OutputServerLoopbackConnection::OutputServerLoopbackConnection");
    throw;
  }
}

OutputServerLoopbackConnection::~OutputServerLoopbackConnection ()
{
}

/*
    Сообщения серверу
*/

void OutputServerLoopbackConnection::OnSizeChanged (size_t width, size_t height)
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
    e.touch ("render::scene::OutputServerLoopbackConnection::OnSizeChanged");
    throw;
  }
}

void OutputServerLoopbackConnection::OnViewportChanged (size_t left, size_t top, size_t right, size_t bottom)
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
    e.touch ("render::scene::OutputServerLoopbackConnection::OnViewportChanged");
    throw;
  }
}

void OutputServerLoopbackConnection::OnHandleChanged (void* handle)
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
    e.touch ("render::scene::OutputServerLoopbackConnection::OnHandleChanged");
    throw;
  }
}

void OutputServerLoopbackConnection::OnPaint ()
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
    e.touch ("render::scene::OutputServerLoopbackConnection::OnPaint");
    throw;
  }
}
