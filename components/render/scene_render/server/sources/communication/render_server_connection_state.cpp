#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Описание реализации состояния соединения
*/

struct ConnectionState::Impl
{
  ServerImpl& server; //ссылка на сервер

/// Конструктор
  Impl (ServerImpl& in_server) : server (in_server) {}  
};

/*
    Конструктор / деструктор
*/

ConnectionState::ConnectionState (ServerImpl& server)
{
  try
  {
    impl.reset (new Impl (server));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ConnectionState::ConnectionState");
    throw;
  }
}

ConnectionState::~ConnectionState ()
{
}

/*
    Команды клиента
*/

void ConnectionState::LoadResource (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::UnloadResource (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::OnWindowAttached (size_t id, const char* name, const char* init_string, void* handle, size_t width, size_t height, size_t left, size_t top, size_t right, size_t bottom)
{
  try
  {
    impl->server.WindowManager ().AttachWindow (id, name, init_string, handle, width, height, manager::Rect (int (left), int (top), right - left, bottom - top));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowAttached");
    throw;
  }
}

void ConnectionState::OnWindowDetached (size_t id)
{
  try
  {
    impl->server.WindowManager ().DetachWindow (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowDetached");
    throw;
  }
}

void ConnectionState::OnWindowSizeChanged (size_t id, size_t width, size_t height)
{
  try
  {
    impl->server.WindowManager ().SetWindowSize (id, width, height);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowSizeChanged");
    throw;
  }
}

void ConnectionState::OnWindowViewportChanged (size_t id, size_t left, size_t top, size_t right, size_t bottom)
{
  try
  {
    impl->server.WindowManager ().SetWindowViewport (id, manager::Rect (int (left), int (top), right - left, bottom - top));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowViewportChanged");
    throw;
  }
}

void ConnectionState::OnWindowHandleChanged (size_t id, void* handle)
{
  try
  {
    impl->server.WindowManager ().SetWindowHandle (id, handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowHandleChanged");
    throw;
  }
}

void ConnectionState::OnWindowPaint (size_t id)
{
  try
  {
    impl->server.WindowManager ().RepaintWindow (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::ConnectionState::OnWindowPaint");
    throw;
  }
}
