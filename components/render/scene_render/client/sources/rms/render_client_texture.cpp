#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации текстуры
*/

struct Texture::Impl
{
  Context&    context;           //контекст
  stl::string name;              //имя текстуры
  bool        remove_on_destroy; //удалять ли в деструкторе

  Impl (Context& in_context, const char* in_name, bool in_remove_on_destroy)
    : context (in_context)
    , name (in_name)
    , remove_on_destroy (in_remove_on_destroy)
  {
  }
};

/*
    Конструктор / деструктор
*/

Texture::Texture (Context& context, const char* name, bool remove_on_destroy)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl.reset (new Impl (context, name, remove_on_destroy));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Texture::Texture");
    throw;
  }
}

Texture::Texture (Context& context, const char* name, const media::Image& image, render::scene::interchange::TextureDimension dimension, bool create_mipmaps, bool remove_on_destroy)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl.reset (new Impl (context, name, remove_on_destroy));

    context.CreateTexture (name, image, dimension, create_mipmaps);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Texture::Texture");
    throw;
  }
}

Texture::~Texture ()
{
  try
  {
    if (impl->remove_on_destroy)
      impl->context.RemoveTexture (impl->name.c_str ());
  }
  catch (...)
  {
  }
}

/*
    Имя
*/

const char* Texture::Name ()
{
  return impl->name.c_str ();
}

/*
    Обновление текстуры
*/

void Texture::Update (const media::Image& image)
{
  try
  {
    impl->context.UpdateTexture (impl->name.c_str (), image);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Texture::Update");
    throw;
  }
}
