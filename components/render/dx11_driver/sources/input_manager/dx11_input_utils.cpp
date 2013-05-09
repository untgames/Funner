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
  {VertexAttributeSemantic_Position,  "POSITION"},
  {VertexAttributeSemantic_Normal,    "NORMAL"},
  {VertexAttributeSemantic_Color,     "COLOR"},
  {VertexAttributeSemantic_TexCoord0, "TEXCOORD0"},
  {VertexAttributeSemantic_TexCoord1, "TEXCOORD1"},
  {VertexAttributeSemantic_TexCoord2, "TEXCOORD2"},
  {VertexAttributeSemantic_TexCoord3, "TEXCOORD3"},
  {VertexAttributeSemantic_TexCoord4, "TEXCOORD4"},
  {VertexAttributeSemantic_TexCoord5, "TEXCOORD5"},
  {VertexAttributeSemantic_TexCoord6, "TEXCOORD6"},
  {VertexAttributeSemantic_TexCoord7, "TEXCOORD7"},
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
