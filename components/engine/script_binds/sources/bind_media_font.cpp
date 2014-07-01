#include "shared.h"

using namespace media;
using namespace script;
using namespace xtl;

namespace
{

const size_t DEFAULT_FONT_SIZE     = 14;
const size_t DEFAULT_FONT_SIZE_EPS = 2;
const char*  DEFAULT_FONT_CHARSET  = "";

}

namespace components
{

namespace media_font_script_bind
{

/*
    Константы (имена библиотек)
*/

const char* FONT_LIBRARY_LIBRARY                = "Media.FontLibrary";
const char* FONT_CREATION_PARAMS_LIBRARY        = "Media.FontCreationParams";
const char* FONT_CREATION_PARAMS_NATIVE_LIBRARY = "Media.FontCreationParamsNative";
const char* BINDER_NAME                         = "MediaFont";
const char* COMPONENT_NAME                      = "script.binds.MediaFont";

struct FontCreationParams: public media::FontCreationParams
{
  stl::string charset_name_string;

  FontCreationParams ()
  {
    memset (static_cast<media::FontCreationParams*> (this), 0, sizeof (media::FontCreationParams));

    font_size           = DEFAULT_FONT_SIZE;
    font_size_eps       = DEFAULT_FONT_SIZE_EPS;
    charset_name_string = DEFAULT_FONT_CHARSET;
    charset_name        = charset_name_string.c_str ();
  }

  FontCreationParams (const FontCreationParams& p)
    : media::FontCreationParams (p)
    , charset_name_string (p.charset_name_string)
  {
    charset_name = charset_name_string.c_str ();
  }

  xtl::shared_ptr<FontCreationParams> Clone ()
  {
    return xtl::shared_ptr<FontCreationParams> (new FontCreationParams (*this));
  }

  static xtl::shared_ptr<FontCreationParams> Create ()
  {
    return xtl::shared_ptr<FontCreationParams> (new FontCreationParams);
  }

  size_t      FontSize         () const            { return font_size; }
  void        SetFontSize      (size_t value)      { font_size = value; }
  size_t      FontSizeEps      () const            { return font_size_eps; }
  void        SetFontSizeEps   (size_t value)      { font_size_eps = value; }
  size_t      Weight           () const            { return weight; }
  void        SetWeight        (size_t value)      { weight = value; }
  int         Escapement       () const            { return escapement; }
  void        SetEscapement    (int value)         { escapement = value; }
  bool        Bold             () const            { return bold; }
  void        SetBold          (bool value)        { bold = value; }
  bool        Italic           () const            { return italic; }
  void        SetItalic        (bool value)        { italic = value; }
  bool        Underlined       () const            { return underlined; }
  void        SetUnderlined    (bool value)        { underlined = value; }
  bool        Striked          () const            { return striked; }
  void        SetStriked       (bool value)        { striked = value; }
  size_t      StrokeSize       () const            { return stroke_size; }
  void        SetStrokeSize    (size_t value)      { stroke_size = value; }
  size_t      HorizontalDpi    () const            { return horizontal_dpi; }
  void        SetHorizontalDpi (size_t value)      { horizontal_dpi = value; }
  size_t      VerticalDpi      () const            { return vertical_dpi; }
  void        SetVerticalDpi   (size_t value)      { vertical_dpi = value; }
  const char* CharsetName      () const            { return charset_name; }
  void        SetCharsetName   (const char* value) { charset_name_string = value ? value : ""; charset_name = charset_name_string.c_str (); }
};

void bind_font_creation_params_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (FONT_CREATION_PARAMS_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&FontCreationParams::Create));

    //регистрация операций

  lib.Register ("Clone",   make_invoker (&FontCreationParams::Clone));

    //регистрация свойств

