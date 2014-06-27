#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации материала шрифта
*/

typedef stl::vector<MaterialPtr> MaterialArray;

struct FontMaterial::Impl
{
  FontPtr       font;      //ссылка на шрифт
  MaterialArray materials; //материалы
};

/*
    Конструктор / деструктор
*/

FontMaterial::FontMaterial (MaterialManager& material_manager, const FontPtr& font, const char* material_name, const char* semantic)
  : impl (new Impl)
{
  try
  {
    if (!material_name)
      throw xtl::make_null_argument_exception ("", "material_name");

    if (!semantic)
      throw xtl::make_null_argument_exception ("", "semantic");

    if (!font)
      throw xtl::make_null_argument_exception ("", "font");

    impl->font = font;

    size_t materials_count = font->TexturesCount ();

    impl->materials.reserve (materials_count);

    for (size_t i=0; i<materials_count; i++)
    {
      MaterialPtr material = material_manager.CreateMaterial (common::format ("font_material.%s(%s=%s.font%08u.image%02u)", material_name, semantic, font->Name (), &*font, i).c_str (),
        material_name);

      impl->materials.push_back (material);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::FontMaterial::FontMaterial");
    throw;
  }
}

FontMaterial::~FontMaterial ()
{
}

/*
    Шрифт
*/

Font& FontMaterial::Font ()
{
  return *impl->font;
}

/*
    Количество материалов
*/

size_t FontMaterial::MaterialsCount ()
{
  return impl->materials.size ();
}

/*
    Получение имени материала
*/

const char* FontMaterial::MaterialName (size_t material_index)
{
  if (material_index >= impl->materials.size ())
    throw xtl::make_range_exception ("render::scene::client::Font::MaterialName", "material_index", material_index, impl->materials.size ());

  return impl->materials [material_index]->Name ();

}
