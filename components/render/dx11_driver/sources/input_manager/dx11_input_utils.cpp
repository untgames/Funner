#include "shared.h"

namespace render
{

namespace low_level
{

namespace dx11
{

namespace
{

//сопоставление семантики и её имени
struct Semantic2Name
{
  VertexAttributeSemantic semantic; //семантика
  const char*             name;     //имя
};

const Semantic2Name semantic_names [] = {
  {VertexAttributeSemantic_Position,  "Position"},
  {VertexAttributeSemantic_Normal,    "Normal"},
  {VertexAttributeSemantic_Color,     "Color"},
  {VertexAttributeSemantic_TexCoord0, "TexCoord0"},
  {VertexAttributeSemantic_TexCoord1, "TexCoord1"},
  {VertexAttributeSemantic_TexCoord2, "TexCoord2"},
  {VertexAttributeSemantic_TexCoord3, "TexCoord3"},
  {VertexAttributeSemantic_TexCoord4, "TexCoord4"},
  {VertexAttributeSemantic_TexCoord5, "TexCoord5"},
  {VertexAttributeSemantic_TexCoord6, "TexCoord6"},
  {VertexAttributeSemantic_TexCoord7, "TexCoord7"},
};

const size_t semantic_names_count = sizeof (semantic_names) / sizeof (*semantic_names);

}

//получение семантики по имени
VertexAttributeSemantic get_semantic_by_name (const char* name)
{
  static const char* METHOD_NAME = "render::low_level::dx11::get_semantic_by_name";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  const Semantic2Name* iter = semantic_names;

  for (size_t i=0; i<semantic_names_count; i++, iter++)
    if (!strcmp (iter->name, name))
      return iter->semantic;

  return (VertexAttributeSemantic)-1;
}

//получение имени семантики
const char* get_semantic_name (VertexAttributeSemantic semantic)
{
  static const char* METHOD_NAME = "render::low_level::dx11::get_semantic_name";

  const Semantic2Name* iter = semantic_names;

  for (size_t i=0; i<semantic_names_count; i++, iter++)
    if (semantic == iter->semantic)
      return iter->name;

  throw xtl::make_argument_exception (METHOD_NAME, "semantic", semantic);
}

}

}

}
