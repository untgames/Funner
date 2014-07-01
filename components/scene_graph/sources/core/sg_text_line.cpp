#include "shared.h"

using namespace scene_graph;
using namespace math;

#ifdef __BIG_ENDIAN__
const common::Encoding UTF32_ENCODING = common::Encoding_UTF32BE;
#else
const common::Encoding UTF32_ENCODING = common::Encoding_UTF32LE;
#endif

namespace
{

float clamp (float x)
{
  if (x < 0.0f) return 0.0f;
  if (x > 1.0f) return 1.0f;

  return x;
}

vec4f clamp (const vec4f& color)
{
  return vec4f (clamp (color.x), clamp (color.y), clamp (color.z), clamp (color.w));
}

//определение корректности кода символа
inline bool is_symbol_valid (size_t symbol_code, size_t first_code, size_t last_code)
{
  return symbol_code >= first_code && symbol_code < last_code;
}

const math::vec4f DEFAULT_CHAR_COLOR_FACTOR = 1.f;
const size_t      DEFAULT_FONT_SIZE         = 14;
const size_t      DEFAULT_FONT_SIZE_EPS     = 2;
const char*       DEFAULT_FONT_CHARSET      = "";

//границы текста
struct TextDimensions
{
  math::vec2f min;
  math::vec2f max;
};

struct FontCreationParamsImpl: public media::FontCreationParams
{
  stl::string charset_name_string;

  FontCreationParamsImpl ()
  {
    memset (static_cast<FontCreationParams*> (this), 0, sizeof (FontCreationParams));

    font_size           = DEFAULT_FONT_SIZE;
    font_size_eps       = DEFAULT_FONT_SIZE_EPS;
    charset_name_string = DEFAULT_FONT_CHARSET;
    charset_name        = charset_name_string.c_str ();
  }

  FontCreationParamsImpl (const media::FontCreationParams& src)
    : FontCreationParams (src)
    , charset_name_string (src.charset_name)
  {
    charset_name = charset_name_string.c_str ();
  }
};

}


/*
    Описание реализации TextLine
*/

typedef xtl::uninitialized_storage<unsigned int> Utf32Buffer;

struct TextLine::Impl: public xtl::instance_counter<TextLine>
{
  typedef stl::vector <math::vec4f> CharsColors;

  stl::string                text_utf8;
  Utf32Buffer                text_utf32;
  size_t                     length;
  size_t                     text_utf8_hash;
  size_t                     text_utf32_hash;
  bool                       text_utf8_need_update;
  bool                       text_utf32_need_update;
  stl::string                font_name;
  FontCreationParamsImpl     font_creation_params;
  stl::auto_ptr<media::Font> font;
  vec4f                      color;
  vec4f                      last_asked_char_color;    //переменная для хранения возвращаемого значения
  TextLineAlignment          horizontal_alignment;
  TextLineAlignment          vertical_alignment;
  CharsColors                chars_colors_factors;     //множители цвета букв
  CharsColors                chars_colors;
  bool                       chars_colors_need_update;
  float                      spacing_multiplier;

  Impl ()
   : length (0),
     text_utf8_hash (common::strhash ("")),
     text_utf32_hash (common::strhash (L"")),     
     text_utf8_need_update (false),
     text_utf32_need_update (false),
     color (1.f, 1.f, 1.f, 1.f), 
     horizontal_alignment (TextLineAlignment_Left),
     vertical_alignment (TextLineAlignment_Top),
     chars_colors_need_update (false),
     spacing_multiplier (1.f)
   {}

  void OnTextChanged ()
  {
    chars_colors_factors.clear ();
    chars_colors.clear ();
  }

  void UpdateCharsColors ()
  {
    size_t chars_count = chars_colors_factors.size ();

    chars_colors.resize (chars_count);

    for (size_t i = 0; i < chars_count; i++)
      chars_colors [i] = clamp (color * chars_colors_factors [i]);

    chars_colors_need_update = false;
  }
};

/*
    Конструктор / деструктор
*/

TextLine::TextLine (const media::FontLibrary& font_library)
  : TextModel (font_library)
  , impl (new Impl)
{
}

TextLine::~TextLine ()
{
}

/*
    Создание линии текста
*/

TextLine::Pointer TextLine::Create (const media::FontLibrary& font_library)
{
  return Pointer (new TextLine (font_library), false);
}

/*
    Цвет текста
*/

void TextLine::SetColor (const vec4f& color)
{
  impl->color = clamp (color);

  UpdateCharsNotify ();
}

