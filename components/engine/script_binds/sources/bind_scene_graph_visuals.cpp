#include "scene_graph.h"

using namespace scene_graph;

template class engine::decl_sg_cast<SpriteModel, VisualModel>;
template class engine::decl_sg_cast<SpriteModel, Entity>;
template class engine::decl_sg_cast<SpriteModel, Node>;
template class engine::decl_sg_cast<Sprite,      SpriteModel>;
template class engine::decl_sg_cast<Sprite,      VisualModel>;
template class engine::decl_sg_cast<Sprite,      Entity>;
template class engine::decl_sg_cast<Sprite,      Node>;
template class engine::decl_sg_cast<SpriteList,  SpriteModel>;
template class engine::decl_sg_cast<SpriteList,  VisualModel>;
template class engine::decl_sg_cast<SpriteList,  Entity>;
template class engine::decl_sg_cast<SpriteList,  Node>;
template class engine::decl_sg_cast<LineModel,   VisualModel>;
template class engine::decl_sg_cast<LineModel,   Entity>;
template class engine::decl_sg_cast<LineModel,   Node>;
template class engine::decl_sg_cast<Line,        LineModel>;
template class engine::decl_sg_cast<Line,        VisualModel>;
template class engine::decl_sg_cast<Line,        Entity>;
template class engine::decl_sg_cast<Line,        Node>;
template class engine::decl_sg_cast<LineList,    LineModel>;
template class engine::decl_sg_cast<LineList,    VisualModel>;
template class engine::decl_sg_cast<LineList,    Entity>;
template class engine::decl_sg_cast<LineList,    Node>;
template class engine::decl_sg_cast<TextLine,    VisualModel>;
template class engine::decl_sg_cast<TextLine,    Entity>;
template class engine::decl_sg_cast<TextLine,    Node>;
template class engine::decl_sg_cast<HeightMap,   VisualModel>;
template class engine::decl_sg_cast<HeightMap,   Entity>;
template class engine::decl_sg_cast<HeightMap,   Node>;
template class engine::decl_sg_cast<StaticMesh,  VisualModel>;
template class engine::decl_sg_cast<StaticMesh,  Entity>;
template class engine::decl_sg_cast<StaticMesh,  Node>;

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

void bind_static_sprite_model_library (Environment& environment)
{
  InvokerRegistry sprite_mode_lib  = environment.CreateLibrary (SCENE_STATIC_SPRITE_MODE_LIBRARY);
  InvokerRegistry sprite_usage_lib = environment.CreateLibrary (SCENE_STATIC_SPRITE_USAGE_LIBRARY);

  sprite_mode_lib.Register ("get_Billboard",          make_const (SpriteMode_Billboard));
  sprite_mode_lib.Register ("get_Oriented",           make_const (SpriteMode_Oriented));
  sprite_mode_lib.Register ("get_OrientedBillboard",  make_const (SpriteMode_OrientedBillboard));

  sprite_usage_lib.Register ("get_Static",   make_const (SpriteUsage_Static));
  sprite_usage_lib.Register ("get_Dynamic",  make_const (SpriteUsage_Dynamic));
  sprite_usage_lib.Register ("get_Stream",   make_const (SpriteUsage_Stream));
  sprite_usage_lib.Register ("get_Batching", make_const (SpriteUsage_Batching));

  environment.RegisterType<SpriteMode>  (SCENE_STATIC_SPRITE_MODE_LIBRARY);
  environment.RegisterType<SpriteUsage> (SCENE_STATIC_SPRITE_USAGE_LIBRARY);
}

void bind_sprite_model_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_SPRITE_MODEL_LIBRARY);

    //регистрация статических библиотек

  bind_static_sprite_model_library (environment);

    //наследование

  lib.Register (environment, SCENE_VISUAL_MODEL_LIBRARY);

    //регистрация операций

  lib.Register ("set_Material", make_invoker (&SpriteModel::SetMaterial));
  lib.Register ("get_Material", make_invoker (&SpriteModel::Material));
  lib.Register ("set_Batch",    make_invoker (&SpriteModel::SetBatch));
  lib.Register ("get_Batch",    make_invoker (&SpriteModel::Batch));
  lib.Register ("set_Mode",     make_invoker (&SpriteModel::SetMode));
  lib.Register ("get_Mode",     make_invoker (&SpriteModel::Mode));
  lib.Register ("set_Usage",    make_invoker (&SpriteModel::SetUsage));
  lib.Register ("get_Usage",    make_invoker (&SpriteModel::Usage));
  lib.Register ("set_OrtUp",    make_invoker (&SpriteModel::SetOrtUp));
  lib.Register ("get_OrtUp",    make_invoker (&SpriteModel::OrtUp));

    //регистрация типов данных

  environment.RegisterType<SpriteModel> (SCENE_SPRITE_MODEL_LIBRARY);
}

