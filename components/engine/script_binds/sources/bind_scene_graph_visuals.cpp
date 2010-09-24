#include "scene_graph.h"

namespace engine
{

namespace scene_graph_script_binds
{

/*
    Создание спрайта
*/

Sprite::Pointer create_sprite ()
{
  return Sprite::Create ();
}

SpriteList::Pointer create_sprite_list ()
{
  return SpriteList::Create ();
}

/*
     Регистрация библиотеки работы с спрайтовыми моделями
*/

void bind_sprite_model_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SPRITE_MODEL_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация операций

  lib.Register ("set_Material",       make_invoker (&SpriteModel::SetMaterial));
  lib.Register ("get_Material",       make_invoker (&SpriteModel::Material));
  lib.Register ("set_AlphaReference", make_invoker (&SpriteModel::SetAlphaReference));
  lib.Register ("get_AlphaReference", make_invoker (&SpriteModel::AlphaReference));

    //регистрация типов данных

  environment.RegisterType<SpriteModel> (SCENE_SPRITE_MODEL_LIBRARY);
}

/*
     Регистрация библиотеки работы с спрайтами
*/

void bind_sprite_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SPRITE_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_SPRITE_MODEL_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_sprite));

    //регистрация операций

  lib.Register ("set_Color",    make_invoker (implicit_cast<void (Sprite::*) (const vec4f&)> (&Sprite::SetColor)));
  lib.Register ("set_Alpha",    make_invoker (&Sprite::SetAlpha));
  lib.Register ("set_Frame",    make_invoker (&Sprite::SetFrame));
  lib.Register ("get_Color",    make_invoker (&Sprite::Color));
  lib.Register ("get_Alpha",    make_invoker (&Sprite::Alpha));
  lib.Register ("get_Frame",    make_invoker <int (Sprite&)> (&Sprite::Frame));

  lib.Register ("SetColor", make_invoker (make_invoker (implicit_cast<void (Sprite::*) (float, float, float, float)> (&Sprite::SetColor)),
                                          make_invoker (implicit_cast<void (Sprite::*) (float, float, float)>        (&Sprite::SetColor))));

    //регистрация типов данных

  environment.RegisterType<Sprite> (SCENE_SPRITE_LIBRARY);
}

/*
    Создание линии текста
*/

TextLine::Pointer create_text_line ()
{
  return TextLine::Create ();
}

/*
   Регистрация библиотеки работы с текстом
*/

void bind_static_text_line_library (Environment& environment)
{
  InvokerRegistry& text_line_alignment_lib = environment.CreateLibrary (SCENE_STATIC_TEXT_LINE_ALIGNMENT_LIBRARY);

  text_line_alignment_lib.Register ("get_Center",   make_const (TextLineAlignment_Center));
  text_line_alignment_lib.Register ("get_Left",     make_const (TextLineAlignment_Left));
  text_line_alignment_lib.Register ("get_Right",    make_const (TextLineAlignment_Right));
  text_line_alignment_lib.Register ("get_Top",      make_const (TextLineAlignment_Top));
  text_line_alignment_lib.Register ("get_Bottom",   make_const (TextLineAlignment_Bottom));
  text_line_alignment_lib.Register ("get_BaseLine", make_const (TextLineAlignment_BaseLine));
}

void bind_text_line_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_TEXT_LINE_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация статических переменных

  bind_static_text_line_library (environment);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_text_line));

    //регистрация операций

  lib.Register ("set_Text",                make_invoker (implicit_cast<void (TextLine::*) (const char*)> (&TextLine::SetText)));
  lib.Register ("get_Text",                make_invoker (&TextLine::Text));
  lib.Register ("get_TextLength",          make_invoker (&TextLine::TextLength));
  lib.Register ("set_TextUnicode",         make_invoker (implicit_cast<void (TextLine::*) (const wchar_t*)> (&TextLine::SetText)));
  lib.Register ("get_TextUnicode",         make_invoker (&TextLine::TextUnicode));
  lib.Register ("set_Font",                make_invoker (&TextLine::SetFont));
  lib.Register ("get_Font",                make_invoker (&TextLine::Font));
  lib.Register ("set_Color",               make_invoker (implicit_cast<void (TextLine::*) (const vec4f&)> (&TextLine::SetColor)));
  lib.Register ("get_Color",               make_invoker (&TextLine::Color));
  lib.Register ("set_HorizontalAlignment", make_invoker (&TextLine::SetHorizontalAlignment));
  lib.Register ("get_HorizontalAlignment", make_invoker (&TextLine::HorizontalAlignment));
  lib.Register ("set_VerticalAlignment",   make_invoker (&TextLine::SetVerticalAlignment));
  lib.Register ("get_VerticalAlignment",   make_invoker (&TextLine::VerticalAlignment));

  lib.Register ("SetColor", make_invoker (make_invoker (implicit_cast<void (TextLine::*) (float, float, float, float)> (&TextLine::SetColor)),
                                          make_invoker (implicit_cast<void (TextLine::*) (float, float, float)>        (&TextLine::SetColor))));
  lib.Register ("SetCharsColorFactors", make_invoker (&TextLine::SetCharsColorFactors));
  lib.Register ("CharColor", make_invoker (&TextLine::CharColor));

  lib.Register ("SetAlignment", make_invoker (&TextLine::SetAlignment));

  environment.RegisterType<TextLine> (SCENE_TEXT_LINE_LIBRARY);
}