void TextLine::SetColor (float red, float green, float blue, float alpha)
{
  SetColor (vec4f (red, green, blue, alpha));
}

void TextLine::SetColor (float red, float green, float blue)
{
  SetColor (vec4f (red, green, blue, impl->color.w));
}

const vec4f& TextLine::Color () const
{
  return impl->color;
}

/*
   Цвет определенных символов текста
*/

void TextLine::SetCharsColorFactors (size_t first, size_t count, const math::vec4f& color)
{
  static const char* METHOD_NAME = "scene_graph::TextLine::SetCharsColor";

  size_t text_length = TextLength (),
         end         = first + count;

  if (end > text_length)
    throw xtl::make_range_exception (METHOD_NAME, "first + count", end, 0u, text_length + 1);

  if (impl->chars_colors_factors.empty ())
    impl->chars_colors_factors.resize (TextLength (), DEFAULT_CHAR_COLOR_FACTOR);

  stl::fill (impl->chars_colors_factors.begin () + first, impl->chars_colors_factors.begin () + end, color);

  impl->chars_colors_need_update = true;

  UpdateCharsNotify ();
}

const math::vec4f& TextLine::CharColorFactor (size_t index) const
{
  static const char* METHOD_NAME = "scene_graph::TextLine::CharColorFactor";

  size_t text_length = TextLength ();

  if (index >= text_length)
    throw xtl::make_range_exception (METHOD_NAME, "index", index, 0u, text_length);

  if (impl->chars_colors_factors.empty ())
    return DEFAULT_CHAR_COLOR_FACTOR;

  return impl->chars_colors_factors [index];
}

void TextLine::CharsColorFactors (size_t first, size_t count, math::vec4f* colors) const
{
  static const char* METHOD_NAME = "scene_graph::TextLine::CharsColorFactors";

  size_t text_length = TextLength (),
         end         = first + count;

  if (end > text_length)
    throw xtl::make_range_exception (METHOD_NAME, "first + count", end, 0u, text_length + 1);

  if (impl->chars_colors_factors.empty ())
  {
    for (size_t i = 0; i < count; i++, colors++)
      *colors = 1.f;
  }
  else
    stl::copy (impl->chars_colors_factors.begin () + first, impl->chars_colors_factors.begin () + end, colors);
}

const math::vec4f& TextLine::CharColor (size_t index) const
{
  if (index >= TextLength ())
    throw xtl::make_range_exception ("scene_graph::TextLine::SetCharsColor", "index", index, 0u, TextLength ());

  if (impl->chars_colors_need_update)
    impl->UpdateCharsColors ();

  if (impl->chars_colors.empty ())
    return Color ();

  return impl->chars_colors [index];
}

void TextLine::CharsColors (size_t first, size_t count, math::vec4f* colors) const
{
  static const char* METHOD_NAME = "scene_graph::TextLine::CharsColor";

  size_t text_length = TextLength (),
         end         = first + count;

  if (end > text_length)
    throw xtl::make_range_exception (METHOD_NAME, "first + count", end, 0u, text_length + 1);

  if (impl->chars_colors_need_update)
    impl->UpdateCharsColors ();

  if (impl->chars_colors.empty ())
  {
    const math::vec4f& color = Color ();

    for (size_t i = 0; i < count; i++, colors++)
      *colors = color;
  }
  else
    stl::copy (impl->chars_colors.begin () + first, impl->chars_colors.begin () + end, colors);
}

/*
   Установка/получение текста
*/

void TextLine::SetTextUtf8 (const char* text)
{
  if (!text)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetTextUtf8", "text");

  impl->text_utf8              = text;
  impl->text_utf8_hash         = common::strhash (impl->text_utf8.c_str ());  
  impl->length                 = impl->text_utf8.size ();
  impl->text_utf8_need_update  = false;
  impl->text_utf32_need_update = true;  

  impl->OnTextChanged ();

  UpdateCharsNotify ();
}

void TextLine::SetTextUtf32 (const unsigned int* text, size_t length)
{  
  if (length && !text)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetText(const unsigned int*,size_t)", "text");

  impl->text_utf32.resize (length + 1, false);

  impl->length = 0;

  if (length)
  {
    size_t size = length * sizeof (unsigned int);

    memcpy (impl->text_utf32.data (), text, size);
    memset (impl->text_utf32.data () + length, 0, sizeof (unsigned int));

    impl->text_utf32_hash = common::crc32 (impl->text_utf32.data (), size);
  }
  else
  {
    memset (impl->text_utf32.data (), 0, sizeof (unsigned int));    

    impl->text_utf32_hash = ~0u;
  }

  impl->text_utf32_need_update = false;  
  impl->text_utf8_need_update  = true;
  impl->length                 = length;

  impl->OnTextChanged ();

  UpdateCharsNotify ();
}

