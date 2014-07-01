#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации шрифта
*/

typedef stl::vector<TexturePtr>          TextureArray;
typedef stl::vector<RasterizedGlyphInfo> GlyphArray;

struct Font::Impl
{
  TextureArray textures;         //текстуры
  GlyphArray   glyphs;           //глифы
  size_t       first_glyph_code; //код первого глифа
  stl::string  name;             //имя шрифта
    
  Impl () : first_glyph_code () {}
};

/*
    Конструктор / деструктор
*/

Font::Font (MaterialManager& material_manager, const media::Font& font, const media::RasterizedFontCreationParams& creation_params)
  : impl (new Impl)
{
  try
  {
      //сохранение имени

    impl->name = font.Name ();

      //растеризация

    media::RasterizedFont rasterized_font = font.CreateRasterizedFont (creation_params);

      //создание текстур

    typedef stl::vector<math::vec2f> SizeArray;

    SizeArray sizes;

    size_t images_count = rasterized_font.ImagesCount ();

    impl->textures.reserve (images_count);
    sizes.reserve (images_count);

    for (size_t i=0; i<images_count; i++)
    {
      media::Image image;

      rasterized_font.BuildImage (i, image);

      TexturePtr texture = material_manager.CreateTexture (common::format ("font_textures.%s.font%08u.image%02u", font.Name (), font.Id (), i).c_str (),
        image, interchange::TextureDimension_2D, true);

      impl->textures.push_back (texture);

      sizes.push_back (math::vec2f ((float)image.Width (), (float)image.Height ()));
    }

      //заполнение таблицы глифов

    size_t glyphs_count = font.GlyphsCount ();

    impl->first_glyph_code = font.FirstGlyphCode ();

    if (glyphs_count)
    {
      impl->glyphs.resize (glyphs_count);

      const media::RasterizedGlyphInfo* src_glyph = rasterized_font.RasterizedGlyphs ();
      RasterizedGlyphInfo*              dst_glyph = &impl->glyphs [0];

      for (size_t i=0; i<glyphs_count; i++, src_glyph++, dst_glyph++)
      {
        const math::vec2f& size = sizes [src_glyph->image_index];

        dst_glyph->image_index = src_glyph->image_index;
        dst_glyph->tex_offset  = math::vec2f (src_glyph->x_pos / size.x, src_glyph->y_pos / size.y);
        dst_glyph->tex_size    = math::vec2f (src_glyph->width / size.x, src_glyph->height / size.y);
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Font::Font");
    throw;
  }
}

Font::~Font ()
{
}

/*
    Имя шрифта
*/

const char* Font::Name ()
{
  return impl->name.c_str ();
}

/*
    Размер таблицы глифов
*/

size_t Font::GlyphsCount ()
{
  return impl->glyphs.size ();
}

/*
    Код первого глифа
*/

size_t Font::FirstGlyphCode ()
{
  return impl->first_glyph_code;
}

/*
    Растеризованные глифы
*/

const RasterizedGlyphInfo* Font::RasterizedGlyphs ()
{
  if (impl->glyphs.empty ())
    return 0;

  return &impl->glyphs [0];
}

/*
    Количество текстур
*/

size_t Font::TexturesCount ()
{
  return impl->textures.size ();
}

/*
    Получение текстуры
*/

const char* Font::TextureName (size_t texture_index)
{
  if (texture_index >= impl->textures.size ())
    throw xtl::make_range_exception ("render::scene::client::Font::TextureName", "texture_index", texture_index, impl->textures.size ());

  return impl->textures [texture_index]->Name ();
}