  lib.Register ("get_FontSize",      make_invoker (&FontCreationParams::FontSize));
  lib.Register ("set_FontSize",      make_invoker (&FontCreationParams::SetFontSize));
  lib.Register ("get_FontSizeEps",   make_invoker (&FontCreationParams::FontSizeEps));
  lib.Register ("set_FontSizeEps",   make_invoker (&FontCreationParams::SetFontSizeEps));
  lib.Register ("get_Weight",        make_invoker (&FontCreationParams::Weight));
  lib.Register ("set_Weight",        make_invoker (&FontCreationParams::SetWeight));
  lib.Register ("get_Escapement",    make_invoker (&FontCreationParams::Escapement));
  lib.Register ("set_Escapement",    make_invoker (&FontCreationParams::SetEscapement));
  lib.Register ("get_Bold",          make_invoker (&FontCreationParams::Bold));
  lib.Register ("set_Bold",          make_invoker (&FontCreationParams::SetBold));
  lib.Register ("get_Italic",        make_invoker (&FontCreationParams::Italic));
  lib.Register ("set_Italic",        make_invoker (&FontCreationParams::SetItalic));
  lib.Register ("get_Underlined",    make_invoker (&FontCreationParams::Underlined));
  lib.Register ("set_Underlined",    make_invoker (&FontCreationParams::SetUnderlined));
  lib.Register ("get_Striked",       make_invoker (&FontCreationParams::Striked));
  lib.Register ("set_Striked",       make_invoker (&FontCreationParams::SetStriked));
  lib.Register ("get_StrokeSize",    make_invoker (&FontCreationParams::StrokeSize));
  lib.Register ("set_StrokeSize",    make_invoker (&FontCreationParams::SetStrokeSize));
  lib.Register ("get_HorizontalDpi", make_invoker (&FontCreationParams::HorizontalDpi));
  lib.Register ("set_HorizontalDpi", make_invoker (&FontCreationParams::SetHorizontalDpi));
  lib.Register ("get_VerticalDpi",   make_invoker (&FontCreationParams::VerticalDpi));
  lib.Register ("set_VerticalDpi",   make_invoker (&FontCreationParams::SetVerticalDpi));
  lib.Register ("get_CharsetName",   make_invoker (&FontCreationParams::CharsetName));
  lib.Register ("set_CharsetName",   make_invoker (&FontCreationParams::SetCharsetName));

  environment.RegisterType<FontCreationParams> (FONT_CREATION_PARAMS_LIBRARY);

  InvokerRegistry native_lib = environment.CreateLibrary (FONT_CREATION_PARAMS_NATIVE_LIBRARY);

  environment.RegisterType<media::FontCreationParams> (FONT_CREATION_PARAMS_NATIVE_LIBRARY);
}

///Функции
FontLibrary create_font_library ()
{
  return FontLibrary ();
}

///Регистрация библиотеки работы с библиотекой шрифтов
void bind_font_library_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (FONT_LIBRARY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_font_library));

    //регистрация операций

  lib.Register ("LoadFont",    make_invoker (&FontLibrary::LoadFont));
  lib.Register ("UnloadFont",  make_invoker (&FontLibrary::UnloadFont));
  lib.Register ("LoadFonts",   make_invoker (&FontLibrary::LoadFont));
  lib.Register ("UnloadFonts", make_invoker (&FontLibrary::UnloadFont));
  lib.Register ("Clear",       make_invoker (&FontLibrary::Clear));

  lib.Register ("get_Size",            make_invoker (&FontLibrary::Size));
  lib.Register ("get_Name",            make_invoker (&FontLibrary::Name));
  lib.Register ("set_Name",            make_invoker (&FontLibrary::SetName));
  lib.Register ("get_CacheDir",        make_invoker (&FontLibrary::CacheDir));
  lib.Register ("set_CacheDir",        make_invoker (&FontLibrary::SetCacheDir));
  lib.Register ("get_IsCacheEnabled",  make_invoker (&FontLibrary::CacheState));
  lib.Register ("set_IsCacheEnabled",  make_invoker (&FontLibrary::SetCacheState));

  environment.RegisterType<FontLibrary> (FONT_LIBRARY_LIBRARY);
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_font_creation_params_library (environment);
      bind_font_library_library (environment);
    }
};

template class engine::decl_sg_cast<components::media_font_script_bind::FontCreationParams, media::FontCreationParams>;

extern "C"
{

common::ComponentRegistrator<Component> MediaFontScriptBind (COMPONENT_NAME);

}

}

}