const char* TextLine::TextUtf8 () const
{
  if (impl->text_utf8_need_update)
  {
    try
    {
      if (!impl->text_utf32_need_update)
      {      
        if (impl->text_utf32.size () != 0)
        {
          impl->text_utf8.fast_resize (impl->length * 4);          

          const void* source           = impl->text_utf32.data ();
          size_t      source_size      = (impl->text_utf32.size () - 1) * sizeof (unsigned int);
          void*       destination      = &impl->text_utf8 [0];
          size_t      destination_size = impl->text_utf8.size ();

          convert_encoding (UTF32_ENCODING, source, source_size, common::Encoding_UTF8, destination, destination_size);

          if (source_size)
            throw xtl::format_operation_exception ("", "Internal error: buffer not enough (source_size=%u, destination_size=%u)", source_size, destination_size);

          *(char*)destination = '\0';
        }
        else
        {
          impl->text_utf8.clear ();
        }
      }
      else
      {
        throw xtl::format_operation_exception ("", "Internal error: no base text representation for conversion");
      }

      impl->text_utf8_hash        = common::strhash (impl->text_utf8.c_str ());
      impl->text_utf8_need_update = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("scene_graph::TextLine::Text");
      throw;
    }
  }

  return impl->text_utf8.c_str ();
}

const unsigned int* TextLine::TextUtf32 () const
{
  if (impl->text_utf32_need_update)
  {
    try
    {
      if (!impl->text_utf8_need_update)
      {      
        if (!impl->text_utf8.empty ())
        {
          impl->text_utf32.resize (impl->text_utf8.size () + 1, false);

          const void* source           = impl->text_utf8.c_str ();
          size_t      source_size      = impl->text_utf8.size ();
          void*       destination      = impl->text_utf32.data ();
          size_t      destination_size = impl->text_utf32.size () * sizeof (unsigned int);

          convert_encoding (common::Encoding_UTF8, source, source_size, UTF32_ENCODING, destination, destination_size);

          if (source_size || destination_size < sizeof (unsigned int))
            throw xtl::format_operation_exception ("", "Internal error: buffer not enough (source_size=%u, destination_size=%u)", source_size, destination_size);

          memset (destination, 0, sizeof (unsigned int));

          impl->length = (unsigned int*)destination - impl->text_utf32.data ();                    
        }
        else        
        {
          impl->text_utf32.resize (1, false);

          impl->length = 0;

          memset (impl->text_utf32.data (), 0, sizeof (unsigned int));
        }
      }
      else
      {
        throw xtl::format_operation_exception ("", "Internal error: no base text representation for conversion");
      }

      impl->text_utf32_hash        = impl->text_utf32.size () == 0 ? ~0u : common::crc32 (impl->text_utf32.data (), (impl->text_utf32.size () - 1) * sizeof (unsigned int));

      impl->text_utf32_need_update = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("scene_graph::TextLine::TextUtf32");
      throw;
    }
  }

  return impl->text_utf32.data ();
}

size_t TextLine::TextLength  () const
{
  if (impl->text_utf32_need_update)
    TextUtf32 (); //обновление длины

  return impl->length;
}

/*
    Хэш текста
*/

size_t TextLine::TextUtf8Hash () const
{
  if (impl->text_utf8_need_update)
    TextUtf8 (); //обновление хэша

  return impl->text_utf8_hash;
}

size_t TextLine::TextUtf32Hash () const
{
  if (impl->text_utf32_need_update)
    TextUtf32 (); //обновление хэша

  return impl->text_utf32_hash;
}

/*
   Установка/получение имени шрифта
*/

void TextLine::SetFont (const char* font_name)
{
  try
  {
    if (!font_name)
      throw xtl::make_null_argument_exception ("", "font_name");

    if (impl->font_name == font_name)
      return;

    impl->font_name = font_name;

    impl->font.reset ();

    UpdateFontsNotify ();

    UpdateCharsNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::TextLine::SetFont");
    throw;
  }
}

const char* TextLine::Font () const
{
  return impl->font_name.c_str ();
}

