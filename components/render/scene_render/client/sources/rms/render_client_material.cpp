#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации материала
*/

struct Material::Impl
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

Material::Material (Context& context, const char* name, const char* prototype, bool remove_on_destroy)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl.reset (new Impl (context, name, remove_on_destroy));

    if (prototype)
      context.CloneMaterial (name, prototype);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Material::Material");
    throw;
  }
}

Material::~Material ()
{
  try
  {
    if (impl->remove_on_destroy)
      impl->context.RemoveMaterial (impl->name.c_str ());
  }
  catch (...)
  {
  }
}

/*
    Имя
*/

const char* Material::Name ()
{
  return impl->name.c_str ();
}

/*
    Обновление текстурной карты на стороне сервера (копии на стороне клиента не сохраняются)
*/

void Material::SetTexmapImage (const char* semantic, const char* image_name)
{
  try
  {
    if (!semantic)
      throw xtl::make_null_argument_exception ("", "semantic");

    if (!image_name)
      throw xtl::make_null_argument_exception ("", "image_name");
printf ("%s(%u)\n", __FUNCTION__, __LINE__); fflush (stdout);
    impl->context.SetMaterialTexmapImage (impl->name.c_str (), semantic, image_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Material::SetTexmapImage");
    throw;
  }
}

void Material::SetTexmapSampler (const char* semantic, const char* sampler)
{
  try
  {
    if (!semantic)
      throw xtl::make_null_argument_exception ("", "semantic");

    if (!sampler)
      throw xtl::make_null_argument_exception ("", "sampler");

    impl->context.SetMaterialTexmapSampler (impl->name.c_str (), semantic, sampler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Material::SetTexmapSampler");
    throw;
  }
}
