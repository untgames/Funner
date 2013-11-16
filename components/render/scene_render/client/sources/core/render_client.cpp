#include "shared.h"

using namespace render::scene::client;

/*
    Константы
*/

const size_t DEFAULT_MAX_DRAW_DEPTH = 3; //разрешенная глубина вложенного рендеринга по умолчанию

/*
    Описание реализации рендера сцены
*/

struct Client::Impl: public xtl::reference_counter
{
  ConnectionPtr connection;     //ссылка на соединение
  size_t        max_draw_depth; //разрешенная глубина вложенного рендеринга

/// Конструктор
  Impl ()
    : max_draw_depth (DEFAULT_MAX_DRAW_DEPTH)
  {
  }
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

    impl->connection->Context ().SetMaxDrawDepth (static_cast<uint32> (impl->max_draw_depth));
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

RenderTarget Client::CreateRenderTarget (const char* target_name, const char* init_string)
{
  try
  {
    RenderTargetPtr render_target (new RenderTargetImpl (impl->connection, target_name, init_string), false);    

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
  try
  {
    if (level == impl->max_draw_depth)
      return;

    impl->max_draw_depth = level;

    impl->connection->Context ().SetMaxDrawDepth (static_cast<uint32> (level));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Client::SetMaxDrawDepth");
    throw;
  }
}

size_t Client::MaxDrawDepth () const
{
  return impl->max_draw_depth;
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