void TextLine::SetFontCreationParams (const media::FontCreationParams& params)
{
  if (!params.charset_name)
    throw xtl::make_null_argument_exception ("scene_graph::TextLine::SetFontCreationParams", "params.charset_name");

  impl->font.reset ();

  static_cast<media::FontCreationParams&> (impl->font_creation_params) = params;

  impl->font_creation_params.charset_name_string = params.charset_name;
  impl->font_creation_params.charset_name        = impl->font_creation_params.charset_name_string.c_str ();

  UpdateCharsNotify ();
}

const media::FontCreationParams& TextLine::FontCreationParams () const
{
  return impl->font_creation_params;
}

/*
   Установка/получение межбуквенного интервала
*/

void TextLine::SetSpacingMultiplier (float spacing_multiplier)
{
  impl->spacing_multiplier = spacing_multiplier;

  UpdateCharsNotify ();
}

float TextLine::SpacingMultiplier () const
{
  return impl->spacing_multiplier;
}

/*
   Выравнивание
*/

void TextLine::SetAlignment (TextLineAlignment horizontal, TextLineAlignment vertical)
{
  static const char* METHOD_NAME = "scene_graph::TextLine::SetAlignment";

  if (horizontal >= TextLineAlignment_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "horizontal", horizontal, "Unknown TextLineAlignment");

  if (vertical >= TextLineAlignment_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "vertical", vertical, "Unknown TextLineAlignment");

  impl->horizontal_alignment = horizontal;
  impl->vertical_alignment = vertical;

  UpdateCharsNotify ();
}

void TextLine::SetHorizontalAlignment (TextLineAlignment alignment)
{
  if (alignment >= TextLineAlignment_Num)
    throw xtl::make_argument_exception ("scene_graph::TextLine::SetHorizontalAlignment", "alignment", alignment, "Unknown TextLineAlignment");

  impl->horizontal_alignment = alignment;

  UpdateCharsNotify ();
}

void TextLine::SetVerticalAlignment (TextLineAlignment alignment)
{
  if (alignment >= TextLineAlignment_Num)
    throw xtl::make_argument_exception ("scene_graph::TextLine::SetVerticalAlignment", "alignment", alignment, "Unknown TextLineAlignment");

  impl->vertical_alignment = alignment;

  UpdateCharsNotify ();
}

TextLineAlignment TextLine::VerticalAlignment () const
{
  return impl->vertical_alignment;
}

TextLineAlignment TextLine::HorizontalAlignment () const
{
  return impl->horizontal_alignment;
}

/*
    Перестроение таблицы символов
*/

