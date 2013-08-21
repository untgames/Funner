#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации рендера сцены
*/

struct Render::Impl: public xtl::reference_counter
{
  stl::auto_ptr<Connection> connection; //ссылка на соединение
};

/*
    Конструктор / деструктор
*/

Render::Render (const char* connection_name, const char* init_string, size_t logon_timeout_ms)
{
  try
  {
    if (!connection_name)
      throw xtl::make_null_argument_exception ("", "connection_name");

    if (!init_string)
      init_string = "";

    impl = new Impl;

    impl->connection.reset (new Connection (connection_name, init_string, logon_timeout_ms));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Render::Render");
    throw;
  }
}

Render::Render (const Render& render)
  : impl (render.impl)
{
  addref (impl);
}

Render::~Render ()
{
  release (impl);
}

Render& Render::operator = (const Render& render)
{
  Render (render).Swap (*this);

  return *this;
}

/*
    Описание системы рендеринга
*/

const char* Render::Description () const
{
  return impl->connection->Description ();
}

/*
    Создание цели рендеринга
*/

RenderTarget Render::CreateRenderTarget (const char* target_name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Максимальный уровень вложенности рендеринга
*/

void Render::SetMaxDrawDepth (size_t level)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t Render::MaxDrawDepth () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Работа с ресурсами
*/

void Render::LoadResource (const char* file_name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Render::UnloadResource (const char* file_name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Обмен
*/

void Render::Swap (Render& render)
{
  stl::swap (impl, render.impl);
}

namespace render
{

namespace scene
{

void swap (Render& render1, Render& render2)
{
  render1.Swap (render2);
}

}

}
