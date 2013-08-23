#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации рендера сцены
*/

struct Client::Impl: public xtl::reference_counter
{
  ConnectionPtr connection; //ссылка на соединение
};

/*
    Конструктор / деструктор
*/

Client::Client (const char* connection_name, const char* init_string, size_t logon_timeout_ms)
{
  try
  {
    if (!connection_name)
      throw xtl::make_null_argument_exception ("", "connection_name");

    if (!init_string)
      init_string = "";

    impl = new Impl;

    impl->connection = ConnectionPtr (new Connection (connection_name, init_string, logon_timeout_ms), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Client::Client");
    throw;
  }
}

Client::Client (const Client& render)
  : impl (render.impl)
{
  addref (impl);
}

Client::~Client ()
{
  release (impl);
}

Client& Client::operator = (const Client& render)
{
  Client (render).Swap (*this);

  return *this;
}

/*
    Описание системы рендеринга
*/

const char* Client::Description () const
{
  return impl->connection->Description ();
}

/*
    Создание цели рендеринга
*/

namespace
{

struct RenderTargetWrapper: public RenderTarget
{
  RenderTargetWrapper (RenderTargetImpl* impl) : RenderTarget (impl) {}
};

}

RenderTarget Client::CreateRenderTarget (const char* target_name)
{
  try
  {
    RenderTargetPtr render_target (new RenderTargetImpl (impl->connection, target_name), false);    

    return RenderTargetWrapper (render_target.get ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Client::CreateRenderTarget");
    throw;
  }
}

/*
    Максимальный уровень вложенности рендеринга
*/

void Client::SetMaxDrawDepth (size_t level)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t Client::MaxDrawDepth () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Работа с ресурсами
*/

void Client::LoadResource (const char* file_name)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    impl->connection->Context ().LoadResource (file_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Client::LoadResource");
    throw;
  }
}

void Client::UnloadResource (const char* file_name)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    impl->connection->Context ().UnloadResource (file_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Client::UnloadResource");
    throw;
  }
}

/*
    Обмен
*/

void Client::Swap (Client& render)
{
  stl::swap (impl, render.impl);
}

namespace render
{

namespace scene
{

void swap (Client& render1, Client& render2)
{
  render1.Swap (render2);
}

}

}