void TextLine::RebuildCharsCore ()
{
  try
  {
    if (!impl->font)
    {
        //создание шрифта

      impl->font.reset (new media::Font (FontLibrary ().CreateFont (impl->font_name.c_str (), impl->font_creation_params)));
    }

      //обновление цветов

    if (impl->chars_colors_need_update)
      impl->UpdateCharsColors ();

      //обновление символов

    size_t chars_count = TextLength ();

    ResizeChars (0);
    ResizeChars (chars_count);

    media::Font&            font               = *impl->font;
    const media::GlyphInfo* glyphs             = font.Glyphs ();
    size_t                  glyphs_count       = font.GlyphsCount (),
                            first_glyph_code   = font.FirstGlyphCode (),
                            last_glyph_code    = first_glyph_code + glyphs_count;
    float                   current_pen_x      = 0.0f,
                            current_pen_y      = 0.0f,
                            font_size          = (float)font.FontSize (),
                            spacing_multiplier = SpacingMultiplier (),
                            advance_multiplier = spacing_multiplier / font_size;
    TextDimensions          text_dimensions;
    const unsigned int*     src_char           = TextUtf32 ();
    const math::vec4f*      colors             = impl->chars_colors.empty () ? (const math::vec4f*)0 : &impl->chars_colors [0],
                            &default_color     = impl->color;
    CharDesc                *dst_first_char    = CharsForUpdate (),
                            *dst_char          = dst_first_char;
    size_t                  prev_glyph_index   = 0;

    for (size_t i=0; i<chars_count; i++, src_char++)
    {
        //проверка наличия кода символа в шрифте

      size_t current_symbol_code = *src_char;

      if (!is_symbol_valid (current_symbol_code, first_glyph_code, last_glyph_code))
      {
        if (!is_symbol_valid ('?', first_glyph_code, last_glyph_code))
          continue;

        current_symbol_code = '?';
      }

        //получение глифа

      size_t                  glyph_index = current_symbol_code - first_glyph_code;
      const media::GlyphInfo& glyph       = glyphs [glyph_index];

        //перенос пера

      if (dst_char != dst_first_char) //производится только если есть предыдущий символ
      {
        if (font.HasKerning (prev_glyph_index, glyph_index))
        {
          media::KerningInfo kerning_info = font.Kerning (prev_glyph_index, glyph_index);

          current_pen_x += kerning_info.x_kerning / font_size;
          current_pen_y += kerning_info.y_kerning / font_size;
        }
      }

        //инициализация символа

      float bearing_x = glyph.bearing_x / font_size,
            bearing_y = glyph.bearing_y / font_size,
            size_x    = glyph.width / font_size,
            size_y    = glyph.height / font_size;

      dst_char->code     = *src_char;
      dst_char->font     = &font;
      dst_char->position = math::vec3f (current_pen_x + bearing_x + size_x / 2.f, current_pen_y + bearing_y - size_y / 2.f, 0.f);
      dst_char->size     = math::vec2f (size_x, size_y);
      dst_char->color    = colors ? colors [i] : default_color;

        //перенос пера

      current_pen_x += glyph.advance_x * advance_multiplier;
      current_pen_y += glyph.advance_y * advance_multiplier;

        //корректировка границ текста

      TextDimensions glyph_dimensions;

      glyph_dimensions.min.x = dst_char->position.x - dst_char->size.x * 0.5f;
      glyph_dimensions.min.y = dst_char->position.y - dst_char->size.y * 0.5f;
      glyph_dimensions.max.x = glyph_dimensions.min.x + dst_char->size.x;
      glyph_dimensions.max.y = glyph_dimensions.min.y + dst_char->size.y;

      if (glyph_dimensions.min.x < text_dimensions.min.x) text_dimensions.min.x = glyph_dimensions.min.x;
      if (glyph_dimensions.min.y < text_dimensions.min.y) text_dimensions.min.y = glyph_dimensions.min.y;
      if (glyph_dimensions.max.x > text_dimensions.max.x) text_dimensions.max.x = glyph_dimensions.max.x;
      if (glyph_dimensions.max.y > text_dimensions.max.y) text_dimensions.max.y = glyph_dimensions.max.y;

        //переход к следующему символу

      dst_char++;

      prev_glyph_index = glyph_index;
    }    

      //корректировка количества символов

    chars_count = dst_char - dst_first_char;

    ResizeChars (chars_count);

      //смещение в зависимости от выравнивания

    if (impl->horizontal_alignment != TextLineAlignment_Left || impl->vertical_alignment != TextLineAlignment_Left)
    {
        //расчёт вектора смещения надписи

      math::vec2f size = text_dimensions.max - text_dimensions.min;
      math::vec3f offset (-text_dimensions.min.x, -text_dimensions.min.y, 0);

      switch (impl->horizontal_alignment)
      {
        default:
        case TextLineAlignment_Center:
          offset.x -= 0.5f * size.x;
          break;
        case TextLineAlignment_Right:
          offset.x -= size.x;
          break;
        case TextLineAlignment_BaseLine:
          offset.x = 0.f;
          break;
        case TextLineAlignment_Left:
          break;
      }

      switch (impl->vertical_alignment)
      {
        default:
        case TextLineAlignment_Center:
          offset.y -= 0.5f * size.y;
          break;
        case TextLineAlignment_Top:
          offset.y -= size.y;
          break;
        case TextLineAlignment_BaseLine:
          offset.y = 0.f;
          break;
        case TextLineAlignment_Bottom:
          break;
      }

      dst_char = dst_first_char;

      for (size_t i=0; i<chars_count; i++, dst_char++)
        dst_char->position += offset; 
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::TextLine::RebuildCharsCore");
    throw;
  }
}

/*
    Метод, вызываемый при посещении объекта
*/

void TextLine::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    TextModel::AcceptCore (visitor);
}

/*
    Связывание свойств
*/

void TextLine::BindProperties (common::PropertyBindingMap& bindings)
{
  TextModel::BindProperties (bindings);

  bindings.AddProperty ("Font", xtl::bind (&TextLine::Font, this), xtl::bind (&TextLine::SetFont, this, _1));
  bindings.AddProperty ("Color", xtl::bind (&TextLine::Color, this), xtl::bind (xtl::implicit_cast<void (TextLine::*)(const math::vec4f&)> (&TextLine::SetColor), this, _1));
  bindings.AddProperty ("TextUtf8", xtl::bind (&TextLine::TextUtf8, this), xtl::bind (&TextLine::SetTextUtf8, this, _1));
}