/*
    Создание модели
*/

VisualModel::Pointer create_visual_model ()
{
  return VisualModel::Create ();
}

/*
   Регистрация библиотеки работы с моделями
*/

void bind_visual_model_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_VISUAL_MODEL_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_visual_model));

    //регистрация операций

  lib.Register ("set_MeshName", make_invoker (&VisualModel::SetMeshName));
  lib.Register ("get_MeshName", make_invoker (&VisualModel::MeshName));

  environment.RegisterType<VisualModel> (SCENE_VISUAL_MODEL_LIBRARY);
}

/*
    Создание карты высот
*/

HeightMap::Pointer create_height_map ()
{
  return HeightMap::Create ();
}

void set_height_map_cell_height (HeightMap& map, size_t row, size_t column, float height)
{
  map.Vertex (row, column).height = height;
}

float get_height_map_cell_height (HeightMap& map, size_t row, size_t column)
{
  return map.Vertex (row, column).height;
}

void set_height_map_cell_normal (HeightMap& map, size_t row, size_t column, const math::vec3f& normal)
{
  map.Vertex (row, column).normal = normal;
}

const math::vec3f& get_height_map_cell_normal (HeightMap& map, size_t row, size_t column)
{
  return map.Vertex (row, column).normal;
}

void set_height_map_cell_color (HeightMap& map, size_t row, size_t column, const math::vec4f& color)
{
  map.Vertex (row, column).color = color;
}

math::vec4f& get_height_map_cell_color (HeightMap& map, size_t row, size_t column)
{
  return map.Vertex (row, column).color;
}

/*
   Регистрация библиотеки работы с картами высот
*/

void bind_height_map_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_HEIGHT_MAP_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_height_map));

    //регистрация операций
    
  lib.Register ("set_Material", make_invoker (&HeightMap::SetMaterial));
  lib.Register ("get_Material", make_invoker (&HeightMap::Material));

  lib.Register ("set_RowsCount",    make_invoker (&HeightMap::SetRowsCount));
  lib.Register ("set_ColumnsCount", make_invoker (&HeightMap::SetColumnsCount));
  lib.Register ("get_RowsCount",    make_invoker (&HeightMap::RowsCount));
  lib.Register ("get_ColumnsCount", make_invoker (&HeightMap::ColumnsCount));    

  lib.Register ("SetCellsCount",     make_invoker (&HeightMap::SetCellsCount));
  lib.Register ("SetVerticesHeight", make_invoker (&HeightMap::SetVerticesHeight));
  lib.Register ("SetVerticesNormal", make_invoker (&HeightMap::SetVerticesNormal));
  lib.Register ("SetVerticesColor",  make_invoker (&HeightMap::SetVerticesColor));

  lib.Register ("SetVertexHeight", make_invoker (&set_height_map_cell_height));
  lib.Register ("GetVertexHeight", make_invoker (&get_height_map_cell_height));
  lib.Register ("SetVertexNormal", make_invoker (&set_height_map_cell_normal));
  lib.Register ("GetVertexNormal", make_invoker (&get_height_map_cell_normal));
  lib.Register ("SetVertexColor",  make_invoker (&set_height_map_cell_color));
  lib.Register ("GetVertexColor",  make_invoker (&get_height_map_cell_color));

  environment.RegisterType<HeightMap> (SCENE_HEIGHT_MAP_LIBRARY);
}

}

}