/*
     Регистрация библиотеки работы с спрайтами
*/

void bind_sprite_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_SPRITE_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_SPRITE_MODEL_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_sprite));

    //регистрация операций

  lib.Register ("set_TexOffset",  make_invoker (&Sprite::SetTexOffset));
  lib.Register ("get_TexOffset",  make_invoker (&Sprite::TexOffset));
  lib.Register ("set_TexSize",    make_invoker (&Sprite::SetTexSize));
  lib.Register ("get_TexSize",    make_invoker (&Sprite::TexSize));
  lib.Register ("set_Color",      make_invoker (implicit_cast<void (Sprite::*) (const vec4f&)> (&Sprite::SetColor)));
  lib.Register ("set_Alpha",      make_invoker (&Sprite::SetAlpha));
  lib.Register ("get_Color",      make_invoker (&Sprite::Color));
  lib.Register ("get_Alpha",      make_invoker (&Sprite::Alpha));

  lib.Register ("SetColor", make_invoker (make_invoker (implicit_cast<void (Sprite::*) (float, float, float, float)> (&Sprite::SetColor)),
                                          make_invoker (implicit_cast<void (Sprite::*) (float, float, float)>        (&Sprite::SetColor))));

    //регистрация типов данных

  environment.RegisterType<Sprite> (SCENE_SPRITE_LIBRARY);
}

/*
    Создание линии
*/

Line::Pointer create_line ()
{
  return Line::Create ();
}

LineList::Pointer create_line_list ()
{
  return LineList::Create ();
}

/*
     Регистрация библиотеки работы с спрайтовыми моделями
*/

void bind_static_line_model_library (Environment& environment)
{
  InvokerRegistry line_usage_lib = environment.CreateLibrary (SCENE_STATIC_LINE_USAGE_LIBRARY);

  line_usage_lib.Register ("get_Static",   make_const (LineUsage_Static));
  line_usage_lib.Register ("get_Dynamic",  make_const (LineUsage_Dynamic));
  line_usage_lib.Register ("get_Stream",   make_const (LineUsage_Stream));
  line_usage_lib.Register ("get_Batching", make_const (LineUsage_Batching));

  environment.RegisterType<LineUsage> (SCENE_STATIC_LINE_USAGE_LIBRARY);
}

void bind_line_model_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_LINE_MODEL_LIBRARY);

    //регистрация статических библиотек

  bind_static_line_model_library (environment);

    //наследование

  lib.Register (environment, SCENE_VISUAL_MODEL_LIBRARY);

    //регистрация операций

  lib.Register ("set_Material", make_invoker (&LineModel::SetMaterial));
  lib.Register ("get_Material", make_invoker (&LineModel::Material));
  lib.Register ("set_Batch",    make_invoker (&LineModel::SetBatch));
  lib.Register ("get_Batch",    make_invoker (&LineModel::Batch));
  lib.Register ("set_Usage",    make_invoker (&LineModel::SetUsage));
  lib.Register ("get_Usage",    make_invoker (&LineModel::Usage));

    //регистрация типов данных

  environment.RegisterType<LineModel> (SCENE_LINE_MODEL_LIBRARY);
}

/*
     Регистрация библиотеки работы с линиями
*/

void bind_line_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_LINE_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_LINE_MODEL_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_line));

    //регистрация операций

  lib.Register ("set_TexOffset",  make_invoker (&Line::SetTexOffset));
  lib.Register ("get_TexOffset",  make_invoker (&Line::TexOffset));
  lib.Register ("set_Color0",     make_invoker<void (Line*, const vec4f&)> (xtl::bind (implicit_cast<void (Line::*) (size_t, const vec4f&)> (&Line::SetColor), _1, 0, _2)));
  lib.Register ("set_Alpha0",     make_invoker<void (Line*, float)> (xtl::bind (&Line::SetAlpha, _1, 0, _2)));
  lib.Register ("get_Color0",     make_invoker<const vec4f& (const Line*)> (xtl::bind (&Line::Color, _1, 0)));
  lib.Register ("get_Alpha0",     make_invoker<float (const Line*)> (xtl::bind (&Line::Alpha, _1, 0)));
  lib.Register ("set_Color1",     make_invoker<void (Line*, const vec4f&)> (xtl::bind (implicit_cast<void (Line::*) (size_t, const vec4f&)> (&Line::SetColor), _1, 1, _2)));
  lib.Register ("set_Alpha1",     make_invoker<void (Line*, float)> (xtl::bind (&Line::SetAlpha, _1, 1, _2)));
  lib.Register ("get_Color1",     make_invoker<const vec4f& (const Line*)> (xtl::bind (&Line::Color, _1, 1)));
  lib.Register ("get_Alpha1",     make_invoker<float (const Line*)> (xtl::bind (&Line::Alpha, _1, 1)));

  lib.Register ("SetAlpha", make_invoker (&Line::SetAlpha));
  lib.Register ("SetColor", make_invoker (make_invoker (implicit_cast<void (Line::*) (size_t, float, float, float, float)> (&Line::SetColor)),
                                          make_invoker (implicit_cast<void (Line::*) (size_t, float, float, float)>        (&Line::SetColor)),
                                          make_invoker (implicit_cast<void (Line::*) (size_t, const math::vec4f&)>         (&Line::SetColor))));

    //регистрация типов данных

  environment.RegisterType<Line> (SCENE_LINE_LIBRARY);
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
  InvokerRegistry text_line_alignment_lib = environment.CreateLibrary (SCENE_STATIC_TEXT_LINE_ALIGNMENT_LIBRARY);

  text_line_alignment_lib.Register ("get_Center",   make_const (TextLineAlignment_Center));
  text_line_alignment_lib.Register ("get_Left",     make_const (TextLineAlignment_Left));
  text_line_alignment_lib.Register ("get_Right",    make_const (TextLineAlignment_Right));
  text_line_alignment_lib.Register ("get_Top",      make_const (TextLineAlignment_Top));
  text_line_alignment_lib.Register ("get_Bottom",   make_const (TextLineAlignment_Bottom));
  text_line_alignment_lib.Register ("get_BaseLine", make_const (TextLineAlignment_BaseLine));
}

void bind_text_line_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_TEXT_LINE_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_VISUAL_MODEL_LIBRARY);

    //регистрация статических переменных

  bind_static_text_line_library (environment);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_text_line));

    //регистрация операций

  lib.Register ("set_Text",                make_invoker (implicit_cast<void (TextLine::*) (const char*)> (&TextLine::SetTextUtf8)));
  lib.Register ("get_Text",                make_invoker (&TextLine::TextUtf8));
  lib.Register ("get_TextLength",          make_invoker (&TextLine::TextLength));
  lib.Register ("set_Font",                make_invoker (&TextLine::SetFont));
  lib.Register ("get_Font",                make_invoker (&TextLine::Font));
  lib.Register ("set_Color",               make_invoker (implicit_cast<void (TextLine::*) (const vec4f&)> (&TextLine::SetColor)));
  lib.Register ("get_Color",               make_invoker (&TextLine::Color));
  lib.Register ("set_HorizontalAlignment", make_invoker (&TextLine::SetHorizontalAlignment));
  lib.Register ("get_HorizontalAlignment", make_invoker (&TextLine::HorizontalAlignment));
  lib.Register ("set_VerticalAlignment",   make_invoker (&TextLine::SetVerticalAlignment));
  lib.Register ("get_VerticalAlignment",   make_invoker (&TextLine::VerticalAlignment));
  lib.Register ("set_SpacingMultiplier",   make_invoker (&TextLine::SetSpacingMultiplier));
  lib.Register ("get_SpacingMultiplier",   make_invoker (&TextLine::SpacingMultiplier));

  lib.Register ("SetColor", make_invoker (make_invoker (implicit_cast<void (TextLine::*) (float, float, float, float)> (&TextLine::SetColor)),
                                          make_invoker (implicit_cast<void (TextLine::*) (float, float, float)>        (&TextLine::SetColor))));
  lib.Register ("SetCharsColorFactors", make_invoker (&TextLine::SetCharsColorFactors));
  lib.Register ("CharColor",            make_invoker (&TextLine::CharColor));
  lib.Register ("CharColorFactor",      make_invoker (&TextLine::CharColorFactor));

  lib.Register ("SetAlignment", make_invoker (&TextLine::SetAlignment));

  environment.RegisterType<TextLine> (SCENE_TEXT_LINE_LIBRARY);
}

/*
    Создание модели
*/

StaticMesh::Pointer create_static_mesh ()
{
  return StaticMesh::Create ();
}

/*
   Регистрация библиотеки работы с моделями
*/

void bind_static_mesh_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_STATIC_MESH_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_VISUAL_MODEL_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_static_mesh));

    //регистрация операций

  lib.Register ("set_MeshName", make_invoker (&StaticMesh::SetMeshName));
  lib.Register ("get_MeshName", make_invoker (&StaticMesh::MeshName));

  environment.RegisterType<StaticMesh> (SCENE_STATIC_MESH_LIBRARY);
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
  InvokerRegistry lib = environment.CreateLibrary (SCENE_HEIGHT_MAP_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_VISUAL_MODEL_LIBRARY);

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
